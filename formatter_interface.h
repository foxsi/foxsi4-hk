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


    
    // like demo_tcp_server()---entry point for received Ethernet packets. 
    // Inside, delegate to handler for specific system.
    void ethernet_handler();
    
    // should identify system and delegate to command handler. 
    void system_handler();
    
    // for reading/writing power on/off chip (on power board):
    void power_switch_handler(); 
    // for reading ADC chip (on power board):
    void power_health_handler();
    // for reading RTD chip:
    void rtd_handler();
    // for reflective (on PIC) health data:
    void introspect_handler();
    
    // can return by calling TCP_Send()
    
    


#ifdef	__cplusplus
}
#endif

#endif	/* FORMATTER_INTERFACE_H */

