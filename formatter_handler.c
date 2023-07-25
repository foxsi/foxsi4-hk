#include "formatter_handler.h"
#include "parameters.h"


int ether_resolver(tcpTCB_t *port, uint8_t ether_msg[], unsigned int len) {
    if(ether_msg[0] == FMTR_POWER_CS) {
        return power_handler(&port, ether_msg, len);
    } else if(ether_msg[0] == FMTR_SENSE_CS) {
        return -1;
    } else {
        return -1;
    }
}



int power_handler(tcpTCB_t *port, uint8_t ether_msg[], unsigned int len) {
    
    if(len < 2) {
        // somehow deal with error
        return -1;
    }
    
    if(ether_msg[0] != FMTR_POWER_CS) {
        // this function shouldn't have been called in the first place
        return -1;
    }
    
    uint8_t read = 0x80 & ether_msg[1];
    uint8_t cdte_broadcast = FMTR_POWER_BROADCAST_CDTE & ether_msg[1];
    uint8_t cmos_broadcast = FMTR_POWER_BROADCAST_CMOS & ether_msg[1];
    if(read == 0x80) {
        // read exchange with MAX7317
        LATEbits.LATE2 = 0;
        uint8_t spi_return_buff;
        spi_return_buff = SPI1_ExchangeByte(ether_msg[1]);
        uint16_t send_len = 1;
        LATEbits.LATE2 = 1;
        
        TCP_Send(&port, spi_return_buff, send_len);
        // need some way to return spi_return_buff value.
        return 1;
        
    } else {
        if(len < 3) {
            // need output level information to send
            return -1;
        }
        
        if(cdte_broadcast == 0xff) {
            // turn on or off all cdte registers
            LATEbits.LATE2 = 0;
            uint8_t spi_send_fmtr_buff[2] = {FMTR_POWER_PORT_CDTEDE, ether_msg[2]};
            SPI1_WriteBlock(spi_send_fmtr_buff, 2);
            LATEbits.LATE2 = 1;
            
            LATEbits.LATE2 = 0;
            spi_send_fmtr_buff[0] = FMTR_POWER_PORT_CDTE1;
            spi_send_fmtr_buff[1] = ether_msg[2];
            SPI1_WriteBlock(spi_send_fmtr_buff, 2);
            LATEbits.LATE2 = 1;
            
            LATEbits.LATE2 = 0;
            spi_send_fmtr_buff[0] = FMTR_POWER_PORT_CDTE2;
            spi_send_fmtr_buff[1] = ether_msg[2];
            LATEbits.LATE2 = 1;
            
            LATEbits.LATE2 = 0;
            spi_send_fmtr_buff[0] = FMTR_POWER_PORT_CDTE3;
            spi_send_fmtr_buff[1] = ether_msg[2];
            LATEbits.LATE2 = 1;
            
            LATEbits.LATE2 = 0;
            spi_send_fmtr_buff[0] = FMTR_POWER_PORT_CDTE4;
            spi_send_fmtr_buff[1] = ether_msg[2];
            LATEbits.LATE2 = 1;
        }
        
        if(cmos_broadcast == 0xff) {
            // turn on or off all cmos detectors
            LATEbits.LATE2 = 0;
            uint8_t spi_send_fmtr_buff[2] = {FMTR_POWER_PORT_CMOS1, ether_msg[2]};
            SPI1_WriteBlock(spi_send_fmtr_buff, 2);
            LATEbits.LATE2 = 1;
            
            LATEbits.LATE2 = 0;
            spi_send_fmtr_buff[0] = FMTR_POWER_PORT_CMOS2;
            spi_send_fmtr_buff[1] = ether_msg[2];
            LATEbits.LATE2 = 1;
        }
        
        // just write to MAX7317
        LATEbits.LATE2 = 0;
        uint8_t spi_send_fmtr_buff[2] = {ether_msg[1], ether_msg[2]};
        SPI1_WriteBlock(spi_send_fmtr_buff, 2);
        LATEbits.LATE2 = 1;
        return 1;
    }
    
}
