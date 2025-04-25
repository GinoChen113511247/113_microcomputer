
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "user_defined.h"
#ifdef Use_USART
#include <stdio.h>
// Note to include Serial_Retarget_printf.c,
// and to check item "USAART Configuration" in HardwareSystem_Ini.C
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   0x08008000   /* Start @ of user Flash area */
                 // SECTOR 2
#define FLASH_USER_END_ADDR     0x08040000   /* End @ of user Flash area */
                 // SECTOR 6

#define DATA_32                 ((uint32_t)0x87654321)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
   
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);

																		
																		
/* Private functions ---------------------------------------------------------*/
void stm32f4_Hardware_Init (void);


//#define BUTTON_INTERRUPT
void hold_key_ms(uint32_t wait_ms);

void PURGE_FLASH_READ_PROTECTION(void);
void	ChangeBMP_24bit_to_16bit(uint8_t* p_bmp, uint8_t* p_StoreAdd);

extern uint8_t startAddress;

#ifndef Board_B
#define GPIO_led1		GPIOC
#define GPIO_led2		GPIOD
#define Pin_led1		0
#define Pin_led2		3
#else
//==== for Board B ================
#define GPIO_led1		GPIOF
#define GPIO_led2		GPIOF
#define Pin_led1		9
#define Pin_led2		10

void Board_B_GPIO(void)
{
/*  --------------------------------------------------------------
    GPIOF: OUTPUT, Speed_50MHz fast, push pull, no pull 
	  --------------------------------------------------------------
	  | Pin: PF |     9      10   
    |         |    led 1  led 2
    | initial |   0(low)   0(low)
	  --------------------------------------------------------------
*/
#define pin_9 9
#define pin_10 10
#define bit_GPIOF 5

	#define  cc9_10   (0x0Ful<<(pin_9*2) )    	// 1111b<<(9*2)	
	#define  bb9_10   (0x05ul<<(pin_9*2) )    	// 0101b<<(9*2)	
		RCC->AHB1ENR |=  (1UL << bit_GPIOF);     // enable clock for GPIOF
	// 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOF->BSRR = (Bit(pin_9) | Bit(pin_10))<<16;	// Initial output value for led 1 & 2 = 0
    GPIOF->MODER   = (GPIOF->MODER & ~cc9_10 )  | bb9_10; // MODE
		GPIOF->OSPEEDR = (GPIOF->OSPEEDR & ~cc9_10) | (bb9_10<<1); // Output Speed: (fast) 1010b<<(9*2)	
    GPIOF->OTYPER  &= ~(0x03ul<<pin_9);         				// Output Type
    GPIOF->PUPDR   = (GPIOF->PUPDR & ~cc9_10 );  			// Pull-up Pull-down
		GPIOF->AFR[1]  &= ~(0xFFul<<(pin_9-8)*4);  				  // AF=0: System or Alternate Function
//<<<<<<<<<<<===============================
}
#endif
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t status, i, load_value;

	stm32f4_Hardware_Init ();
#ifdef Purge_Level_1
	PURGE_FLASH_READ_PROTECTION();
#endif
	
#ifdef Board_B
	Board_B_GPIO();
#endif	
	delay_ms(500);   // wait 0.5 sec
  GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
  GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));  // Turn off Led 2
	delay_ms(500);   // wait 0.5 sec
  GPIOx_OUTPUT_resetPINS(GPIO_led1, Bit(Pin_led1));  // Turn on Led 1
  GPIOx_OUTPUT_resetPINS(GPIO_led2, Bit(Pin_led2));  // Turn on Led 2
	delay_ms(500);   // wait 0.5 sec

#ifdef Use_USART
	  printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
#endif
	
//------ Press User Key (PA0) to continue !!!! ------------
#ifdef BUTTON_INTERRUPT
	hold_EXTI_ms(2000);

  while(get_ButtonHoldState() ==0){
		GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
		GPIOx_OUTPUT_resetPINS(GPIO_led2, Bit(Pin_led2));  // Turn on Led 2
		delay_ms(200);   // wait 0.2 sec
		GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));  // Turn off Led 2
		GPIOx_OUTPUT_resetPINS(GPIO_led1, Bit(Pin_led1));  // Turn on Led 1
		delay_ms(200);   // wait 0.2 sec
	}							
#else	
  while((GPIOA->IDR & 0x01) ==0)							// normally low
  {
		GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
		GPIOx_OUTPUT_resetPINS(GPIO_led2, Bit(Pin_led2));  // Turn on Led 2
		delay_ms(200);   // wait 0.2 sec
		GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));  // Turn off Led 2
		GPIOx_OUTPUT_resetPINS(GPIO_led1, Bit(Pin_led1));  // Turn on Led 1
		delay_ms(200);   // wait 0.2 sec
	}							

		// Only Led 1 on
	hold_key_ms(2000);
#endif	
	
  GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
  GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));  // Turn off Led 2
//========= Begin Flash Operation	

	load_value = DATA_32+ *((uint32_t*) FLASH_USER_START_ADDR);  //DATA_32 + content @ FLASH_USER_START_ADDR
	
	// check Write Protection bits of SECTORs 2~5
	if ((FLASH->OPTCR & (0x003Cul<<16)) != (0x003Cul<<16) )   // 0: Write Protection
	{
      FLASH_unlock_REGISTER_OPTCR;
		while((FLASH->SR & 0x00010000 ) != 0)
 //(Fail Code)==>     FLASH->OPTCR |= ((uint32_t)0x003C0000) | Bit(1);    // bit 1: start; SECTOR 2~5: 1= Write allowable
      FLASH->OPTCR |= ((uint32_t)0x003C0000);    //SECTOR 2~5 (bits 18~21): 1= Write allowable
	// write 1 to bit 1 after setting accessible sector number
		FLASH->OPTCR |= ((uint32_t)0x003C0000) | Bit(1);    // bit 1: start; SECTOR 2~5 (bits 18~21) : 1= Write allowable
		while((FLASH->SR & 0x00010000 ) != 0);
	}
	
  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_unlock_REGISTER_CR;
  FLASH_clear_allErrorStatus_SR;

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

#define PSIZE_word  FLASH_PSIZE  // 0: 1B, 1: 2B, 2: 4B
  status = User_FLASH_SectorErase(START_FLASH_SECTOR, FINAL_FLASH_SECTOR, PSIZE_word);
	  if (status != (FINAL_FLASH_SECTOR+1))		// != EndSECTOR + 1
    { 
      Error_Handler();
    }
	
  GPIOx_OUTPUT_resetPINS(GPIO_led2, Bit(Pin_led2));  // Turn on Led 2

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

	status = User_FLASH_beforeWRITE(PSIZE_word);
	if (status == 0)
	{
		FLASH_clear_allErrorStatus_SR;
		
		for (i=Address; i<FLASH_USER_END_ADDR; i=i+4)
	  {
			*(volatile uint32_t *)i = (uint32_t) load_value;
    /* Wait for last operation to be completed */
	    status = FLASH_check_BUSY_bit();          // state: 0=OK, 1=error, 3=timeout
	  }

		FLASH_clear_REGISTER_CR_9bits;
	}	
	
	
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
	FLASH_lock_REGISTER_CR;

  GPIOx_OUTPUT_resetPINS(GPIO_led1, Bit(Pin_led1));  // Turn on Led 1
  /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = 0x0;
  
  while (Address < FLASH_USER_END_ADDR)
  {
    data32 = *(__IO uint32_t*)Address;

    if (data32 != load_value)
    {
      MemoryProgramStatus++;  
    }

    Address = Address + 4;
  }  

  /*Check if there is an issue to program data*/
  if (MemoryProgramStatus == 0)
  {
    /* No error detected. Switch on LED5*/
   GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));			// Turn off Led 2
  }
  else
  {
    /* Error detected. Switch on LED5*/
    Error_Handler();
  }
//<<<<<<<<<<<<<<<<<=======================
  
	
	//########################################################
  //###### A practical example with the file "includeBinary.s" ####
	//########################################################

  /* Erase the user Flash area AGAIN *************/ 
		FLASH_unlock_REGISTER_CR;
		FLASH_clear_allErrorStatus_SR;
		status = User_FLASH_SectorErase(START_FLASH_SECTOR, FINAL_FLASH_SECTOR, PSIZE_word);

		
		status = User_FLASH_beforeWRITE(PSIZE_word);
	  i = FLASH->CR;
		i &= ~(Bit(8) | Bit(9) );
		i |= Bit(8);   // PSIZE = 16 bits
		FLASH->CR = i;

		if (status == 0)
		{
			FLASH_clear_allErrorStatus_SR;
		
	//------>>>>>>>>>>		
	ChangeBMP_24bit_to_16bit(&startAddress, (uint8_t*) FLASH_USER_START_ADDR);
  //------<<<<<<<<<<<
			
			FLASH_clear_REGISTER_CR_9bits;
		}	
		FLASH_lock_REGISTER_CR;

  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* Turn LED5 (RED) on */
  while(1)
  {
		GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
		GPIOx_OUTPUT_resetPINS(GPIO_led2, Bit(Pin_led2));  // Turn on Led 2
		delay_ms(80);   // wait 0.2 sec
		GPIOx_OUTPUT_setPINS(GPIO_led2, Bit(Pin_led2));  // Turn off Led 2
		GPIOx_OUTPUT_resetPINS(GPIO_led1, Bit(Pin_led1));  // Turn on Led 1
		delay_ms(80);   // wait 0.2 sec
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


