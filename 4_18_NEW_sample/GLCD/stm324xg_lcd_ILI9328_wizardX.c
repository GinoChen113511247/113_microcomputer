/*//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
*/
//
//=================================================
//     <h> FSMC Memory Bank Selection
//     <o0> (1~25) FSMC Address bit for RS pin (Register Sel) <1-25>
//     <o1.26..27> NOR/PSRAM 1~4 Selection
//                 <0=> NE1
//                 <1=> ?
//                 <2=> ?
//                 <3=> ?
//     </h>
#define  RS_bit							 6
#define	 LCD_BASE_sel        0x6C000000
#define	 LCD_BASE        (LCD_BASE_sel | (0x01UL<<(RS_bit+1))-2)

//==============================================================
//     <?> Fonts selection: check to include *.C files in this project
//         <q?>  font8.c 
//         <q?>  font12.c
//         <q?>  font16.c 
//         <q?>  font20.c 
//         <q?>  font24.c 
//     <?>
//--------------------------
//  ???????
//--------------------------
//     <o?.0..3> LCD Display Direction
//          <??>    Landscape
//          <?=>    Portrait
//--------------------------
#define __Fonts_select         0x0000001E			//0
#define __default_Font         3			//1
#define __LCD_DIRECT	         1      //2 ==> 1: LANDSCAPEÆÁ  2: PORTRAIT

//=================================================
//     <e?> LCD Reset Pin (default PC5)
//     <o1>   port label of LCD Reset pin
//						<i> 0: PORTA, 1: PORTB, ..., 10: PORTK
//                 <0=> PORT A 
//                 <1=> PORT ? 
//                 <2=> PORT ? 
//                 <3=> PORT ? 
//                 <4=> PORT ? 
//                 <5=> PORT ? 
//                 <6=> PORT ? 
//                 <7=> PORT ? 
//                 <8=> PORT ? 
//                 <9=> PORT ? 
//                 <10=> PORT ? 
//     <o2>  (0~15) Pin number of LCD Reset pin <0-?>
//     </e>
//     <?> LCD Backlight Pin (default PB0)
//     <o3>  

//     <o4>  
//     <?>
//=================================================
#define USE_resetPIN 0		// 0
#define PNr_reset 2		// 1
#define PIN_rs	5     // 2
#define PNr_backlight 1		// 3
#define PIN_bl	15         // 4
//=============================================

//=================================================
//     <h> LCD FSMC Pin in Set 1
//						<i> PD0~PD1 ==> FSMC D2~D3
//						<i> PD4, PD5 ==> FSMC NOE, NWE
//						<i> PD8~PD10 ==> FSMC D13~D15
//						<i> PD14~PD15 ==> FSMC D0~D1
//						<i> PD7 ==> FSMC NE1
//						<i> PD11~PD13 ==> FSMC A16~A18
//     <o?>   ??????????
//     <h>  Pins of FSMC Set 1
// ?????????????????????
//     </h>
//     </h>
//----------------LCD FSMC Pin in Set 2-------------------------------
// ?????
//						<i> PE7~PE15 ==> FSMC D4~D12
//						<i> PE2, PE3~PE6 ==> FSMC A23, A19~A22
//----------------LCD FSMC Pin in Set 3----------------------
// ?????
//						<i> PG9~PG10, PG12 ==> FSMC NE2~NE3, NE4
//						<i> PG0~PG5 ==> FSMC A10~A15
//						<i> PG13~PG14 ==> FSMC A24~A25
//----------------LCD FSMC Pin in Set 4----------------------
// ?????
//						<i> PF0~PF5 ==> FSMC A0~A5
//						<i> PF12~PF15 ==> FSMC A6~A9
#define PNr_FSMC_set1 3									// 0
#define PinBit_FSMC_set1 0xC733     		// 1
#define PNr_FSMC_set2 4									// 2
#define PinBit_FSMC_set2	0xFF80        // 3
#define PNr_FSMC_set3 6									// 4
#define PinBit_FSMC_set3	0x1000        // 5
#define PNr_FSMC_set4 5									// 6
#define PinBit_FSMC_set4	0x1000        // 7


//=================================================
//     <o?> Default Text Color
//       <0xFFFF=> LCD_COLOR_WHITE
//       <?=> LCD_COLOR_BLUE
//       <?=> LCD_COLOR_RED
//       <?=> LCD_COLOR_MAGENTA
//     <o?> Default Background Color
//       <?=> LCD_COLOR_BLACK
//       <?=> LCD_COLOR_YELLOW
//       <?=> LCD_COLOR_GREEN
//       <?=> LCD_COLOR_GRAY

#define Default_TextColor		0xFFFF
#define Default_BackColor		0x0000

//=================================================
//     <q0> Show LCD device ID
//=================================================
#define Show_DeviceID		 1		// 0
//=============================================

// --------???????????????????
