#include "user_func.h"
#include "mcc_generated_files/mcc.h"

void DEMO_TCP_echo_server(void)
{
    // create the socket for the TCP Server
    static tcpTCB_t port7TCB;
    // create the TX and RX buffers
    static uint8_t rxdataPort7[20];
    static uint8_t txdataPort7[20];
    uint16_t rxLen, txLen, i;
    socketState_t socket_state;
    rxLen = 0;
    // checking the status of the socket
    socket_state = TCP_SocketPoll(&port7TCB);
    switch(socket_state)
    {
        case NOT_A_SOCKET:
            //Inserting and initializing the socket
            TCP_SocketInit(&port7TCB);

        case SOCKET_CLOSED:
            //configure the local port
            // EDITED PORT NUMBER FROM EXAMPLE
            TCP_Bind(&port7TCB, 7);
            // add receive buffer
            TCP_InsertRxBuffer(&port7TCB,rxdataPort7, sizeof(rxdataPort7));
            // start the server
            TCP_Listen(&port7TCB);
            break;

        case SOCKET_CONNECTED:
            // check if the buffer was sent, if yes we can reuse the //buffer
//            uint8_t arr[] = {0x00,0xFF};
//            uint8_t* buff = arr;
//            TCP_Send(&port7TCB, 0x00, 1);
            
            if(TCP_SendDone(&port7TCB))
            {
                // check to see if there are any received data
                rxLen = TCP_GetRxLength(&port7TCB);

                if(rxLen > 0)
                {
                    //make sure it safe to use the receive buffer
                    rxLen = TCP_GetReceivedData(&port7TCB);

                    //simulate some buffer processing copy from //the RX buffer to the TX buffer
//                    for(i = 0; i < rxLen; i++)
//                    {
//                        txdataPort7[i] = rxdataPort7[i];
//                    }
                    // reuse the rx buffer
                    if(rxdataPort7[0] == 0x33){
                        uint8_t conv4[] = {0x02, 0x00, 0x00, 0x88};
                        uint8_t* spi_buff;
//                          Start Conversion on Ch 4------------------------------------------------------------
                        LATEbits.LATE1 = 0;
                        spi_buff = conv4;
                        SPI1_WriteBlock(spi_buff, 4);
                        LATEbits.LATE1 = 1;
                        __delay_ms(20);
//        For now using 180 ms delay to wait till--------------------------------------------- 
//        INTERRUPT bit goes high and LTC is done with conversion
                         __delay_ms(180);
        
//         Supposed to be INTERRUPT pin, LOW when busy, HIGH when free
//         for some reason not working
//        while(!LATDbits.LATD2);
                        LATEbits.LATE1 = 0;
     
        // Read Result at Ch 4----------------------------------------------------------------
                        SPI1_WriteByte(0x03);
                        SPI1_WriteByte(0x00);
                        SPI1_WriteByte(0x2C);
//                        SPI1_WriteByte(0x00);
                        SPI1_WriteByte(0);
                        txdataPort7[0] = SPI1_ReadByte();
//                        txdataPort7[0] = 0x13;
                        SPI1_WriteByte(0);
                        txdataPort7[1] = SPI1_ReadByte();
                        SPI1_WriteByte(0);
                        txdataPort7[2] = SPI1_ReadByte();
                        SPI1_WriteByte(0);
                        txdataPort7[3] = SPI1_ReadByte();
                        LATEbits.LATE1 = 1;
                        
                    }
            
                    TCP_InsertRxBuffer(&port7TCB,rxdataPort7, sizeof(rxdataPort7));

//                    txLen = rxLen;
                    //send data back to the source
                    TCP_Send(&port7TCB, txdataPort7, 4);
                }
            }
            break;
        case SOCKET_CLOSING:
            TCP_SocketRemove(&port7TCB);
            break;
        default:
            // we should not end up here
            break;
    }
}
