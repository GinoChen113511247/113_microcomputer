/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "user_defined.h"
#include "stlogo.h"
#include <stdio.h>

void stm32f4_Hardware_Init (void);
static void delay(__IO uint32_t nCount);
void GLCD_Value(uint8_t row_no, uint8_t col_no, int16_t val);
extern uint8_t startAddress;
char* get_JPG_error_code(void);
void DEMO_shifting(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize, uint8_t infty);

#include <stdlib.h>
#define SZ_imagepool_x 	0x1900	//160*10*2 = 3,200 = 0x0C80
// 2 * 0x0C80 = 0x1900
#ifndef Bit
#define Bit(x) 	(0x01ul<<x)
#endif
//====================
//  GPIO
//--------------------
#define GPIOx_OUTPUT_setPINS(GPIOx, _bitmap) (GPIOx->BSRR  = (uint32_t) _bitmap)
#define GPIOx_OUTPUT_resetPINS(GPIOx, _bitmap) (GPIOx->BSRR  = (uint32_t) _bitmap<<16)
#define GPIOx_OUTPUT_togglePINS(GPIOx, _bitmap) (GPIOx->ODR  ^= (uint32_t) _bitmap)


void Wait_PressPA0(uint16_t Cnum);
void Driver_GPIO(void);

//>>>>>--------------------------------------------
#include "user_defined.h"
#define Flash_Sector4 0x08010000	// 64 KBytes
#define Flash_Sector5 0x08020000	// 128 KBytes
#define Flash_Sector6 0x08040000	// 128 KBytes
#define Flash_Sector7 0x08060000	// 128 KBytes
#define p_bmp         Flash_Sector6	// 0x08040000
#define p_StAdd      Flash_Sector4     // 0x08010000

uint32_t BMP_check_show(uint16_t x_pixel, uint16_t y_pixel, uint8_t *p_BmpAdd);
void Flash_Erase_for_Write(uint32_t beginSector, uint32_t endSector);	// in user_defined.h
void	ChangeBMP_24bit_to_16bit(uint8_t* p, uint8_t* p_StoreAdd);

//<<<<<-----------------------------------
#include <stdlib.h>
#include <stdio.h>
#define Flash_Sector8 0x08080000	// 128 KBytes
void save_RGBimage_in_flash(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize, uint16_t* p_StoreAdd);
char* get_JPG_error_code(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t bitCount;
	uint8_t res;
	// Hardware intialization
	stm32f4_Hardware_Init ();
//	Driver_GPIO();
	//TFT intialization
	LCD_Init();
  	delay(2000); /* delay 2000 ms */

  	LCD_Clear(LCD_COLOR_GREEN);
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLUE); // Text = red; back = blue

	//>>>> JPEG =========================
#define JPG_OK 0
#define JPG_fileAddress	Flash_Sector8

	// scaling factor = 1/2
	res = LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 320, 240);  // start at xPixel=0, yPixel=0
	if (res != JPG_OK)
	{
		LCD_SetTextColor(YELLOW);
		LCD_SaveFont();
		LCD_SetFont(&Font16);
		LCD_DisplayStringAt(5, 100, get_JPG_error_code(),  LEFT_MODE);
		LCD_RestoreFont();
		while(1);
	}
	
		LCD_DisplayStringLineCol(3, 1, "Press KEY1 to erase");
		LCD_DisplayStringLineCol(5, 1, "some flash sectors!");

	//=================
	Wait_PressPA0(10);	
	while(GPIOA->IDR & Bit(0))			// wait until release KEY1
//====================================
		LCD_SetTextColor(YELLOW);
		LCD_DisplayStringLineCol(3, 1, "Waiting ......!    ");
		LCD_SetTextColor(RED);
		LCD_DisplayStringLineCol(5, 1, "Waiting ......!    ");

#define PSIZE_word	1  // 0: 1B, 1: 2B, 2: 4B
	
// /* Step 1: Erase the user Flash: Sectors 4 and 5*************/ 
	Flash_Erase_for_Write(4, 5);			// erase flash sectors 4 to 5
//>>----------------------
	LCD_DisplayStringLineCol(5, 1, "Press KEY1 to go on!");
	Wait_PressPA0(10);	
	while(GPIOA->IDR & Bit(0))			// wait until release KEY1
 //<<-----------------------

	res = LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 320, 240);  // start at xPixel=0, yPixel=0
  User_FLASH_beforeWRITE(PSIZE_word);		// prepare for flash programming
  FLASH_clear_allErrorStatus_SR;// clear FLASH->SR
	//------>>>>>>>>>>		
	save_RGBimage_in_flash(0, 0, 320, 240, (uint16_t*) p_StAdd);
	//------<<<<<<<<<<<
	FLASH_clear_REGISTER_CR_9bits; // clear bits 0~6, 8, 9 of FLASH->CR

	LCD_Clear(YELLOW);
	 delay(200); /* delay 200 ms */
	LCD_DrawRGBImage(0, 0, 320, 240, (uint8_t*) p_StAdd);	
	LCD_DisplayStringLineCol(5, 1, "This is a RGB image!");
	LCD_DisplayStringLineCol(6, 1, "stored in 0x0801000!");
	LCD_SetTextColor(YELLOW);
	LCD_DisplayStringLineCol(7, 1, "size = 0x025800!");

	while(1);
}

void GLCD_Value(uint8_t row_no, uint8_t col_no, int16_t val)
{
  char p_text[6] = "";

	sprintf(p_text, "%i ", val);  //% read a decimal, octal, or hexadecimal integer
	LCD_DisplayStringLineCol(row_no, col_no, p_text);
}



void save_RGBimage_in_flash(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize, uint16_t* p_StoreAdd)
{
#define Image_width  320	
#define Image_height 1
#define NumBypes_pixel 2
#define SZ_imagepool 	(Image_width * Image_height * NumBypes_pixel)

    uint8_t *pImage_0;	
    uint16_t i, j;
    pImage_0 = (uint8_t *)malloc(SZ_imagepool);
    if (pImage_0 == NULL){
        LCD_DisplayStringAt(5, 100, "ERROR: NOT enough HEAP", LEFT_MODE);
        LCD_DisplayStringAt(5, 116, "   Size of pImage_0 is", LEFT_MODE);
        GLCD_Value(7, 12, (int16_t)(SZ_imagepool));
        while(1);
    }
		
    for(i = 0; i < ySize; i++){
        uint32_t status;
        uint16_t nSh_cpy, *pBuffer;
			
        nSh_cpy = y0 + i;
        LCD_ReadRGBImage(x0, nSh_cpy, xSize, 1, pImage_0);	
        pBuffer = (uint16_t *)pImage_0;
        for (j = 0; j < xSize; j++){
            *p_StoreAdd++ = *pBuffer++;
            status = FLASH_check_BUSY_bit();
            if (status != 0){
                LCD_DisplayStringLineCol(3, 1, "Flash Progrg. Error!" );
                return;
            }
        }
    }
	
    free(pImage_0);
}

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (10000 * nCount); index != 0; index--)
  {
  }
}

//===============================
void Wait_PressPA0(uint16_t Cnum)
{
 uint16_t keyTimer = Cnum;

    while(1)
   {	
        if(GPIOA->IDR & Bit(0))	// normally low
       {
           if (--keyTimer==0){
              LCD_Clear(YELLOW);
              return;
           }
       } else keyTimer = Cnum;
       delay(10);
   }	// end of while{1}
}
