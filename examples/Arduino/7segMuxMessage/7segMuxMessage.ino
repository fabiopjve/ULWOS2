/******************************************************************************
  ULWOS2 example on Arduino, 4-digit LED display timer with breathing LED
  using message queue
  - Thread 1 - software PWM on builtin LED
  - Thread 2 - LED brightness control
  - Thread 3 - LED display multiplexer
  - Thread 4 - seconds timer
  - Thread 5 - buttons processing

  Use 220R resistors to connect display segment pins to Arduino pins
  
  Author: Fábio Pereira
  Date: Aug, 09, 2020
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

#define RESET_BUTTON_PIN        A0  // the pin connected to the reset push button
#define DIRECTION_BUTTON_PIN    A1  // the pin connected to the direction push button
#define DEBOUNCE_TIME           30  // debounce time in ms

#define DISPLAY_TIME  1 // in ms 

uint8_t dispDecoder[17] = {
    0b00000000, // display off
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110, // 9
    0b11101110, // A
    0b00111110, // b
    0b10011100, // C
    0b01111010, // d
    0b10011110, // e
    0b10001110, // F
};

uint8_t dutyCycle = 0, period = 25; // breathing LED
uint16_t valueToDisplay = 0;

ULWOS2_CREATE_MESSAGE_QUEUE(keyPressed, uint16_t, 4);

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

// decode and display a digit (0 to F)
// is supressZero is true then value zero is not displayed
void displayDecode(uint8_t data, bool suppressZero)
{
    uint8_t dispSegments;
    DISP_ALL_OFF;
    if (data<16) {
        if (!data && suppressZero) data = 0;    // blank display
        else data++;    // increment data so we can access the right decoder
    } else {
        // blank display
        data = 0;
    }
    dispSegments = dispDecoder[data];
    digitalWrite(DISP_SEG_A, dispSegments & 0x80);
    digitalWrite(DISP_SEG_B, dispSegments & 0x40);
    digitalWrite(DISP_SEG_C, dispSegments & 0x20);
    digitalWrite(DISP_SEG_D, dispSegments & 0x10);
    digitalWrite(DISP_SEG_E, dispSegments & 0x08);
    digitalWrite(DISP_SEG_F, dispSegments & 0x04);
    digitalWrite(DISP_SEG_G, dispSegments & 0x02);
}

// Display multiplexing thread
void displayThread(void)
{
    static uint16_t displayData;
    uint8_t digit;
    static bool suppressZero;  
    ULWOS2_THREAD_START();
    while(1)
    {
        suppressZero = true;  // set this starting value to false if you don't want to supress zeros
        displayData = valueToDisplay;       // update internal display data
        // DIGIT ONE - THOUSANDS !!!
        digit = displayData/1000;           // calculate thousands digit
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 1000;  // update display data (remove thousands)
        SEG_OFF(DISP_COM1); // enable display 1 (thousands)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        SEG_ON(DISP_COM1); // enable display 1 (thousands)
        // DIGIT TWO - HUNDREDS !!!
        digit = displayData/100;            // calculate hundreds
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 100;   // update display data (remove hundreds)
        SEG_OFF(DISP_COM2); // enable display 2 (hundreds)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        SEG_ON(DISP_COM2); // enable display 2 (hundreds)
        // DIGIT THREE - TENS !!!
        digit = displayData/10;             // calculate tens
        if (digit) suppressZero = false;    // if this digit is not zero, stop suppressing zeros
        displayDecode(digit, suppressZero); // decode the digit and send it to display
        displayData -= (uint16_t)digit * 10;// update display data (remove tens)
        SEG_OFF(DISP_COM3); // enable display 3 (tens)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        SEG_ON(DISP_COM3); // enable display 3 (tens)        
        // DIGIT FOUR  - LAST ONE !!! 
        displayDecode(displayData, false);  // decode the remainder and send it to display (do not suppress zero)
        SEG_OFF(DISP_COM4); // enable display 4 (units)
        ULWOS2_THREAD_SLEEP_MS(DISPLAY_TIME);   // wait for some time
        SEG_ON(DISP_COM4); // enable display 4 (units)     
    }    
}

// Seconds thread - increments valueToDisplay every one second
void secondsThread(void)
{
    static bool direction = false;
    uint16_t tempData;
    ULWOS2_THREAD_START();
    DISP_ALL_OFF;
    while(1)
    {
        if (!direction) {
            valueToDisplay++;
            // we can't display values higher than 9999, so wrap around
            if (valueToDisplay >= 10000) valueToDisplay = 0;
        }
        else {
            valueToDisplay--;
            // we can't display values higher than 9999, so wrap around
            if (valueToDisplay >= 10000) valueToDisplay = 9999;
        }
        while (ULWOS2_MESSAGE_DEQUEUE(keyPressed,tempData)) {
            // we have received a message in our message queue
            if (tempData == RESET_BUTTON_PIN) valueToDisplay = 0;
            if (tempData == DIRECTION_BUTTON_PIN) direction = !direction;
        }        
        // sleep for one second
        ULWOS2_THREAD_SLEEP_MS(1000);
    }
}

// Process buttons and send messages when a button changes state to pressed
void processButtonsThread(void)
{
    static uint16_t lastButton;
    ULWOS2_THREAD_START();  // this is necessary for each thread in ULWOS2!
    while (1) {
        if (digitalRead(RESET_BUTTON_PIN) == LOW || digitalRead(DIRECTION_BUTTON_PIN) == LOW) {
            // if we are here, a button was pressed!
            // wait to make sure it is still pressed (debounce it)
            ULWOS2_THREAD_SLEEP_MS(DEBOUNCE_TIME);
            // check which button was pressed
            uint16_t thisButton;
            if (digitalRead(RESET_BUTTON_PIN) == LOW) thisButton  = RESET_BUTTON_PIN;
            else if (digitalRead(DIRECTION_BUTTON_PIN) == LOW) thisButton = DIRECTION_BUTTON_PIN;
            else thisButton = 0;
            // we only send a message when we detect a new button press
            if (thisButton && thisButton != lastButton) ULWOS2_MESSAGE_ENQUEUE(keyPressed, thisButton);
            lastButton = thisButton;       
        }
        ULWOS2_THREAD_SLEEP_MS(5);      
    }
}

void setup() {
    ULWOS2_INIT();
    ULWOS2_THREAD_CREATE(softPWMthread, 0);
    ULWOS2_THREAD_CREATE(breathThread, 2);
    ULWOS2_THREAD_CREATE(displayThread, 1);
    ULWOS2_THREAD_CREATE(secondsThread, 3);
    ULWOS2_THREAD_CREATE(processButtonsThread, 3);
    pinMode(LED_BUILTIN, OUTPUT);  // initialize LED I/O pin as output
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    pinMode(DIRECTION_BUTTON_PIN, INPUT_PULLUP);
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
