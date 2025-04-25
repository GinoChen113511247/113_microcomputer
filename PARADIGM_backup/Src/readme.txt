/**
   use UART3 to show the message on your PC
   GPIO ==> LEDs flashing and User button key hold-pressing operations
   FLASH_EraseProgram FLASH Erase and Program example
**/

(1) Hold user button over 2 seconds to enter the flash programming example

(2) @par Example Description 

This example guides you through the different configuration steps 
to erase and program the STM32F4xx internal FLASH memory mounted on 
STM32F4-Discovery RevB & RevC board.

After Reset, the Flash memory Program/Erase Controller is locked. A dedicated function
is used to enable the FLASH control register access.
Before programming the desired addresses, an erase operation is performed using 
the flash erase sector feature. The erase procedure is done by filling the erase init 
structure giving the starting erase sector and the number of sectors to erase.
At this stage, all these sectors will be erased one by one separately.

@note: if problem occurs on a sector, erase will be stopped and faulty sector will 
be returned to user (through variable 'SectorError').

Once this operation is finished, sector word programming operation will be performed 
in the FLASH memory. The written data is then read back and checked.

STM32F4-Discovery RevB & RevC board's LEDs can be used to monitor the transfer status:
 - LED4 (GREEN) is ON when there are no errors detected after data programing
 - LED5 (RED) is ON when there are errors detected after data programing
 - LED6 (BLUE) is ON when there is an issue during erase procedure
 - LED3 (ORANGE) is ON when there is an issue during program procedure

(3) @par Example Description 

   _________________________ 
  |           ______________|                       _______________
  |          |USART3        |                      | HyperTerminal |
  |          |              |                      |               |
  |          |      TX(PC10)|______________________|RX             |
  |          |              |                      |               |
  |          |              |     RS232 Cable      |               |             
  |          |              |                      |               |
  |          |      RX(PC11)|______________________|TX             |          
  |          |              |                      |               |           
  |          |______________|                      |_______________|          
  |                         |                       
  |                         |                    
  |                         |                      
  |                         |                      
  |_STM324xG________________|                      

This example shows how to retarget the C library printf function to the UART. 
This implementation output the printf message on the Hyperterminal using USART3 
Tx=PC10, Rx=PC11.

The USART3 is configured as follow:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled


@par Hardware and Software environment 

  - This example runs on STM32F407xx Devices.
    
  - This example has been tested with STM32F4-Discovery RevB & RevC board and can be
    easily tailored to any other supported device and development board.
  
@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your MDK-ARM uvision
 - Rebuild all files and load your image into target memory
 - Run the example


