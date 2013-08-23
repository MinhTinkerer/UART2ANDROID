/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f1_system.h"
#include "module_rs232.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
void RS232_Config( void )
{
	GPIO_InitTypeDef GPIO_Structure;
	USART_InitTypeDef USART_Structure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);

	/* USART1 Tx PA9 */
	GPIO_Structure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_Structure);
	/* USART1 Rx PA10 */
	GPIO_Structure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_Structure);

	USART_Structure.USART_BaudRate = 9600;
	USART_Structure.USART_WordLength = USART_WordLength_8b;
	USART_Structure.USART_StopBits = USART_StopBits_1;
	USART_Structure.USART_Parity = USART_Parity_No ;
	USART_Structure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Structure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_Structure);
	USART_Cmd(USART1, ENABLE);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**��� : RS232_Print
**�\�� : 
**��J : 
**��X : 
**�ϥ� : 
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_Print( USART_TypeDef* USARTx, u8 *pWord )
{
  while(*pWord != '\0') {
    USART_SendData(USARTx, *pWord);
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    pWord++;
  }
}
/*=====================================================================================================*/
/*=====================================================================================================*
**��� : RS232_VisualScope_CRC16
**�\�� : 
**��J : 
**��X : 
**�ϥ� : 
**=====================================================================================================*/
/*=====================================================================================================*/
static u16 RS232_VisualScope_CRC16( u8 *Array, u16 Len )
{
	u16 USART_IX, USART_IY, USART_CRC;

	USART_CRC = 0xffff;
	for(USART_IX=0; USART_IX<Len; USART_IX++) {
		USART_CRC = USART_CRC^(u16)(Array[USART_IX]);
		for(USART_IY=0; USART_IY<=7; USART_IY++) {
			if((USART_CRC&1)!=0)
				USART_CRC = (USART_CRC>>1)^0xA001;
			else
				USART_CRC = USART_CRC>>1;
		}
	}
	return(USART_CRC);
}
/*=====================================================================================================*/
/*=====================================================================================================*
**��� : USART_VisualScope
**�\�� : 
**��J : 
**��X : 
**�ϥ� : 
**=====================================================================================================*/
/*=====================================================================================================*/
void RS232_VisualScope( USART_TypeDef* USARTx, u8 *pWord, u16 Len )
{
	u8 i = 0;
	u16 Temp = 0;

	Temp = RS232_VisualScope_CRC16(pWord, Len);
	pWord[8] = Temp&0x00ff;
	pWord[9] = (Temp&0xff00)>>8;

	for(i=0; i<10; i++) {
		USART_SendData(USARTx, *pWord);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		pWord++;
	}
}
/*=====================================================================================================*/
/*=====================================================================================================*/
