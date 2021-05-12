///////////////////////////////////// RECORDING STUIDIO SMART SWITCH ////////////////////////////////////////////////
// Hey! My name is Richard and this is the program for my studio smart switch. This switch will control two outlets 
// independently using 3 possible states: On, Off, and Recording. In the Off state, both outlets will be off. In the 
// On state, both outlets will be on. In the Recording state, one outlet will be on and one will be off. There will 
// be a screen connected which will display the current state and 3 hardware buttons will be used to activate their 
// respective states. 

// Feel free to check out the hardware here: https://drive.google.com/drive/folders/1OrVUQHJO3h5ACMl2yvR-MDkTQtR7tal_?usp=sharing


///////////////////////////////////// LIBRARY DECLARATIONS /////////////////////////////////////////////////////////

// These are the libraries I need for the OLED screen and a timer library to make it blink.

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <arduino-timer.h>

///////////////////////////////////// VARIABLE DECLARATIONS /////////////////////////////////////////////////////////

// First I define the i/o pin numbers for the buttons and the Relays

#define OFF_btn 2
#define ON_btn 3
#define REC_btn 4

#define RLY_ONE 5
#define RLY_TWO 6

// Then I define the screen size 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Then I define the pins for my SSD1306 display connected using software SPI
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

// Then I instantiate a display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Then I define the initial button states as HIGH since I'm using the
// internal pullup resistors

int OFF_btn_state = HIGH;
int ON_btn_state = HIGH;
int REC_btn_state = HIGH;

// Now I'll initialize a global state variable and set it to OFF.
// Since I'll have 3 global states, I'm using 1, 2, and 3 for simplicity.
// 1 = OFF, 2 = ON, 3 = REC

int GLOBAL_state = 1;

// Then I'll instantiate a timer for blinking the Rec state
auto timer = timer_create_default();


///////////////////////////////////// CUSTOM FUNCTION DECLARATIONS ///////////////////////////////////////////////////

// I'll define the set_global_state function
int Set_global_state (int x) {
  GLOBAL_state = x;
}

// and I'll define the Arduino reset function (just in case)
void(* resetFunc) (void) = 0;

// then I define the function to prepare screen for printing
void setupScreenToPrint(void) {
  display.clearDisplay();
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(50, 6);            // Start at top-left corner
}

// and define functions for printing 3 states & clearing screen

// This will print 'On' to the display
void printON(void) {
  display.println(F("On"));
  display.display();
}

// This will print 'Off' to the display
void printOFF(void) {
  display.println(F("Off"));
  display.display();
}

// This will print 'Rec' to the display
void printREC(void) {
  display.println(F("Rec"));
  display.display();
}

// This will clear the display
void printNothing(void) {
  display.println(F(""));
  display.display();
}

// Here I'm creating the logic for blinking the 'Rec' status on the OLED.
// First I create a 'Recording state' which represents whether or not 'Rec'
// is currently on the screen. 

// 1 = Screen is displaying 'Rec' 
// 0 = Screen is blank.

int REC_state = 1;

// Then I define a function that checks the state, changes it, and toggles
// between displaying 'Rec' and printing nothing on the screen. 

bool toggle_rec(void *) {
  setupScreenToPrint();
  if(REC_state == 0){
    REC_state = 1;
    printREC();
  } else {
    REC_state = 0;
    printNothing();
  }
  return true; // keep timer active? true
}


///////////////////////////////////// SETUP FUNCTION ////////////////////////////////////////////////////////////////

// Now I'll define the setup function
void setup() {
  
  // Then Set the pinMode for inputs
  pinMode(OFF_btn, INPUT_PULLUP);
  pinMode(ON_btn, INPUT_PULLUP);
  pinMode(REC_btn, INPUT_PULLUP);

  // Then Set the pinMode for outputs
  pinMode(RLY_ONE, OUTPUT);
  pinMode(RLY_TWO, OUTPUT);

  // Make sure relays are off
  digitalWrite(RLY_ONE, HIGH);
  digitalWrite(RLY_TWO, HIGH);

  // Then I rotate screen 180 degrees & clear display
  display.setRotation(2);

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Then I output "OFF" to the LCD Screen
  setupScreenToPrint();
  printOFF();
}

///////////////////////////////////// LOOP FUNCTION /////////////////////////////////////////////////////////////////

// Now I'll implement the logic for what I want the program to do. Basically the program works like this: 
// I check the current state and store it in the previous_state variable. Then I check the state of all the buttons 
// and store them in variables. Then, depending on which buttons were pressed, I change the global state. Then, I 
// check if the global state is different from the previous_state (indicating a change), and if it is, I implement 
// the new state by signalling the relays and displaying the new state. 

// I made the program work this way so that this loop can run indefinitely without doing anything so long as the state isn't changed,
// meaning if the state is 'Off' and you press the 'Off' button, nothing will happen & if no buttons are pressed, nothing will happen. 

void loop() {
  // So here's what all that looks like. 
  
  // This keeps my timer running
  timer.tick(); 

  // This is where I record the current state. 
  int previous_state = GLOBAL_state;

  // Then I read the state of each button & store the result in a variable
  OFF_btn_state = digitalRead(OFF_btn);
  ON_btn_state = digitalRead(ON_btn);
  REC_btn_state = digitalRead(REC_btn);

  // Then I'll compare the following:
  // Is a button being pressed?
  // Is that button the only one being pressed?
  // Is the desired state already active?

  // Off button pressed
  if (
    OFF_btn_state == LOW &&
    ON_btn_state == HIGH &&
    REC_btn_state == HIGH  &&
    GLOBAL_state != 1
  ) {
    Set_global_state (1);
  }

  // On button pressed
  else if (
    OFF_btn_state == HIGH &&
    ON_btn_state == LOW &&
    REC_btn_state == HIGH  &&
    GLOBAL_state != 2
  ) {
    Set_global_state (2);
  }

  // Record button pressed
  else if (
    OFF_btn_state == HIGH &&
    ON_btn_state == HIGH &&
    REC_btn_state == LOW  &&
    GLOBAL_state != 3
  ) {
    Set_global_state (3);
  }

  // All three buttons pressed, reset device.
  else if (
    OFF_btn_state == LOW &&
    ON_btn_state == LOW &&
    REC_btn_state == LOW
  ) {
    resetFunc();
  }

  // Then I check if the state has changed
  
  if ( previous_state != GLOBAL_state) {

  // If it has, I check if the previous_state was 3 (Rec) and cancel the timer if so. 
    
    if (previous_state == 3) {
      timer.cancel();
    }

  // Then I set the relays based on the new state. 
  
    if (GLOBAL_state == 1) {
      
      digitalWrite(RLY_ONE, HIGH);
      digitalWrite(RLY_TWO, HIGH);
      setupScreenToPrint();
      printOFF();
  
    } else if (GLOBAL_state == 2) {
      
      digitalWrite(RLY_ONE, LOW);
      digitalWrite(RLY_TWO, LOW);
      setupScreenToPrint();
      printON();
  
    } else if (GLOBAL_state == 3) {
      digitalWrite(RLY_ONE, LOW);
      digitalWrite(RLY_TWO, HIGH);
      setupScreenToPrint();
      printREC();

      // and here's where I make 'Recording' Blink every second when active.
      timer.every(1000, toggle_rec);
    }
  }
  // End Loop
}
