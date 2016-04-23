
#include "Def.h"
#include "stdio.h"
#include "UartCtrl.h"
#include "def.h"
#include "uart.h"
#include "motor.h"

#include "PID.h"

#include "Report.h"
#include "ConfigTable.h"
#include "Control.h"

uint8_t getUartData = FALSE;

uint8_t g_u8RecData[RXBUFSIZE]  = {0};

//volatile uint32_t g_u32comRbytes = 0;
//volatile uint32_t g_u32comRhead  = 0;
//volatile uint32_t g_u32comRtail  = 0;
volatile uint8_t g_UartDataTotalLength = 0;
uint8_t g_UartDataIndex = 0;

void setupUART()
{
	/* Enable peripheral clock */
	CLK_EnableModuleClock(UART0_MODULE);
	/* Peripheral clock source */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_HIRC, CLK_CLKDIV_UART(1));
	/* Set PD multi-function pins for UART0 RXD, TXD */
	SYS->P5_MFP &= ~(SYS_MFP_P51_Msk | SYS_MFP_P50_Msk);
  SYS->P5_MFP |= (SYS_MFP_P51_UART0_RXD | SYS_MFP_P50_UART0_TXD);
	
	//SYS->P0_MFP = SYS_MFP_P00_TXD | SYS_MFP_P01_RXD;
	
	/* Reset UART module */
	SYS_ResetModule(UART0_RST);

	/* Configure UART0 and set UART0 Baudrate */
	UART_Open(UART0, 115200);
}

uint8_t GetUartChar(void)
{
	uint8_t c;
	if(g_UartDataIndex < g_UartDataTotalLength)
	{
		c = g_u8RecData[g_UartDataIndex];
		g_UartDataIndex ++;
	}
	return c;
}


/*---------------------------------------------------------------------------------------------------------*/
/* UART Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void UART_HANDLE()
{
    uint8_t u8InChar=0xFF;
    uint32_t u32IntSts= UART0->INTSTS;

    if(u32IntSts & UART_INTSTS_RDAINT_Msk) {
				
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART)) {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART);           /* Rx trigger level is 1 byte*/

						//printf("%c", u8InChar);
            /* Check if buffer full */
            if(g_UartDataTotalLength < RXBUFSIZE) {
                /* Enqueue the character */
                g_u8RecData[g_UartDataTotalLength] = u8InChar;
								g_UartDataTotalLength++;
                //g_u32comRtail = (g_u32comRtail == (RXBUFSIZE-1)) ? 0 : (g_u32comRtail+1);
                //g_u32comRbytes++;
            }
						if(u8InChar == '\n' || u8InChar == '\0' || u8InChar == '$')
						{
							
								getUartData = TRUE;
								g_UartDataIndex = 0;
								//g_UartDataTotalLength = 0;
						}
        }
    }
		
//		if(u32IntSts & UART_ISR_THRE_INT_Msk) {
//			printf("\nInput:");
////        uint16_t tmp;
////        tmp = g_u32comRtail;
////        if(g_u32comRhead != tmp) {
////            u8InChar = g_u8RecData[g_u32comRhead];
////            UART_WRITE(UART,u8InChar);
////            g_u32comRhead = (g_u32comRhead == (RXBUFSIZE-1)) ? 0 : (g_u32comRhead+1);
////            g_u32comRbytes--;
////        }
//    }
		
		
}


/*---------------------------------------------------------------------------------------------------------*/
/*  UART Function Test                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void UART_NVIC_INIT()
{
    
    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART, UART_INTEN_RDAIEN_Msk);
	//UART_ENABLE_INT(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    
    NVIC_EnableIRQ(UART0_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
    UART_HANDLE();
}

void CommandProcess(void)
{
	int command;
	char mode;
	// Read incoming control messages
	//if (Serial_available() >= 2)
	if(getUartData == TRUE && (g_UartDataTotalLength >= 2) )
	{
		char start = GetUartChar();//UART_READ(UART);//Serial_read();
		if (start == '@')
		{// Start of new control message
			printf("@HOOK \n");
			//int command = UART_READ(UART);//Serial_read(); // Commands
			command = GetUartChar();
			if (command == 'm')
			{
					//printf("@s ok \n");
					// 's'tream output control
					mode = GetUartChar();
					if (mode == 's') 
					{// 's'tart stream
						//stream_mode = STREAM_START;
						Motor_Start();
					}
					else if (mode == 'p') 
					{// 'p'ause stream
						//stream_mode = STREAM_PAUSE;
						Motor_Stop();
					}
			}
			else if(command == 'v')	//Check 'v'ersion
			{
				CheckVersion();
			}
			else if (command == 'p') {// Set 'p'id command
				SetPID();
			}
			else if (command == 't') {// Set 'Thro'id command
				SetThro();
			}
			else if(command == 'r')	 //Set 'r'eport mode
			{
				mode = GetUartChar();
				if (mode == 'p') {// Report controller 'p'id
					char type = GetUartChar();
					if (type == 'p') // 'p'id
						report_mode = REPORT_ANGLE_PID;
					else if (type == 'r') //'r'ate pid
						report_mode = REPORT_RATE_PID;
				}
				if (mode == 'm') //'m'otor power
						report_mode = REPORT_MOTOR_POWER;
				if (mode == 'e') //"e"uler
						report_mode = REPORT_AHRS_EULER;
				if (mode == 'r') //"r"aw sensor data
						report_mode = REPORT_SENSORS_RAW;
			}
			else if (command == 's') {// Set 'p'id command
				SaveParamsToFlash();
			}
//		else 
//		{ 
//			if (report_format == REPORT_FORMAT_TEXT) {
//			printf("Unknown command.\n");
//		}
		} // Skip character
		
		getUartData = FALSE;	//标记接收完成
		g_UartDataTotalLength = 0;
	}
}
