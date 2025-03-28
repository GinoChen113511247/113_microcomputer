#include "stm32f407xx.h"

#define pin_0 0
#define pin_10 10
#define bit_GPIOA 0
#define bit_GPIOF 5

void samp1_GPIO(void)
{
/*  --------------------------------------------------------------
    GPIOA: INPUT, pull-down
    --------------------------------------------------------------
    | Pin : PA |    0
    | Key 1    |   KEY1
   --------------------------------------------------------------
*/
   RCC->AHB1ENR |=  (1UL << bit_GPIOA);     // bit_GPIOA: enable clock for GPIOA
 // 0:PORTA, 1: PORTB, …, 10: PORTK
   GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_0 *2)) );                  //Input mode (00b)
    GPIOA->PUPDR   = (GPIOA->PUPDR & ~(0x03ul<<(pin_0 *2)) ) | (0x02ul<<(pin_0 *2)); //pull-down (10b)
    GPIOA->AFR[0]  &= ~(0x0Ful<<pin_0 *4);           // AF0 (0000b)                

/*  --------------------------------------------------------------
    GPIOF: OUTPUT, Speed_25MHz medium, push pull, no pull 
   --------------------------------------------------------------
    | Pin: PF |        10   
    | LED2    |      
   --------------------------------------------------------------
*/
   RCC->AHB1ENR |=  (1UL << bit_GPIOF);     // bit_GPIOF: enable clock for GPIOF
   // 0:PORTA, 1: PORTB, …, 10: PORTK
    GPIOF->MODER   = (GPIOF->MODER & ~(0x03ul<<(pin_10 *2)) )  | (0x01ul<<(pin_10 *2)); //Output mode (01b)
    GPIOF->OSPEEDR = (GPIOF->OSPEEDR & ~(0x03ul<<(pin_10 *2)) ) |  (0x01ul<<(pin_10 *2)); //medium speed (01b)
    GPIOF->OTYPER  &= ~(0x01ul<<pin_10);         		// push-pull (0b)
    GPIOF->PUPDR   = (GPIOF->PUPDR & ~(0x03ul<<(pin_10 *2)) ); // no PU, no PD (00b)
    GPIOF->AFR[1]  &= ~(0x0Ful<<(pin_10 -8)*4);      // AF0 (0000b)
}
