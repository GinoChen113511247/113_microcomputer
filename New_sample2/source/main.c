/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include  "stlogo.h"
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

#define TP_INT_IN   ( (GPIOB->IDR & Bit(1)) >> 1)		// PB1
#define GPIO_SCK		GPIOB
#define GPIO_CS		GPIOC
#define GPIO_MI 		GPIOB
#define GPIO_MO 		GPIOF
#define pin_SCK		0    // PB0
#define pin_CS		13  // PC13
#define pin_MI		2    // PB2
#define pin_MO		11   // PF11

void Wait_PressPA0(uint16_t Cnum);
void Driver_GPIO(void);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	// Hardware intialization
	stm32f4_Hardware_Init ();
	
	Driver_GPIO();
	//TFT intialization
	LCD_Init();
  delay(2000); /* delay 2000 ms */

  LCD_Clear(LCD_COLOR_GREEN);
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLUE); // Text = red; back = blue
  
	LCD_DisplayStringAt(36, 140, (char*)"Press KEY1    ", LEFT_MODE);
	LCD_DisplayStringAt(36, 160, (char*)"then Release  ", LEFT_MODE);
	
	Wait_PressPA0(10);
	while(GPIOA->IDR & Bit(0))
	;

	while(1)
	{
			    if( !(TP_INT_IN) )
    {
	LCD_DisplayStringLineCol(5, 5, "Touched    " );		
	GPIOx_OUTPUT_setPINS(GPIO_SCK, Bit(pin_SCK));
	GPIOx_OUTPUT_setPINS(GPIO_CS, Bit(pin_CS) );
	GPIOx_OUTPUT_setPINS(GPIO_MO, Bit(pin_MO) );
    }else{
	LCD_DisplayStringLineCol(5, 5, "           " );		
	GPIOx_OUTPUT_resetPINS(GPIO_SCK, Bit(pin_SCK));
	GPIOx_OUTPUT_resetPINS(GPIO_CS, Bit(pin_CS) );
	GPIOx_OUTPUT_resetPINS(GPIO_MO, Bit(pin_MO) );
    }
    if (GPIO_MI->IDR & Bit(pin_MI)){			// test MISO
	LCD_DisplayStringLineCol(6, 5, "MISO: In= 1" );		
    } else {
	LCD_DisplayStringLineCol(6, 5, "MISO: In= 0" );		
   }
   if (GPIO_MO->IDR & Bit(pin_MO)){			// test MOSI
	LCD_DisplayStringLineCol(8, 5, "MOSI: Out= 1" );		
   } else {
	LCD_DisplayStringLineCol(8, 5, "MOSI: Out= 0" );		
   }
	}
	
  MenuInit();
	LCD_DrawBitmap(190, 130, (uint8_t *) stlogo);
	
	LCD_DisplayStringLineCol(3, 1, (char*)" Press KEY1");
	LCD_SaveColors();
	while(1)
	{
		uint8_t new_s, old_s=0, i;
		static uint16_t val=0, cnt=0;
		GLCD_Value(5, 12, (int16_t) (++val));
		for (i=0;i<5;i++)
		{
			delay(200);
			new_s = Bit(0) & GPIOA->IDR;
			if (old_s==0 && new_s==1)
			{
				GPIOx_OUTPUT_togglePINS(GPIOF, Bit(10));
				LCD_SetColors(BLUE, GREEN);
				GLCD_Value(8, 10, (int16_t) (++cnt));
				LCD_RestoreColors();
			}
			old_s=new_s;
		}
	}
	


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


void GLCD_Value(uint8_t row_no, uint8_t col_no, int16_t val)
{
  char p_text[6] = "";

	sprintf(p_text, "%i ", val);  //% read a decimal, octal, or hexadecimal integer
	LCD_DisplayStringLineCol(row_no, col_no, p_text);
}

void DEMO_shifting(uint16_t x0, uint16_t y0, uint16_t xSize, uint16_t ySize, uint8_t infty)
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//		Screen Image Shifting Demo (1. Downward; 2. Leftward
//====================================================	
#include <stdlib.h>
#define Image_width  160	
#define Image_height 10
#define NumBypes_pixel 2
#define SZ_imagepool 	(Image_width * Image_height * NumBypes_pixel)	//160*10*2 = 3,200 = 0x0C80
																																		// 2 * 0x0C80 = 0x1900
uint8_t* pImage_0;	
uint8_t* pImage_1;
	 pImage_0 = (uint8_t*) malloc(SZ_imagepool);
		if (!pImage_0){
			LCD_DisplayStringAt(5, 100, "ERROR: NOT enough HEAP",  LEFT_MODE);
			LCD_DisplayStringAt(5, 116, "   Size of pImage_0 is",  LEFT_MODE);
			GLCD_Value(7, 12, (int16_t) (SZ_imagepool));
			while(1);
		}
	 pImage_1 = (uint8_t*) malloc(SZ_imagepool);
		if (!pImage_1){
			LCD_DisplayStringAt(5, 100, "ERROR: NOT enough HEAP",  LEFT_MODE);
			LCD_DisplayStringAt(5, 116, "   Size of pImage_1 is",  LEFT_MODE);
			GLCD_Value(7, 12, (int16_t) (SZ_imagepool));
			while(1);
		}

	if ((x0 + xSize-1) > LCD_Pixel_Width()) xSize = LCD_Pixel_Width()+1 - x0;
	if ((y0 + ySize-1) > LCD_Pixel_Height()) ySize = LCD_Pixel_Height()+1 - y0;
	while(1){
	uint8_t 	i, j, k, n_slice, nRDU;
	uint16_t	x_end= x0+xSize, y_end= y0+ySize;
	uint16_t	nSh_cpy, nSh_pst=0;
		
		// ---------- Down Shift
		n_slice = (uint8_t) ((ySize+9)/10);
		nRDU = ySize%10;		//	i.e.,	nRDU = ySize - (ySize/10)*10;
		if (nRDU == 0) nRDU = 10;
		for(k=0; k<3; k++){
			for(j=1; j<n_slice; j++)
			{
			uint16_t height = 10;
				delay(200); /* delay 200 ms */
				nSh_pst = y_end;
	//=========Get image data of LCD screen ====================
				LCD_ReadRGBImage(x0, y_end-10, xSize, 10, pImage_0);	
	//<<<================================
				for(i=1; i<n_slice; i++){
					if (i == n_slice-1){
						height = nRDU;
						nSh_pst = y0+10;
					} else {
						nSh_pst = nSh_pst-10;	// y0+ySize-10*i
					}
					LCD_ReadRGBImage(x0, nSh_pst-10, xSize, height, pImage_1);	
					LCD_DrawRGBImage(x0, nSh_pst, xSize, height, pImage_1);
				}
	//=========Load image data to LCD screen ====================
				LCD_DrawRGBImage(x0, y0, xSize, 10, pImage_0);	
	//<<<================================
			}
			delay(20*nRDU); /* delay 20*nRDU ms */
			nSh_pst = y_end;
			LCD_ReadRGBImage(x0, y_end-nRDU, xSize, nRDU, pImage_0);	
	//<<<================================
			for(i=1; i<n_slice; i++){
				nSh_pst = nSh_pst-10;	// n_end-10*i
				LCD_ReadRGBImage(x0, nSh_pst-nRDU, xSize, 10, pImage_1);	
				LCD_DrawRGBImage(x0, nSh_pst, xSize, 10, pImage_1);
			}
				LCD_DrawRGBImage(x0, y0, xSize, nRDU, pImage_0);	
		}		// end of k

		// ---------- Left Shift
		n_slice = (uint8_t) ((xSize+9)/10);
		nRDU = xSize%10;		//	i.e.,	nRDU = xSize - (xSize/10)*10;
		if (nRDU == 0) nRDU = 10;
		for(k=0; k<3; k++){
//			for(j=0; j<n_slice; j++){
			for(j=1; j<n_slice; j++){
			uint16_t width = 10;
				delay(200); /* delay 200 ms */
				nSh_cpy = x0;
	//=========Get image data of LCD screen ====================
				LCD_ReadRGBImage(x0, y0, 10, ySize, pImage_0);	
	//<<<================================
				for(i=1; i<n_slice; i++){
					if (i == n_slice-1) width = nRDU;
					nSh_cpy = nSh_cpy+10;	// x0+10*i
					LCD_ReadRGBImage(nSh_cpy, y0, width, ySize, pImage_1);	
					LCD_DrawRGBImage(nSh_cpy-10, y0, width, ySize, pImage_1);
				}
	//=========Load image data to LCD screen ====================
				LCD_DrawRGBImage(x_end-10, y0, 10, ySize, pImage_0);	
	//<<<================================
			}
			delay(20*nRDU); /* delay 20*nRDU ms */
			nSh_cpy = x0+nRDU;
			LCD_ReadRGBImage(x0, y0, nRDU, ySize, pImage_0);	
	//<<<================================
			for(i=1; i<n_slice; i++){
				nSh_pst = nSh_pst-10;	// n_end-10*i
				LCD_ReadRGBImage(nSh_cpy, y0, 10, ySize, pImage_1);	
				LCD_DrawRGBImage(nSh_cpy-nRDU, y0, 10, ySize, pImage_1);
				nSh_cpy = nSh_cpy+10;	// x0+nRDU + 10*i
			}
			LCD_DrawRGBImage(x_end-nRDU, y0, nRDU, ySize, pImage_0);	
		}		// end of k
		
		if (!infty ) break;
	}
    free(pImage_1);             /* Discard work area */
    free(pImage_0);             /* Discard work area */	
}
