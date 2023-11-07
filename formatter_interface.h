/* 
 * File:   formatter_interface.h
 * Author: thanasi
 *
 * Created on September 12, 2023, 5:02 PM
 */

#ifndef FORMATTER_INTERFACE_H
#define	FORMATTER_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stddef.h> // for size_t
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/TCPIPLibrary/tcpv4.h"
#include "mcc_generated_files/spi1.h"
    
#define FOXSI_ETHER_BUFF_SIZE           64;
    
// These are SPI CS signal number == SPI REx pin number + 1 == ethernet receive msg[0]
#define FOXSI_POWER_SWITCH              0x03    // for U8/MAX7317 on power board
#define FOXSI_POWER_SWITCH_READ_MASK    0x80    // check if first byte if read command
    
#define FOXSI_POWER_HEALTH              0x04    // for U6/AD7490 on power board
#define FOXSI_POWER_HEALTH_WRITE_MASK   0x80    // check if first byte is write command
#define FOXSI_POWER_HEALTH_READ_ALL     0x20    // check if second byte is read all command
#define FOXSI_POWER_HEALTH_FORWARD      0xf0    // check if second byte is forward command
#define FOXSI_POWER_HEALTH_SETUP        0xff    // check if second byte is setup command
#define FOXSI_POWER_HEALTH_CONTROL_MASK 0x8310  // mask to OR with shifted address to get control command
#define FOXSI_POWER_HEALTH_MIN_DELAY_US 1       // minimum required CS raise between packets.
    
#define FOXSI_RTD1                      0x01    // for U19/LTC2983 on housekeeping board
#define FOXSI_RTD2                      0x02    // for U18/LTC2983 on housekeeping board
#define FOXSI_RTD_CONF_SENSE_ADDR       0x204   // memory address for 4B config data for sense resistor
#define FOXSI_RTD_CONF_RTD_ADDR         0x20c   // memory address for 4B config data for first RTD (others are spaced)
#define FOXSI_RTD_COUNT                 0x09    // number of RTDs per LTC2983 chip
#define FOXSI_RTD_WR_CMD                0x02    // write command byte
#define FOXSI_RTD_RD_CMD                0x03    // read command byte
#define FOXSI_RTD_RD_ADDR               0x001c  // first address of 9 (spaced by 8 B) channels to read (once data ready).
#define FOXSI_RTD_CONF_SENSE            0xe8025800  // value to write to FOXSI_RTD_CONF_SENSE_ADDR to config RSENSE resistor.
#define FOXSI_RTD_CONF_RTD              0x60961000  // value to write to FOXSI_RTD_CONF_RTD_ADDR (and in 8 B offset from there) to config
#define FOXSI_RTD_SEQ_CONV_ADDR         0x00f5      // address to write FOXSI_RTD_SEQ_CONV to start sequential conversion
#define FOXSI_RTD_SEQ_CONV              0x000aaaa8  // value to write to FOXSI_RTD_SEQ_CONV_ADDR to start sequential conversion
#define FOXSI_RTD_SETUP                 0xff    // setup (set config data for all channels)
#define FOXSI_RTD_START_ALL_CONV        0xf0    // start conversion on all channels
#define FOXSI_RTD_READ_ALL              0xf2    // read all outputs (if data ready)
#define FOXSI_RTD_MIN_DELAY_US          10
    
#define FOXSI_INTRO                     0x07
#define FOXSI_INTRO_SET_FLIGHT_STATE    0xf0    // expect 1 B argument specifying state.
#define FOXSI_INTRO_SET_UNLAUNCH        0xf1    // no more arguments. Just set FOXSI_CURRENT_STATE.
#define FOXSI_INTRO_SET_ERRORS          0xe0    // expect 2 B of error mask after.
#define FOXSI_INTRO_GET_FLIGHT_STATE    0x0f    // reply 1 B
#define FOXSI_INTRO_GET_ERRORS          0x0e    // return 1 B errors
#define FOXSI_INTRO_GET_SYNC_COUNTER    0x0c
#define FOXSI_INTRO_GET_CURRENT_CLOCK   0x0d
#define FOXSI_INTRO_GET_CORE_CLOCK      0x0b

    enum FOXSI_FLIGHT_STATE{
        FOXSI_FLIGHT_STATE_AWAIT,
        FOXSI_FLIGHT_STATE_UNLAUNCH,
        FOXSI_FLIGHT_STATE_PRELAUNCH,
        FOXSI_FLIGHT_STATE_SHUTTER,
        FOXSI_FLIGHT_STATE_END
    };
    
    uint8_t                     FOXSI_CURRENT_STATE;
    
    enum FOXSI_ERROR_MASK{
        FOXSI_ERROR_INIT,
        FOXSI_ERROR_ETH_RECV_MSB,
        FOXSI_ERROR_POWER_SWITCH,
        FOXSI_ERROR_POWER_HEALTH,
        FOXSI_ERROR_RTD1,
        FOXSI_ERROR_RTD2,
        FOXSI_ERROR_RTD,
        FOXSI_ERROR_INTRO
    };
    
    // log timing information for PPS
    uint32_t                    FOXSI_TIME_LONG;
    #define                     FOXSI_SYNC_LOG_SIZE 8
    uint32_t                    FOXSI_SYNC_LOG[FOXSI_SYNC_LOG_SIZE];
    
    // for tracking different error types in software. Move this to a separate header, define macros for each bit.
    uint16_t                    FOXSI_ERRORS;
    
    // like demo_tcp_server()---entry point for received Ethernet packets. 
    // Inside, delegate to handler for specific system.
    void ethernet_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size);
    
    
    
    // for reading/writing power on/off chip (on power board):
    void power_switch_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size); 
    
    
    
    // for reading ADC chip (on power board):
    void power_health_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size);
    void power_health_spi_setup();
    uint16_t power_health_convert_addr(uint8_t recv_addr);
    
    
    
    // for reading RTD chip:
    void rtd1_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size);
    void rtd2_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size);
    
    void rtd_setup(uint8_t rtd_num);
    void rtd_start_all_conversion(uint8_t rtd_num);
    void rtd_read_all(tcpTCB_t* port, uint8_t rtd_num);
    
    
    
    // for reflective (on PIC) health data:
    void introspect_handler(tcpTCB_t* port, uint8_t* recv_buff, size_t recv_size);
    
    // work to do each loop:
    void main_loop_handler();
    
    // initialize these systems:
    void housekeeping_init();
    
    // to read TMR1 and store value as uint32_t (rather than uint16_t)
    void lengthen_time();
   
    
    
    // utilities
    void bytes_from_uint16_t(uint16_t source, uint8_t* store);
    void bytes_from_uint32_t(uint32_t source, uint8_t* store);
    void swap_byte_order(uint8_t* data, size_t size);
    


#ifdef	__cplusplus
}
#endif

#endif	/* FORMATTER_INTERFACE_H */

