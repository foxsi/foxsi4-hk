//#include "mcc_generated_files/TCPIPLibrary/tcpv4.h"
//#include "mcc_generated_files/spi1.h"
//#include "mcc_generated_files/mcc.h"
#include "formatter_interface.h"

void ethernet_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {
    
    // handle received data
    if (recv_size > 0) {
        switch (recv_buff[0]) {
            case FOXSI_POWER_SWITCH:
                SPI1_Open(SPI1_DEFAULT_MODE0);
                power_switch_handler(port, recv_buff, recv_size);
                SPI1_Close();
                break;
            case FOXSI_POWER_HEALTH:
                SPI1_Open(SPI1_DEFAULT_MODE3);
                power_health_handler(port, recv_buff, recv_size);
                SPI1_Close();
                break;
            case FOXSI_RTD1:
                SPI1_Open(SPI1_DEFAULT_MODE0);
                rtd1_handler(port, recv_buff, recv_size);
                SPI1_Close();
                break;
            case FOXSI_RTD2:
                SPI1_Open(SPI1_DEFAULT_MODE0);
                rtd2_handler(port, recv_buff, recv_size);
                SPI1_Close();
                break;
            case FOXSI_INTRO:
                introspect_handler(port, recv_buff, recv_size);
                break;
            default:
                // should not end up here
                FOXSI_ERRORS |= FOXSI_ERROR_ETH_RECV_MSB;
                break;
        }
    } else {
        // should not end up here
        FOXSI_ERRORS |= FOXSI_ERROR_ETH_RECV_MSB;
        
    }
}

void power_switch_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {    
    // use pin RE2 for the power switch CS
    if (recv_size > 1) {
        if (recv_buff[1] > 0x0d) {
            // handle read command
            LATEbits.LATE2 = 0;
            SPI1_ExchangeBlock(recv_buff + 1, recv_size - 1);
            LATEbits.LATE2 = 1;
            TCP_Send(port, recv_buff, 1);
        } else {
            // bring CS low:
            LATEbits.LATE2 = 0;
            SPI1_WriteBlock(recv_buff + 1, recv_size - 1);
//            for (int i = 1; i < recv_size; ++i) {
//                SPI1_WriteByte(recv_buff[i]);
//            }
            LATEbits.LATE2 = 1;
        }    
    } else {
        // should not end up here
        FOXSI_ERRORS |= FOXSI_ERROR_POWER_SWITCH;
    }
}

void power_health_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {
    switch (recv_buff[1]) {
        case FOXSI_POWER_HEALTH_READ_ALL: {
            uint8_t response[32]; // 2B per VIN, 16 VINs.
            uint8_t spi_tx_buff[2];
            
            uint16_t ctrl = FOXSI_POWER_HEALTH_CONTROL_MASK | power_health_convert_addr(0x00);
            bytes_from_uint16_t(ctrl, spi_tx_buff);
            
            // read 0x00 
            LATEbits.LATE3 = 0;
            SPI1_WriteBlock(spi_tx_buff, 2);
            LATEbits.LATE3 = 1;
            
            for (uint8_t i = 1; i <= 0x0f; ++i) {
                ctrl = FOXSI_POWER_HEALTH_CONTROL_MASK | power_health_convert_addr(i);
                bytes_from_uint16_t(ctrl, spi_tx_buff);
                uint8_t response_index = 2*i - 2;
                
                if (i == 0x0f) {            // send this one twice.
                    
//                    ctrl = FOXSI_POWER_HEALTH_CONTROL_MASK | power_health_convert_addr(i);
//                    bytes_from_uint16_t(ctrl, spi_tx_buff);
                    
                    // first send
                    LATEbits.LATE3 = 0;
                    SPI1_ExchangeBlock(spi_tx_buff, 2);
                    LATEbits.LATE3 = 1;
                    __delay_us(FOXSI_POWER_HEALTH_MIN_DELAY_US);
                    
                    response[response_index + 1] = spi_tx_buff[1];
                    response[response_index] = spi_tx_buff[0];
                    
                    
                    // reapply the tx control packet mask
                    ctrl = FOXSI_POWER_HEALTH_CONTROL_MASK | power_health_convert_addr(i);
                    bytes_from_uint16_t(ctrl, spi_tx_buff);
                    
                    // second send. now can read back ADC
                    LATEbits.LATE3 = 0;
                    SPI1_ExchangeBlock(spi_tx_buff, 2);
                    LATEbits.LATE3 = 1;
                    __delay_us(FOXSI_POWER_HEALTH_MIN_DELAY_US);
                    
                    response_index += 2;
                    response[response_index + 1] = spi_tx_buff[1];
                    response[response_index] = spi_tx_buff[0];
                    
                } else {                    // send these once.
                    LATEbits.LATE3 = 0;
                    SPI1_ExchangeBlock(spi_tx_buff, 2);
                    LATEbits.LATE3 = 1;
                    // add to eth TX buffer
                    response[response_index + 1] = spi_tx_buff[1];
                    response[response_index] = spi_tx_buff[0];
                }
            }
            
            // send all response data on ethernet
            TCP_Send(port, response, 32);
            break;
        }
        case FOXSI_POWER_HEALTH_FORWARD: {
            
            break;
        }
        case FOXSI_POWER_HEALTH_SETUP: {
            power_health_spi_setup();
            break;
        }
        default: {
            uint16_t ctrl;
            ctrl = FOXSI_POWER_HEALTH_CONTROL_MASK | power_health_convert_addr(recv_buff[1]);
            // this is 2B wide. 
            uint8_t spi_tx_buff[2];
            bytes_from_uint16_t(ctrl, spi_tx_buff);
            
            uint8_t response[2];
            
            // first send
            LATEbits.LATE3 = 0;
            SPI1_WriteBlock(spi_tx_buff, 2);
            LATEbits.LATE3 = 1;
            __delay_us(FOXSI_POWER_HEALTH_MIN_DELAY_US);
            
            // second send
            LATEbits.LATE3 = 0;
            SPI1_ExchangeBlock(spi_tx_buff, 2);
            LATEbits.LATE3 = 1;
            
            // build Ethernet payload (MSB, LSB order)
//            response[1] = spi_tx_buff & 0xff;
//            response[0] = (spi_tx_buff >> 8) & 0xff;
            response[1] = spi_tx_buff[1];
            response[0] = spi_tx_buff[0];
            
            // send on Ethernet
            TCP_Send(port, response, 2);
            
            break;
        }
    }
}

void power_health_spi_setup() {
    uint8_t setup_frame[2] = {0xff, 0xff};
    // send setup frame twice to clear
    LATAbits.LATA5 = 1;
    
    LATEbits.LATE3 = 0;
    SPI1_WriteBlock(setup_frame, 2);
    LATEbits.LATE3 = 1;
    __delay_us(FOXSI_POWER_HEALTH_MIN_DELAY_US);
    LATEbits.LATE3 = 0;
    SPI1_WriteBlock(setup_frame, 2);
    LATEbits.LATE3 = 1;
}

uint16_t power_health_convert_addr(uint8_t recv_addr) {
    uint16_t result = (uint16_t)recv_addr << 10;
    return result; // << 10;
}

void rtd1_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {
    switch (recv_buff[1]) {
        case FOXSI_RTD_SETUP: {
            rtd_setup(FOXSI_RTD1);
            break;
        }
        case FOXSI_RTD_START_ALL_CONV: {
            rtd_start_all_conversion(FOXSI_RTD1);
            break;
        }
        case FOXSI_RTD_READ_ALL: {
            rtd_read_all(port, FOXSI_RTD1);
            break;
        }
        default: {
            if (1 < recv_buff[1] && recv_buff[1] < 11) {
                
            } else {
                // invalid CH identifier.
            }
        }
    }
}

void rtd2_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {
    switch (recv_buff[1]) {
        case FOXSI_RTD_SETUP: {
            rtd_setup(FOXSI_RTD2);
            break;
        }
        case FOXSI_RTD_START_ALL_CONV: {
            rtd_start_all_conversion(FOXSI_RTD2);
            break;
        }
        case FOXSI_RTD_READ_ALL: {
            rtd_read_all(port, FOXSI_RTD2);
            break;
        }
        default: {
            if (1 < recv_buff[1] && recv_buff[1] < 11) {
                
            } else {
                // invalid CH identifier.
                FOXSI_ERRORS |= FOXSI_ERROR_RTD1;
            }
        }
    }
}

void rtd_setup(uint8_t rtd_num) {
    // argument: FOXSI_RTD1 or FOXSI_RTD2
    
    uint8_t spi_tx_buff[7];
    spi_tx_buff[0] = FOXSI_RTD_WR_CMD;
    bytes_from_uint16_t(FOXSI_RTD_CONF_SENSE_ADDR, spi_tx_buff + 1);
    bytes_from_uint32_t(FOXSI_RTD_CONF_SENSE, spi_tx_buff + 3);
    
    if (rtd_num == FOXSI_RTD1) {
        LATEbits.LATE0 = 0;
        SPI1_WriteBlock(spi_tx_buff, 7);
        LATEbits.LATE0 = 1;
        
        for (int i = 0; i < FOXSI_RTD_COUNT; ++i) {
            
            // substitute in memory address for channel config
            bytes_from_uint16_t(FOXSI_RTD_CONF_RTD_ADDR + 8*i, spi_tx_buff + 1);
            // substitute in channel config data
            bytes_from_uint32_t(FOXSI_RTD_CONF_RTD, spi_tx_buff + 3);
            
            LATEbits.LATE0 = 0;
            SPI1_WriteBlock(spi_tx_buff, 7);
            LATEbits.LATE0 = 1;
        }
    } else if (rtd_num == FOXSI_RTD2) {
        LATEbits.LATE1 = 0;
        SPI1_WriteBlock(spi_tx_buff, 7);
        LATEbits.LATE1 = 1;
        
        for (int i = 0; i < FOXSI_RTD_COUNT; ++i) {
            
            // substitute in memory address for channel config
            bytes_from_uint16_t(FOXSI_RTD_CONF_RTD_ADDR + 8*i, spi_tx_buff + 1);
            // substitute in channel config data
            bytes_from_uint32_t(FOXSI_RTD_CONF_RTD, spi_tx_buff + 3);
            
            LATEbits.LATE1 = 0;
            SPI1_WriteBlock(spi_tx_buff, 7);
            LATEbits.LATE1 = 1;
        }
    } else {
        // shouldn't end up here.
        FOXSI_ERRORS |= FOXSI_ERROR_RTD;
    }
}

void rtd_start_all_conversion(uint8_t rtd_num) {
    uint8_t spi_tx_buff[6];
    uint8_t seq_conv_pad[4];
    spi_tx_buff[0] = FOXSI_RTD_WR_CMD;
    bytes_from_uint16_t(FOXSI_RTD_SEQ_CONV_ADDR, spi_tx_buff + 1);
    // remove empty first byte (data to write is 3 B long)
    bytes_from_uint32_t(FOXSI_RTD_SEQ_CONV, seq_conv_pad);
    spi_tx_buff[3] = seq_conv_pad[1];
    spi_tx_buff[4] = seq_conv_pad[2];
    spi_tx_buff[5] = seq_conv_pad[3];
    
    uint8_t command_tx_buff[4];
    command_tx_buff[0] = FOXSI_RTD_WR_CMD;
    command_tx_buff[1] = 0x00;
    command_tx_buff[2] = 0x00;
    command_tx_buff[3] = 0x80;  // register value to convert multiple channels
    
    if (rtd_num == FOXSI_RTD1) {
        LATEbits.LATE0 = 0;
        SPI1_WriteBlock(spi_tx_buff, 6);
        LATEbits.LATE0 = 1;
        
        __delay_us(FOXSI_RTD_MIN_DELAY_US);
        
        LATEbits.LATE0 = 0;
        SPI1_WriteBlock(command_tx_buff, 4);
        LATEbits.LATE0 = 1;
        
    } else if (rtd_num == FOXSI_RTD2) {
        LATEbits.LATE1 = 0;
        SPI1_WriteBlock(spi_tx_buff, 6);
        LATEbits.LATE1 = 1;
        
        __delay_us(FOXSI_RTD_MIN_DELAY_US);
        
        LATEbits.LATE1 = 0;
        SPI1_WriteBlock(command_tx_buff, 4);
        LATEbits.LATE1 = 1;
        
    } else {
        // shouldn't end up here.
        FOXSI_ERRORS |= FOXSI_ERROR_RTD;
    }
}

void rtd_read_all(tcpTCB_t* port, uint8_t rtd_num) {
    uint8_t spi_tx_buff[7];
    uint8_t response[4*9];
    
    if (rtd_num == FOXSI_RTD1) {
        
        // check if INTERRUPT asserted.
        if (PORTDbits.RD0 == 1) {
            for (int i = 0; i < FOXSI_RTD_COUNT; ++i) {
                // assign/reassign value of spi_tx_buff 
                spi_tx_buff[0] = FOXSI_RTD_RD_CMD;
                bytes_from_uint16_t(FOXSI_RTD_RD_ADDR + 8*i, spi_tx_buff + 1);
                for (int j = 3; j < 7; ++j) {
                    spi_tx_buff[j] = 0;
                }

                LATEbits.LATE0 = 0;
                SPI1_ExchangeBlock(spi_tx_buff, 7);
                LATEbits.LATE0 = 1;

                response[4*i] = spi_tx_buff[3];
                response[4*i + 1] = spi_tx_buff[4];
                response[4*i + 2] = spi_tx_buff[5];
                response[4*i + 3] = spi_tx_buff[6];
            }
        }
        
        TCP_Send(port, response, 36);
        
    } else if (rtd_num == FOXSI_RTD2) {
        // check if INTERRUPT asserted.
        if (PORTDbits.RD0 == 1) {
            for (int i = 0; i < FOXSI_RTD_COUNT; ++i) {
                // assign/reassign value of spi_tx_buff 
                spi_tx_buff[0] = FOXSI_RTD_RD_CMD;
                bytes_from_uint16_t(FOXSI_RTD_RD_ADDR + 8*i, spi_tx_buff + 1);
                for (int j = 3; j < 7; ++j) {
                    spi_tx_buff[j] = 0;
                }

                LATEbits.LATE1 = 0;
                SPI1_ExchangeBlock(spi_tx_buff, 7);
                LATEbits.LATE1 = 1;

                response[4*i] = spi_tx_buff[3];
                response[4*i + 1] = spi_tx_buff[4];
                response[4*i + 2] = spi_tx_buff[5];
                response[4*i + 3] = spi_tx_buff[6];
            }
        }
        
        TCP_Send(port, response, 36);
    } else {
        // shouldn't end up here.
        FOXSI_ERRORS |= FOXSI_ERROR_RTD;
    }
    
}

void introspect_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size) {
    if (recv_size > 1) {
        switch (recv_buff[1]) {
            case FOXSI_INTRO_SET_FLIGHT_STATE:
                if (recv_size > 2) {
                    FOXSI_CURRENT_STATE = recv_buff[2];
                } else {
                    // shouldn't end up here.
                    FOXSI_ERRORS |= FOXSI_ERROR_INTRO;
                }
                break;
            case FOXSI_INTRO_SET_UNLAUNCH:
                FOXSI_CURRENT_STATE = FOXSI_FLIGHT_STATE_UNLAUNCH;
                LATGbits.LATG4 = 0;
                
                // todo: set low again ever?
                break;
            case FOXSI_INTRO_SET_ERRORS:
                if (recv_size > 3) {
                    FOXSI_ERRORS = (uint16_t)(recv_buff[2] << 8);
                    FOXSI_ERRORS |= recv_buff[3];
                } else {
                    // shouldn't end up here.
                    FOXSI_ERRORS |= FOXSI_ERROR_INTRO;
                }
                break;
            case FOXSI_INTRO_GET_FLIGHT_STATE: {     
                uint8_t response[2];
                bytes_from_uint16_t(FOXSI_CURRENT_STATE, response);
                // send on Ethernet
                TCP_Send(port, response, 2);
                break;
            }
            case FOXSI_INTRO_GET_ERRORS: {
                uint8_t response[2];
                bytes_from_uint16_t(FOXSI_ERRORS, response);
                TCP_Send(port, response, 2);
                break;
            }
            case FOXSI_INTRO_GET_CURRENT_CLOCK: {
                uint8_t response[4];
                bytes_from_uint32_t(FOXSI_TIME_LONG, response);
                TCP_Send(port, response, 4);
                break;
                
            }
            case FOXSI_INTRO_GET_SYNC_COUNTER: {
                // todo: write it
                
                uint8_t response[32];
                for (int i = 0; i < 32; i += 4) {
                    bytes_from_uint32_t(FOXSI_SYNC_LOG[i/8], response + i);
                }
                TCP_Send(port, response, 32);
                        
                break;
            }
            default:
                FOXSI_ERRORS |= FOXSI_ERROR_INTRO;
                break;
        }
    } else {
        FOXSI_ERRORS |= FOXSI_ERROR_INTRO;
    }
}



void lengthen_time() {
    
    // this should only be called by ISR for SYNC input signal.
//    uint32_t                    FOXSI_TIME_LONG;
//    #define                     FOXSI_SYNC_LOG_SIZE 16
//    uint32_t                    FOXSI_SYNC_LOG[FOXSI_SYNC_LOG_SIZE];
    
    uint16_t sys_last_time = TMR1_ReadTimer();
    
    uint16_t big_old_lsb = FOXSI_TIME_LONG & 0xffff;
    uint16_t big_old_msb = (FOXSI_TIME_LONG >> 16) & 0xffff;
    // timer rolled over:
    if (sys_last_time < big_old_lsb) {
        // increment "rollover counter" part of FOXSI_TIME_LONG (leading 16 b):
        ++big_old_msb;
        
    }
    // set LSB 16b of FOXSI_TIME_LONG to sys_last_time
    FOXSI_TIME_LONG = (uint32_t)(big_old_msb << 16) | (uint32_t)sys_last_time;
    
    // roll FOXSI_SYNC_LOG around
    // (first element is newest, last is oldest)
    for (uint8_t i = FOXSI_SYNC_LOG_SIZE - 1; i > 0; --i) {
        FOXSI_SYNC_LOG[i] = FOXSI_SYNC_LOG[i - 1];
    }
    FOXSI_SYNC_LOG[0] = FOXSI_TIME_LONG;
}


void bytes_from_uint16_t(uint16_t source, uint8_t* store) {
    store[0] = (uint8_t)((source >> 8) & 0xff);
    store[1] = (uint8_t)((source) & 0xff);
}
void bytes_from_uint32_t(uint32_t source, uint8_t* store) {
    store[0] = (source >> 24) & 0xff;
    store[1] = (source >> 16) & 0xff;
    store[2] = (source >> 8) & 0xff;
    store[3] = (source) & 0xff;
}
void swap_byte_order(uint8_t* data, size_t size) {
    uint8_t temp = data[0];
    data[0] = data[size - 1];
    data[size - 1] = temp;
    for (int i = 1; i < size; ++i) {
        data[i] = data[size - i - 1];
    }
}