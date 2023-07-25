/* 
 * File:   parameters.h
 * Author: thanasi
 *
 * Created on July 24, 2023, 12:29 PM
 */

#ifndef PARAMETERS_H
#define	PARAMETERS_H

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * Formatter sends power-related commands with first byte 0x03. Should be
 * sent to MAX7317 chip on nCS line 3 / RE2.
 */
#define FMTR_POWER_CS               0x03
/*
 * Formatter sends ADC / Isense / Vsense commands with first byte 0x04. 
 * Should be sent to AD7490 chip on nCS line 4 / RE3.
 */
#define FMTR_SENSE_CS               0x04
    
/*
 * Power-related macros (for turning on, off, and device-specific registers
 * to address).
 */
#define FMTR_POWER_WR_ON            0x00
#define FMTR_POWER_WR_OFF           0x01

#define FMTR_POWER_BROADCAST_CDTE   0x10
#define FMTR_POWER_BROADCAST_CMOS   0x11
#define FMTR_POWER_PORT_CDTEDE      0x00
#define FMTR_POWER_PORT_CDTE1       0x03
#define FMTR_POWER_PORT_CDTE2       0x04
#define FMTR_POWER_PORT_CDTE3       0x05
#define FMTR_POWER_PORT_CDTE4       0x06
#define FMTR_POWER_PORT_CMOS1       0x07
#define FMTR_POWER_PORT_CMOS2       0x08
#define FMTR_POWER_PORT_TIMEPIX     0x01
#define FMTR_POWER_PORT_SAAS        0x02
#define FMTR_POWER_PORT_FMTR        0x09

#ifdef	__cplusplus
}
#endif

#endif	/* PARAMETERS_H */

