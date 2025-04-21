#include "?"

#define pin_0 0
#define pin_1 1
#define pin_2 2
#define pin_8 8
#define pin_10 10

void Driver_GPIO(void)
{
/*  --------------------------------------------------------------
    GPIOA: INPUT, pull-down
	  --------------------------------------------------------------
	  | Pin : PA |    0
    | Key 1    |   KEY1
	  --------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << ?);     // enable clock for GPIOA
	// 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_?*2)) );                       //Input mode (00b)
    GPIOA->PUPDR   = (GPIOA->PUPDR & ~(0x03ul<<(pin_?*2)) ) | (0x02ul<<(pin_?*2)); //pull-down (10b)
    GPIOA->AFR[?]  &= ~(0x0Ful<<pin_?*4);                                          // AF0 (0000b)

/*  --------------------------------------------------------------
    GPIOF: INPUT, floating (no Pull-Up or Pull-Down)
	  --------------------------------------------------------------
	  | Pin : PF        |    10
    | touch Module    |   TP_INT
	  --------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << ?);     // enable clock for GPIOF
	// 0:PORTA, 1: PORTB, ..., 10: PORTK
    GPIOF->MODER   = (GPIOF->MODER & ~(0x03ul<<(pin_?*2)) ); //Input mode (00b)
    GPIOF->PUPDR   = (GPIOF->PUPDR & ~(0x03ul<<(pin_?*2)) ); //no pull-up or pull-down (00b)
    GPIOF->AFR[?]  &= ~(0x0Ful<<(pin_? - pin_?)*4);          // AF0 (0000b)
}

