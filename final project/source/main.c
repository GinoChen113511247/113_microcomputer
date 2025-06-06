#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "user_defined.h"
#include <stdio.h>
#include "touch_module.h"

void stm32f4_Hardware_Init(void);
void Wait_PressPA0(uint16_t Cnum);
void Driver_GPIO(void);
void Default_Calibration(void);
void WhackAMole_Run(void);
	
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "user_defined.h"
#include <stdio.h>

#define KEY_TIME 10

int main(void)
{
    stm32f4_Hardware_Init();

    /* Initialize the LCD */
    LCD_Init();
    LCD_DisplayOn();

    /* Perform default touchscreen calibration and initialize GPIO driver */
    Default_Calibration();
    Driver_GPIO();
    
	startup:
	  /* ---------- Scenario 1 : Initial Display ---------- */
	  LCD_Clear(WHITE);
    LCD_SetFont(&Font16);
    LCD_SetColors(RED, BLUE);
    LCD_DisplayStringAt(10, 140, "Press KEY2 to play", CENTER_MODE);

	  /* Wait for KEY1 press to be held long enough */
    Wait_PressPA0(KEY_TIME);
    /* Wait for KEY1 release */
    while (GPIOA->IDR & Bit(0));
	
    /* ---------- Scenario 2 : Whack-A-Mole ---------- */
    WhackAMole_Run();
		
		/* ---------- Scenario 3 : Leaderboard ---------- */
    LCD_Clear(LCD_COLOR_GREEN);
		LCD_SetFont(&Font16);
		LCD_DisplayStringAt(10, 10, "Leader Board:", CENTER_MODE);
		LCD_DisplayStringAt(10, 200, "Press KEY2 to continue", CENTER_MODE);

    Wait_PressPA0(KEY_TIME);
    while (GPIOA->IDR & Bit(0));

    goto startup;
}


/* Busy-wait until KEY1 remains high for Cnum × 10 ms */
void Wait_PressPA0(uint16_t Cnum)
{
    uint16_t count = Cnum;
    while (1)
    {
        if (GPIOA->IDR & Bit(0)) {
            if (--count == 0) return;
        } else {
            count = Cnum;
        }
        delay_ms(10);
    }
}
