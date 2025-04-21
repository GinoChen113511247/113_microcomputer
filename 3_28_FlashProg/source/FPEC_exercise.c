/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "user_defined.h"
#include <stdio.h>

//======================================
static void Flash_Error_message(uint32_t status)
{
	switch (status){
		case 1:
			LCD_DisplayStringLineCol(3, 1, "Flash Error! " );		
			return;
		case 3:
			LCD_DisplayStringLineCol(3, 1, "Flash Time out! " );		
			return;
		default:
			return;
	}
}

void	ChangeBMP_24bit_to_16bit(uint8_t * p_BmpAdd, \
	uint8_t * p_StoreAdd)
{
  uint32_t index = 0, size; 
  uint16_t bitCount;
	
	uint32_t height = 0, i=0;
  uint32_t width  = 0, j = 0;
  
  /* Read bitmap width */
  width = *(uint16_t *) (p_BmpAdd + 18);
  width |= (*(uint16_t *) (p_BmpAdd + 20)) << 16;
  
  /* Read bitmap height */
  height = *(uint16_t *) (p_BmpAdd + 22);
  height |= (*(uint16_t *) (p_BmpAdd + 24)) << 16; 
 
  /* Read number of bits per pixel */
  bitCount = *(uint16_t *) (p_BmpAdd + 28);

  /* Read bitmap size */
//  size = *(__IO uint16_t *) (p_BmpAdd + 2);
//  size |= (*(__IO uint16_t *) (p_BmpAdd + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(__IO uint16_t *) (p_BmpAdd + 10);
  index |= (*(__IO uint16_t *) (p_BmpAdd + 12)) << 16;

	size = (width*height)*2 + index;
	index += (uint32_t) p_StoreAdd;
	i = (uint32_t) p_StoreAdd;
	
  if (bitCount == 24)
	{
		uint32_t status;
		uint32_t skip;
			
		skip = 0x03 & (4 - ( (width*3) &0x03));   // align in 4 multiple
		for (; (uint32_t) p_StoreAdd < index; )
		{
			uint16_t temp;
			temp = *(__IO uint16_t *)p_BmpAdd;
			if (((uint32_t) p_StoreAdd) == (i+2)) temp = (uint16_t)size;	
			if (((uint32_t) p_StoreAdd) == (i+4)) temp = (uint16_t)(size>>16);	
			if (((uint32_t) p_StoreAdd) == (i+28)) temp = 16;	// bitCount = 16
			//if (((uint32_t) p_StoreAdd) == (i+30)) temp = 3;	// Type of Compression = 3 (signal of  16 bits per pixel)
			if (((uint32_t) p_StoreAdd) == (i+34)){
				size = (width*height)*2;
				temp = (uint16_t)size;	
			}
			if (((uint32_t) p_StoreAdd) == (i+36)) temp = (uint16_t)(size>>16);	
					
			*(volatile uint16_t *)p_StoreAdd = temp;
			p_BmpAdd += 2;
			p_StoreAdd += 2;
    /* Wait for last operation to be completed */
				status = FLASH_check_BUSY_bit();          // state: 0=OK, 1=error, 3=timeout
//				FLASH_check_BUSY_bit();          // state: 0=OK, 1=error, 3=timeout
			}

		for (i = 0; i<height; i++)
		{ 
			for (j =0; j <width; j++)
			{
				uint16_t temp;
				temp = (*(__IO uint8_t *)p_BmpAdd++)>>3;
				temp |= ((*(__IO uint8_t *)(p_BmpAdd++) )>> 3)  << 6;
				temp |= ((*(__IO uint8_t *) (p_BmpAdd++) )>> 3)  << (6+5);

				*(volatile uint16_t *)p_StoreAdd =  temp;
				p_StoreAdd += 2;
    /* Wait for last operation to be completed */
				status = FLASH_check_BUSY_bit();          // state: 0=OK, 1=error, 3=timeout
//				FLASH_check_BUSY_bit();          // state: 0=OK, 1=error, 3=timeout
			}
			p_BmpAdd += skip;
		}
	}

}
//===================================================
// return value: BitCount= 0 (not a bmp file), 16, 24
//---------------------------------------------------
uint32_t BMP_check_show(uint16_t x_pixel, uint16_t y_pixel, uint8_t *p_BmpAdd)
{
	uint16_t tmp16;
	
	LCD_SaveColors();
	LCD_SetBackColor(GREEN);
	tmp16 = *(uint16_t *) (p_BmpAdd + 0);	// read BMP signature
	if (tmp16 != 0x4D42){
		LCD_DisplayStringLineCol(1, 1, "It's NOT a bmp file!" );
		tmp16 = 0;
	} else{
		tmp16 = *(uint16_t *) (p_BmpAdd + 28);	// // read number of bits per pixel
		LCD_DrawBitmap(0, 0, p_BmpAdd);  // start at xPixel=0, yPixel=0
		switch (tmp16){
			case 16:
				LCD_DisplayStringLineCol(1, 1, "already 16-bit bmp!" );		
				break;
			case 24:
				LCD_DisplayStringLineCol(1, 1, " 24-bit bmp file!" );		
				break;
			default:
				break;
		}
	}
	LCD_RestoreColors();
	return (uint32_t) tmp16;
}
//================================================
void Flash_Erase_for_Write(uint32_t beginSector, uint32_t endSector)
{
     uint32_t tmp;

	LCD_SaveColors();
	LCD_SetBackColor(YELLOW);
  /* Erase the user Flash area *************/ 
	FLASH_unlock_REGISTER_CR;	// unlock FLASH->CR
	FLASH_clear_allErrorStatus_SR;	// clear FLASH->SR
	if ( (beginSector<2) || (beginSector>11) || (endSector<beginSector) || (endSector>11) )
	{
			LCD_DisplayStringLineCol(3, 1, "Sector No. should be" );	
			LCD_DisplayStringLineCol(4, 1, "  within 2~11!" );	
			while(1){}	
	}
	tmp = User_FLASH_SectorErase(beginSector, endSector, 2);  	// 2: FLASH_PSIZE = 4 Bytes

	if(tmp <= beginSector){
			LCD_DisplayStringLineCol(3, 1, "No Sectors is erased!" );	
			LCD_DisplayStringLineCol(5, 1, "Check Option Bytes");
			LCD_DisplayStringLineCol(6, 1, "of Flash!");
			while(1){}
	}else{
		char p_text[20] = "";
			
		LCD_DisplayStringLineCol(3, 1, "Erased Sect.: ");		
		sprintf(p_text, "%i ~ %i", beginSector, tmp-1);  //% read a decimal, octal, or hexadecimal integer
		LCD_DisplayStringLineCol(3, 15, p_text);		
	}
	LCD_RestoreColors();
}

