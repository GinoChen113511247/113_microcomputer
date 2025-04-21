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

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
uint8_t* pImage_x;	
	 pImage_x = (uint8_t*) malloc(SZ_imagepool_x);

	// Hardware intialization
	stm32f4_Hardware_Init ();
	//TFT intialization
	LCD_Init();
  delay(2000); /* delay 2000 ms */

  LCD_Clear(LCD_COLOR_GREEN);
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLUE); // Text = red; back = blue
  delay(1000); /* delay 1000 ms */

  //=========  GLCD test ;
  LCD_RGB_Test();
  LCD_DrawRect(45, 45, 180, 160);
	LCD_DrawBitmap(0, 0, stlogo);  // start at xPixel=0 (rightward), yPixel=0 (downward)
	LCD_DrawBitmap(0, 190, stlogo);  // start at xPixel=0 (rightward), yPixel=190 (downward)
	LCD_DrawBitmap(250, 0,  stlogo);  
	LCD_DrawBitmap(230, 60,  stlogo);  
  delay(1000); /* delay 1000 ms */
				LCD_ReadRGBImage(0, 0, 80, 40, pImage_x);	
				LCD_DrawRGBImage(60, 60, 80, 40, pImage_x);
    free(pImage_x);             /* Discard work area */

  LCD_Clear(BLACK);
	LCD_DrawBitmap(50, 50, (uint8_t *) 0x08060000);  // start at xPixel=50, yPixel=50
  delay(400); /* delay 400 ms */
	LCD_DrawBitmap(25, 25, (uint8_t *) 0x08060000);  // start at xPixel=25, yPixel=25
  delay(400); /* delay 400 ms */
	LCD_DrawBitmap(0, 0, (uint8_t *) 0x08060000);  // start at xPixel=0, yPixel=0
  delay(1000); /* delay 1000 ms */

//#define JPG_demo
#ifndef 	JPG_demo
	MenuInit();
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font12);
	LCD_DisplayStringLineCol(19, 0,"   Copyright (c) Shir-Kuan Lin, NCTU          " );		// line 10, column 0
	LCD_SetFont(&Font20);

//	LCD_DrawBitmap(60, 140, &startAddress);  // start at xPixel=60, yPixel=140
	LCD_DrawBitmap(190, 130, (uint8_t *) stlogo);  // start at xPixel=190 (rightward), yPixel=130 (downward)
#else
//================== JPG Demo =============================================

#ifdef Callback_Case		
//################# Callback Exercise ###############
#include "callback_example.h"
	register_JPG_callback(in_func_ROM);		// register a callback function
//#########################################
#endif

#define JPG_OK 0
#define JPG_fileAddress	0x08020000
uint8_t res;
uint8_t x0, y0;
uint16_t xSize, ySize;

// No Scaling: scaling factor = 1
	res = LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 0, 0);  // start at xPixel=0, yPixel=0
	if (res != JPG_OK)
	{
		LCD_SetTextColor(YELLOW);
		LCD_SaveFont();
		LCD_SetFont(&Font16);
		LCD_DisplayStringAt(5, 100, get_JPG_error_code(),  LEFT_MODE);
		LCD_RestoreFont();
		while(1);
	}
	delay(3000); /* delay 5000 ms */
	// scaling factor = 1/2
	LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 320, 240);  // start at xPixel=0, yPixel=0
  delay(1000); /* delay 1000 ms */
	// scaling factor = 1/4
	LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 160, 120);  // start at xPixel=0, yPixel=0
  delay(1000); /* delay 1000 ms */
	// scaling factor = 1/8
	LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 80, 60);  // start at xPixel=0, yPixel=0
  delay(2000); /* delay 1000 ms */
	//-----------------------------------------
  LCD_Clear(LCD_COLOR_GREEN);
	LCD_DrawJPG(0, 0, (uint8_t *) JPG_fileAddress, 320, 240);  // start at xPixel=0, yPixel=0
//>>>-----------------------
//	x0 = (320-160)/2 + 160-3;
//	y0 = (240-120)/2 + 120;
//<<<------------------------
	x0 = (320-160)/2;
	y0 = (240-120)/2;
	xSize = 160-7;
	ySize = 120-3;
	LCD_DrawRect(x0-1, y0-1, xSize+2, ySize+2);
	
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//		Screen Image Shifting Demo (1. Downward; 2. Leftward
//====================================================	
	DEMO_shifting(x0, y0, xSize, ySize, 1);		// 0: once; 1:infinity 
#endif

//================== GIF Demo =============================================
//#define GIF_demo
#define GIF_fileAddress	0x08040000
#ifdef 	GIF_demo
	LCD_DrawGIF(0, 0, (uint8_t *) 0x08040000, 320, 240);  // start at xPixel=0, yPixel=0
#endif

	while(1)
	{
		static uint16_t val=0;
		GLCD_Value(5, 12, (int16_t) (++val)); 
		delay(50); /* delay 100 ms */		
	}

}

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (10000 * nCount); index != 0; index--)
  {
  }
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
