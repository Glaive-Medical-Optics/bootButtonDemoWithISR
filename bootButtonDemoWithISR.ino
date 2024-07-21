/* 
 *    This demo program shows how to set up an ISR (Interrupt Service Routine)
 *    that will get called when the boot button has been pressed
 *    on an Arduino Giga R1 board.
 *    
 *    Author: Glaive Software Group
 *    Revised: 7/21/2024.
 *    
 *    After running this program, each time you click the boot button,
 *    the LED lights will change,
 *    "Click" means press and release.
 *    
 *    After the first click, the red LED comes on.
 *    On the next click, the green LED comes on instead.
 *    On the third click, the blue LED comes on instead.
 *    After another click, all LEDs go off and the cycle repeats thereafter.
 *    
 *    The sketch uses an interrupt routine, so your loop() code can be busy doing other work.
 *    
 *    This program is Free Software and has ABSOLUTELY NO WARRANTY.
 */

typedef enum
{
    UP,
    DOWN
} ButtonState;

enum
{
    RED_LED,
    GREEN_LED,
    BLUE_LED,
    NO_LED
};

#define   NONE    0

int LEDpin[4] = {LEDR, LEDG, LEDB, NONE};
char *LEDcolor[4] = {"red","green","blue","none"};  // should you need to have a word representing the state

int LEDstate = NO_LED;  // initially, no LED is lit.

int currentPin = NONE;

ButtonState currentButtonState = UP;

void setup()
{
  // Initialize the serial communication
  Serial.begin(9600);
      while (!Serial) ; // Wait for the serial port to connect.

  // Enable the LED pins. They are initially off.

  pinMode(LEDR, OUTPUT);    // red
  pinMode(LEDG, OUTPUT);    // green
  pinMode(LEDB, OUTPUT);    // blue

  configureBootButton();
}

void loop()
{
    // Compute intensive code can go here
    // and will get temporarily interrupted to change LED colors.
}

void configureBootButton()
{
    // On the Arduino Giga R1 board, the boot button is monitored by GPIO pin PC_13.

  // Enable the clock for port GPIOC.

  RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;

  // Configure pin PC_13 as an input pin.

  GPIOC->MODER &= ~(GPIO_MODER_MODE13); // Clear the mode bits to put the pin in input mode.

  /*
   *    Set up an interrupt routine to be called whenever
   *    the boot button is pressed or released.
   */

  Serial.println("Attaching interrupt.");
  Serial.println("Press the boot button to change which LED is lit.");
  attachInterrupt(digitalPinToInterrupt(PC_13), bootButtonISR, CHANGE);
}

/*
 *    This routine gets called any time the boot button state changes (changes to up or down).
 *    Note that you cannot write to the serial monitor from within an interrupt service routine.
 */

void bootButtonISR()
{
  // Read the state of the boot button
  int pinState = GPIOC->IDR & GPIO_IDR_ID13;

  // Check the state of the boot button.
  // Pin LOW means the button is up; HIGH means the button is down.
  
  if (pinState == LOW)
  {
      // boot button is up
      if (currentButtonState == DOWN)
      {
          currentButtonState = UP;

          // Advance to the next color state

          if (currentPin) digitalWrite(currentPin, HIGH);   // Turn off previous color if there was one.
          LEDstate++;
          if (LEDstate > NO_LED) LEDstate = RED_LED;
          currentPin = LEDpin[LEDstate];
          if (currentPin) digitalWrite(currentPin, LOW);   // Light up this pin.
      }
  }
  else // pinState = HIGH
  {
      // boot button is down
      if (currentButtonState == UP)
      {
          currentButtonState = DOWN;
      }
  }
}
