/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F66J60
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/spi1.h"
#include "user_func.h"

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
//    do { TRISAbits.TRISA3 = 0; } while(0);
//    LATAbits.LATA3 = 1;
    // Open SPI
    
//    TRISEbits.TRISE1 = 0;
    // raise LTC2983 #2 chip select pin high to disable:
//    LATEbits.LATE1 = 1;
    
    SPI1_Open(SPI1_DEFAULT);
    
    
    // Pointer used to write data
    uint8_t* spi_buff;
//    
////    Used to initilize LTC, not sure why this is needed
    LATEbits.LATE1 = 0;
    SPI1_WriteByte(0x14);
    LATEbits.LATE1 = 1;     
    __delay_ms(3);
    
    
    // Possibly check if chip is ready
    uint8_t spi_buff_arr[] = {0x03, 0x00, 0x00, 0xff};
    spi_buff = spi_buff_arr;
    
    // Configure Sense Resistor 150 Ohms------------------------------------------------------
    LATEbits.LATE1 = 0;
    uint8_t rsense_buff[] = {0x02, 0x02, 0x04, 0xE8, 0x02, 0x58, 0x00};
    spi_buff = rsense_buff;
    SPI1_WriteBlock(spi_buff, 7);
    LATEbits.LATE1 = 1;
    
    // Configure RTD 1 at ch 4----------------------------------------------------------------
    LATEbits.LATE1 = 0;
    uint8_t rtd1_buff[] = {0x02, 0x02, 0x1C, 0x60, 0x96, 0x10, 0x00};
    spi_buff = rtd1_buff;
    SPI1_WriteBlock(spi_buff, 7);
    LATEbits.LATE1 = 1;
    
//    Block to start conversion on ch 4-------------------------------------------------------
    uint8_t conv4[] = {0x02, 0x00, 0x00, 0x88};
    
    OSCTUNE = 0x00;
    OSCCON |=2;
    
    while (1)
    {
//        Commented out line in Network_Manage() to get function to run
        Network_Manage();
//        Cannot find DEMO_TCP_EchoServer()?
        DEMO_TCP_echo_server();

        
////        Start Conversion on Ch 4------------------------------------------------------------
//        LATEbits.LATE1 = 0;
//        spi_buff = conv4;
//        SPI1_WriteBlock(spi_buff, 4);
//        LATEbits.LATE1 = 1;
//        
////        For now using 180 ms delay to wait till--------------------------------------------- 
////        INTERRUPT bit goes high and LTC is done with conversion
//        __delay_ms(180);
//        
////         Supposed to be INTERRUPT pin, LOW when busy, HIGH when free
////         for some reason not working
////        while(!LATDbits.LATD2);
//        LATEbits.LATE1 = 0;
//     
//        // Read Result at Ch 4----------------------------------------------------------------
//        SPI1_WriteByte(0x03);
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0x00);
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0x2C);
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
////         Read out status register if wanted
////        SPI1_WriteByte(0x00);
////        while(!PIR1bits.SSP1IF);
////        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0xFF);
//        SPI1_ReadByte();
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0xFF);
//        SPI1_ReadByte();
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0xFF);
//        SPI1_ReadByte();
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        SPI1_WriteByte(0xFF);
//        SPI1_ReadByte();
//        while(!PIR1bits.SSP1IF);
//        PIR1bits.SSP1IF = 0;
//        
//        LATEbits.LATE1 = 1;
//        __delay_ms(30);
       
    }
    SPI1_Close();
}
/**
 End of File
*/