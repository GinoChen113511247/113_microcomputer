#include "stm32f407xx.h"
#include <stdbool.h>

#define pin_0   0
#define pin_1   1
#define pin_2   2
#define pin_8   8
#define pin_10  10
#define pin_13  13
#define Bit(x)  (1UL << (x))


#define SCREEN_INITIAL  0
#define SCREEN_KEY1_1   1
#define SCREEN_KEY1_2   2
#define SCREEN_TOUCH    3

/********************************************************************
 *  ????:Driver_GPIO
 *  ??:????????????
 ********************************************************************/
void Driver_GPIO(void)
{
/*  --------------------------------------------------------------
    GPIOA: INPUT, pull-down
    --------------------------------------------------------------
    | Pin : PA |    0
    | Key 1    |   KEY1
   --------------------------------------------------------------
*/
   RCC->AHB1ENR |=  (1UL << 0);     // enable clock for GPIOA
 // 0:PORTA, 1: PORTB, …, 10: PORTK
   GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_0 *2)) );                  //Input mode (00b)
   GPIOA->PUPDR   = (GPIOA->PUPDR & ~(0x03ul<<(pin_0 *2)) ) | (0x02ul<<(pin_0 *2)); //pull-down (10b)
   GPIOA->AFR[0]  &= ~(0x0Ful<<(pin_0 *4));           // AF0 (0000b)

/*  --------------------------------------------------------------
    GPIOF: INPUT, floating (no Pull-Up or Pull-Down) 
   --------------------------------------------------------------
   | Pin : PF           |    1     
   | touch Module  |   TP_INT  
   --------------------------------------------------------------
*/
   RCC->AHB1ENR |= (1UL << 5);     // enable clock for GPIOF
   GPIOF->MODER  &= ~(0x03ul << (pin_1 * 2));  // Input mode
   GPIOF->PUPDR  &= ~(0x03ul << (pin_1 * 2));  // Floating (no pull)

/*  --------------------------------------------------------------
    GPIOC: OUTPUT, Speed_25MHz medium, push pull, no pull 
   --------------------------------------------------------------
    | Pin: PC |    13       
    | SPI       |    CS   
    | initial   |   1(high)
    --------------------------------------------------------------
*/
   RCC->AHB1ENR |= (1UL << 2);     // enable clock for GPIOC
   // 0:PORTA, 1:PORTB, ..., 10: PORTK
   GPIOC->BSRR = Bit(pin_13);			// Initial output value for CS = 1
   GPIOC->MODER   = (GPIOC->MODER & ~(0x03ul << (pin_13 * 2))) | (0x01ul << (pin_13 * 2));  // Output mode (01b)
   GPIOC->OSPEEDR = (GPIOC->OSPEEDR & ~(0x03ul << (pin_13 * 2))) | (0x01ul << (pin_13 * 2)); // Speed_25MHz medium (01b)
   GPIOC->OTYPER &= ~(1UL << pin_13); // Push-pull (0)
   GPIOC->PUPDR  &= ~(0x03ul << (pin_13 * 2));   // No pull

/*  --------------------------------------------------------------
    GPIOB: OUTPUT, Speed_25MHz medium, push pull, no pull 
    --------------------------------------------------------------
   | Pin: PB |     0      
    | SPI       |    SCK   
    | initial   |   0 (low) 
    --------------------------------------------------------------
*/
//   RCC->AHB1ENR |=  (1UL << bit_GPIOB);     // enable clock for GPIOB
   RCC->AHB1ENR |= (1UL << 1);  // enable clock for GPIOB
   GPIOB->BSRR = Bit(pin_0) << 16; // Initial output value for SCK = 0 (reset bit)
   GPIOB->MODER   = (GPIOB->MODER & ~(0x03ul << (pin_0 * 2))) | (0x01ul << (pin_0 * 2));  // Output mode (01b)
   GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(0x03ul << (pin_0 * 2))) | (0x01ul << (pin_0 * 2)); // Speed_25MHz (01b)
   GPIOB->OTYPER  &= ~(1UL << pin_0); // Push-pull
   GPIOB->PUPDR   &= ~(0x03ul << (pin_0 * 2)); // No pull

/*  --------------------------------------------------------------
    GPIOF: OUTPUT, Speed_25MHz medium, push pull, no pull 
    --------------------------------------------------------------
    | Pin: PF |    11   
    | SPI       |  MOSI
    | initial   |   x (not care)
     --------------------------------------------------------------
*/
   // GPIOF clock already enabled above
   GPIOF->MODER   = (GPIOF->MODER & ~(0x03ul << (11 * 2))) | (0x01ul << (11 * 2)); // Output mode (01b)
   GPIOF->OSPEEDR = (GPIOF->OSPEEDR & ~(0x03ul << (11 * 2))) | (0x01ul << (11 * 2)); // Speed_25MHz (01b)
   GPIOF->OTYPER  &= ~(1UL << 11);  // Push-pull
   GPIOF->PUPDR   &= ~(0x03ul << (11 * 2)); // No pull

/*  --------------------------------------------------------------
    GPIOB: INPUT, float 
    --------------------------------------------------------------
    | Pin: PB |      2   
    | SPI        |    MISO  
    --------------------------------------------------------------
*/
   GPIOB->MODER   &= ~(0x03ul << (pin_2 * 2));  // Input mode (00b)
   GPIOB->PUPDR   &= ~(0x03ul << (pin_2 * 2));  // Floating (no pull)

}			// end of Driver_GPIO



#define CS_pin 	Bit(13)	// PC13
#define SCK_pin 	Bit(0)	// PB0
#define MISO_pin 	Bit(2)	// PB2
#define MOSI_pin 	Bit(11)	// PF11
#define GPIO_MI 	GPIOB	// PB2
#define GPIO_MO 	GPIOF	// PF11
#define GPIO_SCK 	GPIOB	// PB0
#define GPIO_CS 	GPIOC	// PC13
/***********************************************
*  Output Function: Driver_SPICS	(CS pin)
*  Object: touch ic spi enable/unable
*  brief:	set CS = 0 if t_f=0; CS = 1 if t_f = 1.
***********************************************/
void Driver_SPICS(bool t_f)
{
    if (!t_f) 	// t_f = 0
        GPIO_CS->BSRR = CS_pin<<16;	// reset ==> 0
    else	// t_f = 1
        GPIO_CS->BSRR = CS_pin;		// set ==> 1
}

/***********************************************
*  Output Function: Driver_SPISCK	(SCK pin)
*  Object: touch spi clock output
*  brief:	set SCK = 0 if t_f=0; SCK = 1 if t_f = 1.
***********************************************/
void Driver_SPISCK(bool t_f)
{
    if (!t_f) 	// t_f = 0
        GPIO_SCK->BSRR = SCK_pin<<16;	// reset ==> 0
    else	// t_f = 1
        GPIO_SCK->BSRR = SCK_pin;		// set ==> 1
}
/***********************************************
*  Output Function: Driver_SPIMOSI	(MOSI pin)
*  Object: master out
*  brief:	set MOSI = 0 if t_f=0; MOSI = 1 if t_f = 1.
***********************************************/
void Driver_SPIMOSI(bool t_f)
{
    if (!t_f) 	// t_f = 0
        GPIO_MO->BSRR = MOSI_pin<<16;	// reset ==> 0
    else	// t_f = 1
        GPIO_MO->BSRR = MOSI_pin;		// set ==> 1
}

/***********************************************
*  Input Function: Driver_SPIMISO	(MISO pin)
*  Object: master in
*  Return: 1 if MISO=1; 0 if MISO = 0 
***********************************************/
bool Driver_SPIMISO(void)
{
 if (GPIO_MI->IDR & MISO_pin)	// if MISO pin = 1
	return 1;
else
	return 0;
}


void Delay_SPI(uint16_t Num)
{
	volatile uint16_t Timer;
	while(Num--)
	{
	 	Timer = 20;
		while(Timer--);
	}
}
//#define Test_NormalCase
//--------- Receive Data from SPI
uint16_t Touch_SPIRead(void)
{
    uint8_t i;
    uint16_t Val = 0;

//	for(i=0; i<12; i++)			// 12-bit data: max value = 4095
	  // the least 4 significant bits are ignorable
	for(i=0; i<16; i++)
    {	// 16-bit data: max value = 65536
        Val <<= 1;
			#ifndef Test_NormalCase
        Driver_SPISCK(1);	// as soon as the master captures data
        Delay_SPI(1);
			#endif
        Driver_SPISCK(0);	// the slave loads data
        Delay_SPI(1);
        if(Driver_SPIMISO()){ // if MISO is high
	         Val++;
				}
			#ifdef Test_NormalCase
        Driver_SPISCK(1);	// as soon as the master captures data
        Delay_SPI(1);
			#endif
    }
			#ifdef Test_NormalCase
        Driver_SPISCK(1);	// as soon as the master captures data
        Delay_SPI(1);
			#endif
    return Val;
}
//--------- Send data to SPI
void Touch_SPIWrite(uint8_t Val)
{
    uint8_t i;
    Driver_SPISCK(0);  // make sure that the idle clock is low
    for(i=0; i<8; i++)		// 8 bit mode
	  {
	     if(Val & Bit(7)) // most significant bit first
	        Driver_SPIMOSI(1); // if Bit value = 1
	     else
	        Driver_SPIMOSI(0); // if Bit value = 0
	     Val <<= 1;
       Driver_SPISCK(0); // load at the FALLing edge of SCK clock
	     Delay_SPI(1);	 // hold half period
       Driver_SPISCK(1); // capture at the RISing edge of SCK Clock
	     Delay_SPI(1);	 // hold half period
    }
    Driver_SPISCK(0); 		// idle mode
    Delay_SPI(1);
}
