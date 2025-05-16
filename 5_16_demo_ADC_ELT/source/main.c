
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "stm32f4xx_dma.h"
#include "user_defined.h"
#include <stdio.h>


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t ADC_RegularValueTab[64], ADC_InjectedValueTab[32];

#define NumADC 64

/* Private functions ---------------------------------------------------------*/
void DMA_Config(void);

void Display_ADC_elt_Init(void);

void stm32f4_adc(void);
//#define USE_ADC3
void stm32f4_Hardware_Init (void);
void delay_ms(uint32_t wait_ms);
void stm32_nvic(void);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t i, j, leftDMA;

	stm32f4_Hardware_Init ();
	stm32_nvic();

  /* LCD Display init  */
  Display_ADC_elt_Init();

 	stm32f4_adc();	
  DMA_Config();


	i=0;
	j = 0;
	leftDMA = NumADC;
  while (1)
  {
		uint16_t val[3], kk;
	  char p_text[20] = "";
//		uint32_t currentDMA;
		
		while( i < 64){
			
//			do{
//			 if ((ADC1->SR)& Bit(5)){			// if OVR = 1
//				 ADC1->SR = ~Bit(5);
//			}
//		  }while( ((ADC1->SR)& Bit(1)) ==0); // wait until EOC = 1
			 
//			if (ADC1->SR & (Bit(5)| Bit(1)) ){			// if EOC = 1 or OVR=1

/*			if (ADC1->SR & (Bit(1)) ){			// if EOC = 1; almost no EOC when DMA is used
				//ADC1->SR = ~Bit(1);	// clear EOC
*/			
//			if (ADC1->SR & (Bit(4)) ){			// if STRT = 1
//				ADC1->SR = ~Bit(4);	// clear STRT bit
				
				while ((DMA2_Stream0->NDTR) == leftDMA);
				delay_ms(1);
				leftDMA = DMA2_Stream0->NDTR;

				if (j >= 3){
					i = i-9;		// the first 3 lines roll up,
					j=0;				// and then print the new data in the 4th line
					kk=4;
				}else{
					kk=1;			// print a single line for j= 0, 1, 2
				}
				
					for(; kk>0; kk--){
						val[0] = ADC_RegularValueTab[i++]*3300/0xFFF;
						val[1] = ADC_RegularValueTab[i++]*3300/0xFFF;
						val[2] = ADC_RegularValueTab[i++]*3300/0xFFF;
//						sprintf(p_text, "%2i. %3X %3X %3X", i/3, val[0], val[1], val[2]);
						sprintf(p_text, "%2i. %04d %04d %04d", i/3, val[0], val[1], val[2]);
							LCD_DisplayStringLineCol(1+j,  0, p_text);
//							LCD_DisplayString(LCD_LINE(1+j), 0, p_text);
/*						if (j<3){
							LCD_DisplayString(LCD_LINE(2+j), 0, "                    ");
						}
*/
						j++;
					}
//			}
		}
	}
}


/**
  * @brief  ADC3 channel03 with DMA configuration
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  DMA_InitTypeDef       DMA_InitStructure;

  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
	RCC->AHB1ENR |=RCC_AHB1ENR_DMA2EN;
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  	//ADC1
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) ADC_RegularValueTab;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = NumADC;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);
}

/////////////////////////////////////////////
//
/////////////////////////////////////////////
void Display_ADC_elt_Init(void)
{
 	//TFT intialization
	LCD_Init();

	LCD_DisplayOn();
	LCD_Clear(WHITE);


  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(LCD_COLOR_BLUE2);
  LCD_SetTextColor(LCD_COLOR_MAGENTA);

	LCD_DisplayStringLineCol(0,  1,"Regular Values:");
//  LCD_DisplayString(LCD_LINE(0), 1,"Regular Values:");
  LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_DisplayStringLineCol(5,  1,"Injected Values:");
//  LCD_DisplayString(LCD_LINE(5), 1,"Injected Values:");

  LCD_SetTextColor(LCD_COLOR_WHITE);
}

/**
  * @}
  */ 
/**
  * @brief  Display ADC converted value on LCD
  * @param  None
  * @retval None
  */
void Display(uint32_t *p_volt)
{
  uint32_t v=0,mv=0, mmv, i;
  char text[50];

	for (i=0; i<NumADC; i++){
		v=(p_volt[i])/1000;
		mmv = (p_volt[i]%1000);
		mv = mmv/100;
		mmv = (mmv%100)/10;
		sprintf((char*)text,"%d.%d%d V   ",v,mv, mmv);
		LCD_DisplayStringLineCol(6+i, 10, text);
//		LCD_DisplayString(LCD_LINE(6+i), 10, text);
	}
}

/**
  * @brief  Display Init (LCD)
  * @param  None
  * @retval None
  */
/*
void Display_Init(void)
{
 	//TFT intialization
	LCD_Init();

	LCD_DisplayOn();
	LCD_Clear(WHITE);

	LCD_SaveFont();
	LCD_SetFont(&Font20);

  // Set the LCD Back Color and Text Color
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);

  // Display 
  LCD_DisplayStringAtLine(0, "ADC Ch3 Conv @2.4Msps");  // Msps (Mega samples per second)


  // Set the LCD Back Color and Text Color
  LCD_SetBackColor(LCD_COLOR_WHITE);
  LCD_SetTextColor(LCD_COLOR_BLUE);

  // Display 
  LCD_DisplayStringAtLine(2,"  Turn RT1(PA.03)    ");
  LCD_DisplayStringAtLine(4,"   Potentiometer     ");
	
	LCD_DisplayStringLineCol(6, 2, "PA3  =");
	LCD_DisplayStringLineCol(7, 2, "Temp =");
	LCD_DisplayStringLineCol(8, 2, "Vint =");
	LCD_DisplayStringLineCol(9, 2, "Vbat =");

}

*/

