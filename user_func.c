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
            
            if(TCP_SendDone(&port7TCB))
            {
                // check to see if there are any received data
                rxLen = TCP_GetRxLength(&port7TCB);

                if(rxLen > 0)
                {
                    //make sure it safe to use the receive buffer
                    rxLen = TCP_GetReceivedData(&port7TCB);
//                    If input is 0x31 then enter
                    if(rxdataPort7[0] == 0x31){
//                        Start conversion on channel 8 by passing array conv4------------------------------------------------------
//                        to temp chip
                        uint8_t conv4[] = {0x02, 0x00, 0x00, 0x88};
                        uint8_t* spi_buff;
                        LATEbits.LATE1 = 0;
                        spi_buff = conv4;
                        SPI1_WriteBlock(spi_buff, 4);
                        LATEbits.LATE1 = 1; 
                        
//                        Wait while conversion is occuring------------------------------------------------------
//                        While loop to check if channel is done with conversion
//                        Probabaly easier to due with INTURUPT pin but wasnt working
                         uint8_t status[] = {0x03,0x00,0x00};
                         uint8_t ret = 0;
                         while(ret^0x48){
                            LATEbits.LATE1 = 0;
                            spi_buff = status;
//                          Start read from temp chip
                            SPI1_WriteBlock(spi_buff, 3);
                            ret = SPI1_ExchangeByte(0xFF);
                            LATEbits.LATE1 = 1;
                         }
                         __delay_ms(20);
                         
//                       Supposed to be INTERRUPT pin, LOW when busy, HIGH when free
                         
//                        Read results of conversion on channel 8 by passing array------------------------------------------------------
//                        read4 to temp chip, then pass dummy bytes to read results
//                        uint8_t read4[] = {0x03,0x00,0x00};
                        uint8_t read4[] = {0x03,0x00,0x2C};
                        LATEbits.LATE1 = 0;
                        spi_buff = read4;
                        SPI1_WriteBlock(spi_buff, 3);
                        txLen = 4;
                        for(int i=0; i<txLen; i++){
                            txdataPort7[i] = SPI1_ExchangeByte(0xFF);
                        }
                        LATEbits.LATE1 = 1;
                        
//                        Send data back through ethernet
                        TCP_Send(&port7TCB, txdataPort7, txLen);
                      
                    }
                    TCP_InsertRxBuffer(&port7TCB,rxdataPort7, sizeof(rxdataPort7));
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
