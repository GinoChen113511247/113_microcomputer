#include "stm32f4xx.h"
#include "stm324xg_lcd_sklin.h"
#include "touch_module.h"
#include <stdlib.h>
#include <string.h>
#include "user_defined.h"
#include <stdio.h>  // for sprintf

// ---------------------------- constants ----------------------------------
#define GRID_COLS           3
#define GRID_ROWS           3
#define HOLE_SIZE           50
#define HOLE_SPACING_X      20
#define HOLE_SPACING_Y      20
#define TOP_MARGIN          40
#define MOLE_SHOW_TIME_MS   600
#define LCD_BGCOLOR         LCD_COLOR_LIGHTGREEN
#define HOLE_COLOR          LCD_COLOR_BROWN
#define MOLE_COLOR          LCD_COLOR_BROWN
#define HOLE_BORDER_COLOR   LCD_COLOR_BLACK
#define TouchSenseCount     1
#if (TouchSenseCount == 1)
#define TouchJudge          (!TP_INT_IN)
#else
#define TouchJudge          TSC_TouchDet(TouchSenseCount)
#endif

// ------------------------- global variables -----------------------------
static uint16_t holeX[GRID_COLS];
static uint16_t holeY[GRID_ROWS];
static uint32_t score = 0;
static uint8_t currentMole = 0xFF;
static TS_StateTypeDef TS_State;

// ------------------------- helper prototypes -----------------------------
static void computeGrid(void);
static void drawBackground(void);
static void drawHoles(void);
static void spawnMole(uint8_t idx);
static void clearMole(uint8_t idx);
static uint8_t pickRandomHole(uint8_t prev);
static uint8_t touchHitTest(uint16_t x, uint16_t y);
static void updateScore(void);
static void LCD_ClearLine(uint8_t line);

void stm32f4_Hardware_Init(void);
void Driver_GPIO(void);
void Default_Calibration(void);
void TS_GetState(TS_StateTypeDef* pTS_State);
void WaitForTouchRelease(uint32_t Cnum);
void delay_ms(uint32_t ms);

#ifndef Bit
#define Bit(x)  (0x01ul << x)
#endif

/**
 * Whac-A-Mole demo: main game loop function
 */
void WhackAMole_Run(void)
{
    // Define long-press threshold and timer for button exit
#define KEY_LONGTIME 10
    uint16_t KeyTimer = KEY_LONGTIME;

    // Initialize hardware, LCD display, and touch interface
    stm32f4_Hardware_Init();
    LCD_Init();
    LCD_Clear(GREEN);
    LCD_DisplayOn();
    Default_Calibration();
    Driver_GPIO();

    // Set up game grid, background, holes, and initial score display
    computeGrid();
    drawBackground();
    drawHoles();
    updateScore();

    srand(0x1234);

    // Start spawning moles in a loop
    while (1)
    {
        // Pick and display a new mole
        currentMole = pickRandomHole(currentMole);
        spawnMole(currentMole);

        uint32_t t0 = msTick;
        uint8_t hit = 0;

        // Check for touch input or button exit until timeout
        while ((msTick - t0) < MOLE_SHOW_TIME_MS)
        {
            TS_GetState(&TS_State);
            if (TS_State.TouchDetected)
            {
                if (touchHitTest(TS_State.x, TS_State.y))
                {
                    hit = 1;
                    WaitForTouchRelease(50);
                    break;
                }
            }

            // Check if KEY1 is held long enough to exit
            if (GPIOA->IDR & Bit(0))  // if KEY1 is pressed (assuming pressed pulls PA0 high)
            {
                if (--KeyTimer == 0)
                {
                    LCD_Clear(LCD_BGCOLOR);     // optional: clear screen before exit
                    return;                     // exit the game function
                }
            }
            else
            {
                KeyTimer = KEY_LONGTIME;  // reset timer when button released
            }

            delay_ms(10);
        }

        // Remove mole; if hit, increase score
        clearMole(currentMole);
        if (hit)
        {
            score++;
            updateScore();
        }

        // Small pause before next mole
        delay_ms(300);

        // Check again if KEY1 is held long enough to exit outside the touch loop
        if (GPIOA->IDR & Bit(0))  // if KEY1 is pressed
        {
            if (--KeyTimer == 0)
            {
                LCD_Clear(LCD_BGCOLOR);     // optional: clear screen before exit
                return;                     // exit the game function
            }
        }
        else
        {
            KeyTimer = KEY_LONGTIME;  // reset timer when button released
        }
    }
}

// -------------------------- game drawing ---------------------------------
static void computeGrid(void)
{
    uint16_t totalW = GRID_COLS * HOLE_SIZE + (GRID_COLS - 1) * HOLE_SPACING_X;
    uint16_t startX = (LCD_Pixel_Width() + 1 - totalW) / 2;
    uint16_t totalH = GRID_ROWS * HOLE_SIZE + (GRID_ROWS - 1) * HOLE_SPACING_Y;
    uint16_t startY = TOP_MARGIN;

    for (uint8_t c = 0; c < GRID_COLS; ++c)
        holeX[c] = startX + c * (HOLE_SIZE + HOLE_SPACING_X) + HOLE_SIZE / 2;
    for (uint8_t r = 0; r < GRID_ROWS; ++r)
        holeY[r] = startY + r * (HOLE_SIZE + HOLE_SPACING_Y) + HOLE_SIZE / 2;
}

static void drawBackground(void)
{
    LCD_Clear(LCD_BGCOLOR);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLineCol(0, 1, "Whack-A-Mole");
}

static void drawHoles(void)
{
    LCD_SetTextColor(HOLE_BORDER_COLOR);
    for (uint8_t r = 0; r < GRID_ROWS; ++r)
        for (uint8_t c = 0; c < GRID_COLS; ++c)
            LCD_DrawCircle(holeX[c], holeY[r], HOLE_SIZE / 2);
}

static void spawnMole(uint8_t idx)
{
    uint8_t r = idx / GRID_COLS;
    uint8_t c = idx % GRID_COLS;
    LCD_SetTextColor(MOLE_COLOR);
    LCD_FillCircle(holeX[c], holeY[r], (HOLE_SIZE / 2) - 4);
}

static void clearMole(uint8_t idx)
{
    uint8_t r = idx / GRID_COLS;
    uint8_t c = idx % GRID_COLS;
    LCD_SetTextColor(LCD_BGCOLOR);
    LCD_FillCircle(holeX[c], holeY[r], (HOLE_SIZE / 2) - 4);
}

// ------------------------------ utils ------------------------------------
static uint8_t pickRandomHole(uint8_t prev)
{
    uint8_t idx;
    do { idx = rand() % (GRID_ROWS * GRID_COLS); } while (idx == prev);
    return idx;
}

static uint8_t touchHitTest(uint16_t x, uint16_t y)
{
    if (currentMole == 0xFF) return 0;
    uint8_t r = currentMole / GRID_COLS;
    uint8_t c = currentMole % GRID_COLS;
    int32_t dx = (int32_t)x - holeX[c];
    int32_t dy = (int32_t)y - holeY[r];
    return (dx * dx + dy * dy) <= (((HOLE_SIZE / 2) - 4) * ((HOLE_SIZE / 2) - 4));
}

static void updateScore(void)
{
    char buf[16];
    LCD_SetBackColor(LCD_BGCOLOR);
    LCD_SetTextColor(LCD_COLOR_BLUE);
    sprintf(buf, "Score:%lu", (unsigned long)score);
    LCD_DisplayStringLineCol(1, 1, buf);
}

static void LCD_ClearLine(uint8_t line)
{
    uint16_t h = ((sFONT *)LCD_GetFont())->Height;
    LCD_SetTextColor(LCD_BGCOLOR);
    LCD_FillRect(0, line * h, LCD_Pixel_Width() + 1, h);
}
