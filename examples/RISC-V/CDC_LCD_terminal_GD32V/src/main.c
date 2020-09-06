/*
    CDC  terminal with LCD using ULWOS2

    This example runs two threads:
    - one prints a string and toggles green LED every 10 seconds
    - a second thread that handles CDC processing and character echoing and toggles 
      red LED for each character (or block) received
    - a third thread that prints characters to the LCD

    This example was inspired in the code found at:
    https://github.com/linusreM/GD32V-Virtual-COM-Port

    Author: Fábio Pereira
    www.embeddedsystems.io
    Date: September, 6rd, 2020
*/

#include "lcd.h"
#include "cdc_core.h"
#include "gd32v_pjt_include.h"
#include "ULWOS2.h"

ULWOS2_DEFINE_SIGNAL(SIGNAL_NEW_CHARACTER);

char newCharacter;
static uint8_t px, py;
uint16_t currentColor = CL_WHITE;

/*
    Prints a message on CDC device every 10 seconds
*/
void printEveryTenSeconds_thread(void)
{
	ULWOS2_THREAD_START();
	while(1)
	{
        ULWOS2_THREAD_SLEEP_MS(10000);
        cdc_print("Periodic ULWOS2 thread, 10 seconds!\n");
        LEDG_TOG;   // toggle green LED
	}
}

/*
    Processes characters received on CDC device and prints them on the LCD
*/
void terminal_thread(void)
{
    ULWOS2_THREAD_START();
    while (1)
    {
        // always wait for a new character to arrive
        ULWOS2_THREAD_WAIT_FOR_SIGNAL(SIGNAL_NEW_CHARACTER);
        // a new character arrived, prints it if printable
        if (newCharacter >= ' ' && newCharacter <= '~') {
            LCD_ShowChar(px, py, newCharacter, 0, currentColor);
            px += 8;
        } else if (newCharacter == '\b') {
            // this is a backspace
            if (px >= 8) {
                px -= 8;
                LCD_ShowChar(px, py, ' ', 0, currentColor);
            }
        } else if (newCharacter == '\r') {
            // this is a return
            if (py < 64) py += 16;
            px = 0;
        } else if (newCharacter == 4) {
            // this is a CTRL + D, clear the screen
            LCD_Clear(CL_BLACK);
            px = 0;
            py = 0;
        }
    }
}

/*
    CDC thread, performs CDC processing and handles received data
*/
void CDC_thread(void)
{
	ULWOS2_THREAD_START();
    // wait here until USB CDC is configured
    while (!cdc_isConfigured()) ULWOS2_THREAD_SLEEP_MS(1);
    // create printing and terminal threads
    ULWOS2_THREAD_CREATE(printEveryTenSeconds_thread, 10);
    ULWOS2_THREAD_CREATE(terminal_thread, 5);
	while(1)
	{
        cdc_process();
        uint8_t buf[64];
        uint32_t rxLength = cdc_getReceivedData(buf, 64);
        if (rxLength) {
            cdc_sendData(buf, rxLength);
            // if we receive a single character, send it to terminal thread, otherwise print the string
            if (rxLength) {
                newCharacter = buf[0];
                ULWOS2_THREAD_SEND_SIGNAL(SIGNAL_NEW_CHARACTER);
            } else LCD_ShowString(0, 32, (char*)buf, CL_RED);
            LEDR_TOG;   // toggle red LED
        }
        // sleep for 1ms so that other threads can run
        ULWOS2_THREAD_SLEEP_MS(1);
	}
}

void longanNanoInit(void)
{
    SystemInit();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1|GPIO_PIN_2);
    LEDR(1);
    LEDG(1);
    LEDB(1);
    cdc_init();
    Lcd_Init();			// init OLED
    LCD_Clear(CL_BLACK);
    BACK_COLOR = CL_BLACK;
    LCD_ShowString(0, 0, "ULWOS2", CL_BLUE);
    LCD_ShowString(0, 16, "CDC Terminal", CL_GREEN);
}

int main(void)
{
    longanNanoInit();
	ULWOS2_INIT();
	ULWOS2_THREAD_CREATE(CDC_thread, 5);
	ULWOS2_START_SCHEDULER();	
}