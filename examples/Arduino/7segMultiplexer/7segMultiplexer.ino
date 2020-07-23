/******************************************************************************
  ULWOS2 example on Arduino, 4-digit LED display timer with breathing LED
  - Thread 1 - software PWM on builtin LED
  - Thread 2 - LED brightness control
  - Thread 3 - LED display multiplexer
  - Thread 4 - seconds timer

  Use 220R resistors to connect display segment pins to Arduino pins
  
  Author: Fábio Pereira
  Date: Jul, 22, 2020
  embeddedsystems.io
*******************************************************************************/

#include <ULWOS2.h>

#define DISP_SEG_A  8   // display segment A (pin 11) is mapped to Arduino pin D8
#define DISP_SEG_B  6   // display segment B (pin 7) is mapped to Arduino pin D6
#define DISP_SEG_C  4   // display segment C (pin 4) is mapped to Arduino pin D4
#define DISP_SEG_D  3   // display segment D (pin 2) is mapped to Arduino pin D3
#define DISP_SEG_E  2   // display segment E (pin 1) is mapped to Arduino pin D2
#define DISP_SEG_F  7   // display segment F (pin 10) is mapped to Arduino pin D7
#define DISP_SEG_G  5   // display segment G (pin 5) is mapped to Arduino pin D5
#define DISP_COM1   12  // display COM 1 (pin 12) is mapped to Arduino pin D12
#define DISP_COM2   11  // display COM 2 (pin 9) is mapped to Arduino pin D11
#define DISP_COM3   10  // display COM 3 (pin 8) is mapped to Arduino pin D10
#define DISP_COM4   9   // display COM 4 (pin 6) is mapped to Arduino pin D9
#define SEG_ON(x)   digitalWrite(x,1)
#define SEG_OFF(x)  digitalWrite(x,0)
#define DISP_ALL_OFF  {SEG_ON(DISP_COM1);SEG_ON(DISP_COM2);SEG_ON(DISP_COM3);SEG_ON(DISP_COM4);}
#define SHOW_DISP1  {SEG_OFF(DISP_COM1);SEG_ON(DISP_COM2);SEG_ON(DISP_COM3);SEG_ON(DISP_COM4);}
#define SHOW_DISP2  {SEG_ON(DISP_COM1);SEG_OFF(DISP_COM2);SEG_ON(DISP_COM3);SEG_ON(DISP_COM4);}
#define SHOW_DISP3  {SEG_ON(DISP_COM1);SEG_ON(DISP_COM2);SEG_OFF(DISP_COM3);SEG_ON(DISP_COM4);}
#define SHOW_DISP4  {SEG_ON(DISP_COM1);SEG_ON(DISP_COM2);SEG_ON(DISP_COM3);SEG_OFF(DISP_COM4);}
#define SHOW_NONE {SEG_OFF(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_OFF(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_OFF(DISP_SEG_G);}
#define SHOW0   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_OFF(DISP_SEG_G);}
#define SHOW1   {SEG_OFF(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_OFF(DISP_SEG_G);}
#define SHOW2   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_OFF(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW3   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW4   {SEG_OFF(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW5   {SEG_ON(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW6   {SEG_ON(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW7   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_OFF(DISP_SEG_G);}
#define SHOW8   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOW9   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_OFF(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOWA   {SEG_ON(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOWB   {SEG_OFF(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOWC   {SEG_ON(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_OFF(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_OFF(DISP_SEG_G);}
#define SHOWD   {SEG_OFF(DISP_SEG_A);SEG_ON(DISP_SEG_B);SEG_ON(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_OFF(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOWE   {SEG_ON(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_OFF(DISP_SEG_C);SEG_ON(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}
#define SHOWF   {SEG_ON(DISP_SEG_A);SEG_OFF(DISP_SEG_B);SEG_OFF(DISP_SEG_C);SEG_OFF(DISP_SEG_D);SEG_ON(DISP_SEG_E);SEG_ON(DISP_SEG_F);SEG_ON(DISP_SEG_G);}

#define DISPLAY_TIME  1 // in ms 

uint8_t dutyCycle = 0, period = 25; // breathing LED
uint16_t valueToDisplay = 0;

// Software PWM thread
void softPWMthread(void)
{
    int8_t sleepTime;
    ULWOS2_THREAD_START();
    while(1)
    {
        sleepTime = (dutyCycle * period) / 100;
        if (sleepTime>0) {
          digitalWrite(LED_BUILTIN, HIGH);  // turn LED on
          ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
        sleepTime = ((100 - dutyCycle) * period) / 100;
        if (sleepTime>0) {
          digitalWrite(LED_BUILTIN, LOW);   // turn LED off
          ULWOS2_THREAD_SLEEP_MS(sleepTime);
        }
    }
}

// LED brightness control (breathing)
void breathThread(void)
{
    ULWOS2_THREAD_START();
    while(1)
    {
        while (dutyCycle < 100) {
          dutyCycle++;
          ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        while (dutyCycle > 0) {
          dutyCycle--;
          ULWOS2_THREAD_SLEEP_MS(10); // update PWM every 10ms
        }
        // wait 250ms before starting another cycle
        ULWOS2_THREAD_SLEEP_MS(250);
    }
}

// Turn off all displays and segments
void allDisplaysOff(void)
{
  DISP_ALL_OFF;
  SHOW_NONE;
}

// decode and display a digit (0 to F)
// is supressZero is true then value zero is not displayed
void displayDecode(uint8_t data, bool suppressZero)
{
  allDisplaysOff();
  if (!data && suppressZero) return;
  switch (data) {
    case 0: SHOW0; break;
    case 1: SHOW1; break;
    case 2: SHOW2; break;
    case 3: SHOW3; break;
    case 4: SHOW4; break;
    case 5: SHOW5; break;
    case 6: SHOW6; break;
    case 7: SHOW7; break;
    case 8: SHOW8; break;
    case 9: SHOW9; break;
    case 10: SHOWA; break;
    case 11: SHOWB; break;
    case 12: SHOWC; break;
    case 13: SHOWD; break;
    case 14: SHOWE; break;
    case 15: SHOWF; break;
  }
}

// Display multiplexing thread
void displayThread(void)
{
    static uint16_t displayData;
    uint8_t digit;
    static bool suppressZero;  
    ULWOS2_THREAD_START();
    allDisplaysOff();
    while(1)
    {
        suppressZero = true;  // set this starting value to false if you don't want to supress zeros
        displayData = valueToDisplay;       // update internal display data
        // DIGIT ONE - THOUSANDS !!!
        digit = displayData/1000;           // calculate thousands digit
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 1000;  // update display data (remove thousands)
        SHOW_DISP1; // enable display 1 (thousands)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        // DIGIT TWO - HUNDREDS !!!
        digit = displayData/100;            // calculate hundreds
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 100;   // update display data (remove hundreds)
        SHOW_DISP2; // enable display 2 (hundreds)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        // DIGIT THREE - TENS !!!
        digit = displayData/10;             // calculate tens
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 10;// update display data (remove tens)
        SHOW_DISP3; // enable display 3 (tens)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        // DIGIT FOUR  - LAST ONE !!! 
        displayDecode(displayData, false);  // decode the remainder and send it to display (do not suppress zero)
        SHOW_DISP4; // enable display 4 (units)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time      
    }    
}

// Seconds thread
void oneSecondThread(void)
{
    ULWOS2_THREAD_START();
    DISP_ALL_OFF;
    while(1)
    {
        valueToDisplay++;
        // we can't display values higher than 9999, so wrap around
        if (valueToDisplay >= 10000) valueToDisplay = 0;
        // sleep for one second
        ULWOS2_THREAD_SLEEP_MS(1000);
    }
}

void setup() {
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(softPWMthread, 0);
    ULWOS2_THREAD_CREATE(breathThread, 2);
    ULWOS2_THREAD_CREATE(displayThread, 1);
    ULWOS2_THREAD_CREATE(oneSecondThread, 3);
    pinMode(LED_BUILTIN, OUTPUT);  // initialize LED I/O pin as output
    // Set all display pins as outputs
    pinMode(DISP_SEG_A, OUTPUT);
    pinMode(DISP_SEG_B, OUTPUT);
    pinMode(DISP_SEG_C, OUTPUT);
    pinMode(DISP_SEG_D, OUTPUT);
    pinMode(DISP_SEG_E, OUTPUT);
    pinMode(DISP_SEG_F, OUTPUT);
    pinMode(DISP_SEG_G, OUTPUT);
    pinMode(DISP_COM1, OUTPUT);
    pinMode(DISP_COM2, OUTPUT);
    pinMode(DISP_COM3, OUTPUT);
    pinMode(DISP_COM4, OUTPUT);
}

void loop() {
    // run ULWOS2 scheduler (it will run all threads that are ready to run)
    ULWOS2_START_SCHEDULER();
}
