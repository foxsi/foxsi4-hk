/**
\file
\defgroup doc_driver_uart_code UART Driver Source Code Reference
\ingroup doc_driver_uart
\brief This file contains the UART configurations selected by the user in the UART Foundation Services MCC Interface.
\copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
\page License
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
**/

#ifndef _UART_H
#define _UART_H


#include <stdint.h>
#include <stddef.h>
#include "../eusart1.h"

/**
*   \ingroup doc_driver_uart_code
*   \enum uart_configurations_t uart.h
*/
typedef enum { 
    UART0 /**<UART Name */
} uart_configurations_t;

/**
*   \ingroup doc_driver_uart_code
*   \struct uart_functions_t uart.h
*/
typedef struct { uint8_t (*Read)(void); void (*Write)(uint8_t txdata); bool (*TransmitReady)(void); bool (*DataReady)(void); bool (*TransmitDone)(void); void (*Initialize)(void);  } uart_functions_t;

extern const uart_functions_t uart[];

#endif	// _UART_H