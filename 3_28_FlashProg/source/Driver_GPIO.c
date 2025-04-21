#include "stm32f407xx.h"

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