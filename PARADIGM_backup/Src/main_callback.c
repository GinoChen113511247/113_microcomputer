
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "user_defined.h"
//#include "?.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   0x08008000   /* Start @ of user Flash area */
                 // SECTOR 2
#define FLASH_USER_END_ADDR     0x08040000   /* End @ of user Flash area */
                 // SECTOR 6

//#define DATA_32                 ((uint32_t)0x12345678)
#define DATA_32                 ((uint32_t)0x87654321)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
uint32_t FirstSector = 0, NbOfSectors = 0, Address = 0;
uint32_t SectorError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;
*/

/*Variable used for Erase procedure*/
   
/* Private function prototypes -----------------------------------------------*/

																		
																		
/* Private functions ---------------------------------------------------------*/
void stm32f4_Hardware_Init (void);

//############ Just for remark ##############
void hold_key_ms(uint32_t wait_ms);

#define GPIO_led1		GPIOC
#define GPIO_led2		GPIOD
#define Pin_led1		0
#define Pin_led2		3

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

	stm32f4_Hardware_Init ();

	delay_ms(500);   // wait 0.5 sec
  GPIOx_OUTPUT_setPINS(GPIO_led1, Bit(Pin_led1));  // Turn off Led 1
	delay_ms(500);   // wait 0.5 sec
	
//------ Press User Key (PA0) to continue !!!! ------------
//===>>>> ONLY FOR #ifdef BUTTON_INTERRUPT
//#ifdef BUTTON_INTERRUPT
	hold_EXTI_ms(2000);

  while(get_ButtonHoldState() ==0){}							
//#endif	
//<<-------------------------------------------

  while (1)
  {
		GPIOx_OUTPUT_togglePINS(GPIO_led1, Bit(Pin_led1));
		GPIOx_OUTPUT_togglePINS(GPIO_led2, Bit(Pin_led2));
		delay_ms(500);   // wait 0.5 sec
  }

}


