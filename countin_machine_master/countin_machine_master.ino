#include <Wire.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Servo.h>
Servo myservo;


int menuOption ;
int setCounter = 25; // default counter setting
// Variables will change:
int counter = 1;
int IRState = 0;
int lastIRState = 0;
int pos = 70; // initial servo position
int relayState = LOW; //relay position defines motor on start on or off

// Define constants
const int DEBOUNCE_DELAY = 50;
const int  IRPin = 2; // ir sensor
const int ServoPin = 6; // servo conncection com
int motorPin = 8;
// joystick connections
int xPin = A3;
int yPin = A4;
const int buttonPin = A2; // servo switch button

int buttonPinPreviousState = HIGH;
int currentButtonState;

//buttons definition
#define START_BTN A0    //input pin for stop button
#define ZERO_BTN A5   // define pin for set counter zero
#define debounceTimeout 20

int xPinPreviousState = HIGH;
int yPinPreviousState = HIGH;



int StartButtonLastState;    // the previous state of startbutton
int currentStartButtonState; // the current state of startbutton

// Define variables

unsigned long lastDebounceTime = 0;

// Menu options
char * menuOptions[] = {"Count 25", "Count 50"};
bool featureSetting[] = {false, false};
bool menuMode = false;
bool menuNeedsPrint = false;
int optionSelected = 0;

void setup() {
  //joystick
  pinMode(xPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(yPin, INPUT_PULLUP);

  //IR sensor
  // initialize the button pin as an input:
  pinMode(IRPin, INPUT);
  myservo.attach(6);
  myservo.write(pos);

  pinMode(motorPin, OUTPUT);// initialize motor PIN
  pinMode(START_BTN, INPUT_PULLUP);     //connect a normally open switch between A0 and Ground
  pinMode(ZERO_BTN, INPUT_PULLUP);      // zero counter
  currentStartButtonState = digitalRead(START_BTN);
  lcd.begin();
  lcd.clear();
  lcd.print("Welcome Master");
  lcd.setCursor(0, 1);
  lcd.print("Press Buttons");
  delay(3000);




}

void loop() {

  buttonPinPreviousState  = currentButtonState; // save the last state
  currentButtonState = digitalRead(buttonPin); // read new state

  if  ((currentButtonState == LOW) && (buttonPinPreviousState == HIGH)) {
    delay(20);
    if (pos == 10)
      pos = 70;
    else if (pos == 70)
      pos = 10;
    myservo.write(pos);

    if (pos == 10) {
      lcd.clear();
      lcd.print("BOX 1 ");
    }
    else if (pos == 70) {
      lcd.clear();
      lcd.print("BOX 2 ");
    }
  }
  int xPinPressed = digitalRead(xPin);
  int yPinPressed = digitalRead(yPin);
  long int currentTime = millis();

  int start_state = digitalRead(START_BTN);


  if ((xPinPressed == LOW) && (xPinPreviousState == HIGH)) {
    menuOption = 25;
    setCounter = menuOption;
    lcd.clear();
    lcd.print("Counting: 25");
    lcd.setCursor(0, 1);
    lcd.print("Press to confirm");
  }
  if ((yPinPressed == LOW) && (xPinPreviousState == HIGH)) {
    menuOption = 50;
    setCounter = 50;
    lcd.clear();
    lcd.print("Counting: 50");
    lcd.setCursor(0, 1);
    lcd.print("Press to confirm");
  }
  StartButtonLastState = currentStartButtonState;      // save the last state
  currentStartButtonState = digitalRead(START_BTN); // read new state

  if (StartButtonLastState == HIGH && currentStartButtonState    == LOW) {
    // toggle state of relay
    relayState = !relayState;
    // control relay arccoding to the toggled state
    digitalWrite(motorPin, relayState);
    lcd.clear();
    if (relayState == HIGH) {
      lcd.print("ION DRIVE ON");
      delay(1000);
      lcd.clear();
      lcd.print("Counting: ");
      lcd.print(menuOption);
      lcd.setCursor(0, 1);
      lcd.print("Current count: ");
      lcd.print(counter);

    }
    else if (relayState == LOW) {
      lcd.print("ION DRIVE OFF");
      delay(1000);
      lcd.clear();
      lcd.print("Counting stop ");
      lcd.setCursor(0, 1);
      lcd.print("Current count: ");
      lcd.print(counter);
    }
    delay(50);
  }
  IRState = digitalRead(IRPin);
  if (IRState != lastIRState) {
    // if the state has changed, increment the counter
    if (IRState == HIGH) {
      counter++;
      lcd.clear();
      lcd.print("Counting: ");
      lcd.print(menuOption);
      lcd.setCursor(0, 1);
      lcd.print("Current count: ");
      lcd.print(counter);
      delay(10);
    }
    // Delay a little bit to avoid bouncing
    delay(10);
  }
  // save the current state as the    last state for next time through the loop
  lastIRState = IRState;
  if (counter >= setCounter) {
    counter = 0;
    lcd.print("Count finished ");
    digitalWrite(motorPin, LOW); // turn the motor OFF
    pos = myservo.read();
    if  (pos != 70) {
      for (pos = 10; pos <= 70; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
      }
    }
    else if (pos == 70) {
      for (pos = 70    ; pos >= 10; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        lcd.print("BOX 2 ");
      }
    }
    delay(15);
  }
  int zero_state = digitalRead(ZERO_BTN);
  if (zero_state == LOW) { //if zero pressed..
    (counter = 0);
    lcd.clear();

    delay(900);
    lcd.clear();
    lcd.print("Ready to count: ");
    lcd.setCursor(0, 1);
    lcd.print(menuOption);
    delay(2000);
    lcd.clear();
    lcd.print("Counting :");
    lcd.setCursor(10, 0);
    lcd.print(menuOption);
    lcd.setCursor(0, 1);
    lcd.print("Press Start");

  }
}
