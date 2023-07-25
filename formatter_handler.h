/* 
 * File:   power_handler.h
 * Author: thanasi
 *
 * Created on July 24, 2023, 12:46 PM
 */

#ifndef POWER_HANDLER_H
#define	POWER_HANDLER_H

#ifdef	__cplusplus
extern "C" {
#endif
    

#include <stdint.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/TCPIPLibrary/tcpv4.h"
    


int ether_resolver(tcpTCB_t *port, uint8_t ether_msg[], unsigned int len);
int power_handler(tcpTCB_t *port, uint8_t ether_msg[], unsigned int len);


#ifdef	__cplusplus
}
#endif

#endif	/* POWER_HANDLER_H */

