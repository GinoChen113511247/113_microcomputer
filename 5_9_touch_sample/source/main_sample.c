/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "user_defined.h"
#include <stdio.h>	// for sprintf
#include "touch_module.h"

void stm32f4_Hardware_Init (void);
void Wait_PressPA0(uint16_t Cnum);

void Driver_GPIO(void);
void Driver_SPIpin_GPIO(void);

void Default_Calibration(void);
void Touchscreen_demo (void);
void Touchscreen_playFig(void);

void Touchscreen_Calibration (void);

void Touch_sample_FreeDraw(void);
void Touch_sample_Sine(void);
void Sample_alarmA(void);
void Touch_sample_Hit(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
#define KEY_TIME 10  
 uint16_t keyTimer = KEY_TIME;  

	stm32f4_Hardware_Init();

	/* Initialize the LCD */
	LCD_Init();
	LCD_Clear(GREEN);
	LCD_DisplayOn();

	/* Initialize the Touch module */
	Default_Calibration();
	Driver_GPIO();

	
//	Touchscreen_Calibration();

startup:
	while(GPIOA->IDR & 0x01)			// wait until release KEY1
    delay_ms(20);						// wait 20 msec for debouncing

	LCD_SetFont(&Font20);
	LCD_SetColors(RED, BLUE);
	LCD_DisplayStringAt(36, 140, (char*)" Press KEY1   ", LEFT_MODE);
	LCD_DisplayStringAt(36, 160, (char*)" then Release ", LEFT_MODE);

//=================
	Wait_PressPA0(KEY_TIME);	
	while(GPIOA->IDR & Bit(0));			// wait until release KEY1
//====================================
	
//>>>>===== Scinario 1 =======================		
	LCD_Clear(WHITE);	      
	//Touch_sample_FreeDraw();
	
//>>>>===== Scinario 2 =======================		
  Touch_sample_Sine();
	
//>>>>===== Scinario 3 =======================
	LCD_Clear(GREEN);
	//Touch_sample_Hit();
	
//>>>>===== Scinario X =======================		
  LCD_Clear(GREEN);
	Sample_alarmA();
	
	LCD_Clear(LCD_COLOR_GREEN);
	goto startup;
}
//===============================
void Wait_PressPA0(uint16_t Cnum)
{
	uint16_t count = Cnum;
	while(1)
	{	
    if(GPIOA->IDR & Bit(0))						// normally low
    {
			if (--(count)==0){
				return;
			}
		} else count = Cnum;

    delay_ms(10);
	}
}





