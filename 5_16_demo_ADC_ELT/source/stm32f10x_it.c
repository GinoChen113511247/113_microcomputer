/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include <stdio.h>
#define Bit(x) (0x01ul)<<x	   
#define __IO volatile

/* Private variables ---------------------------------------------------------*/
extern __IO	uint16_t ADC_InjectedValueTab[32];
__IO uint32_t Index = 0;

uint32_t cnt_adc=0, cnt_exti11=0, cnt_exti15=0;
/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/
uint32_t j=0;
/**
  * @brief  This function handles ADC1 ADC2 and ADC3 global interrupts requests.
  * @param  None
  * @retval None
  */
//__IO uint8_t flagEOC = 0;
void ADC_IRQHandler(void)
{
	if (ADC1->SR & Bit(2) )	// bit 2: JEOC
	{
		uint16_t val[3];
	  char p_text[20] = "";
  /* Clear ADC1 JEOC pending interrupt bit */
		ADC1->SR = ~Bit(2) ;  // bit 2: JEOC	  (rc_w0): no action for writing 1
  /* Get converted values of two injected channels  */

				val[0] = ADC1->JDR1*3300/0xFFF;
				val[1] = ADC1->JDR2*3300/0xFFF;
		ADC_InjectedValueTab[Index++] = val[0]; //ADC1->JDR1;
		ADC_InjectedValueTab[Index++] = val[1]; //ADC1->JDR2;
//				sprintf(p_text, "%2i. %3X %3X", Index/2, val[0], val[1]);
				sprintf(p_text, "%2i. %04d %04d", Index/2, val[0], val[1]);

				LCD_DisplayStringLineCol(6+j,  0, p_text);
				if ((++j) >=4) j = 0;
  
		cnt_adc++;
		if (Index == 64)
		{
			Index = 0;
		}
	}
}

