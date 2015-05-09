#include <rtttl.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int ledBeat = 10; // Led connected to pin 10
const int heartSensor = A0; // Piezo sensor connected to pin A0
int tempPin = A1; // LM35 connected to pin A1
//int tempPin = analogRead(A1); // LM35 connected to pin A1
float tempRead; // Read the LM35 values
float tempRead_c;
float tempRead_f;

int threshold = 5; // Threshold for the piezo sensor 
int oldvalue = 0; // Old value for piezo sensor
int newvalue = 0; // New value for piezo sensor
int cnt = 0; // Used for timing 
int timings[16]; // Timing values for the piezo sensor

int firstRun = 1; // Tells it to run the splashscreen

int buttonCounter_one;
int toggleCtoF;

// Wait time for the millis timers
unsigned long oldmillis = 0;
unsigned long newmillis = 0;
unsigned long waitUntil = 0;
unsigned long waitUntilTemp = 0;

int fullBattery = 880; // Battery max value
int emptyBattery = 300; // Battery min value
int batteryPercent; // Batttery percent

// Button one 
const int button_one = 5; // Button set to pin 5
unsigned long time_button_one = 0; // the last time the output pin was toggled
unsigned long debounce_one = 200; // the debounce time, increase if the output flickers
int reading_button_one; // the current reading from the input pin
int previous_button_one = HIGH; // the previous reading from the input pin

// Button two
const int button_two = 6; // Button set to pin 5
unsigned long time_button_two = 0; // the last time the output pin was toggled
unsigned long debounce_two = 200; // the debounce time, increase if the output flickers
int reading_button_two; // the current reading from the input pin
int previous_button_two = HIGH; // the previous reading from the input pin

// Speaker stuff
int pinSpeaker = 9;  // Speaker pin 9 
const int octave = 0;  // The octave set for the player
const char heartBeat[] PROGMEM ="heartBeat:16e"; // Makes the heart beat sound
Rtttl player;  // Song player

uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0, 0x0}; // Custom char heart
uint8_t temp[8] = {0x4, 0xa, 0xa, 0xa, 0x11, 0x1f, 0x1f, 0xe}; // Custom char temp 
uint8_t temp_c[8] = {0x8, 0xf4, 0x8, 0x43, 0x4, 0x4, 0x43, 0x0}; // Custom char degrees c
uint8_t temp_f[8] = {0x8, 0xf4, 0x8, 0x7, 0x4, 0x7, 0x4, 0x4}; // Custom char degrees f
uint8_t battery[8] = {0xe, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f}; // Custom char battery

void setup() {
  Serial.begin(9600); 
  Serial.println("#T_H_S");

  player.begin(pinSpeaker);  // Starts the player

  lcd.init(); // Start up the lcd
  lcd.backlight(); // Turn on the lcd backlight
  lcd.createChar(0, heart); // Custom Char heart
  lcd.createChar(1, temp); // Custom Char temp
  lcd.createChar(2, temp_c); // Custom Char degrees C
  lcd.createChar(3, battery); // Custom Char battery
  lcd.createChar(4, temp_f); // Custom char degrees F
  lcd.begin(16, 2); // Set up the lcd to have 16 char on 2 lines

  pinMode(ledBeat, OUTPUT); 

  pinMode(button_one, INPUT); // Set the button as input
  digitalWrite(button_one, HIGH); // initiate the internal pull up resistor
  pinMode(button_two, INPUT); // Set the button as input
  digitalWrite(button_two, HIGH); // initiate the internal pull up resistor
  }

void splashScreen() {
  lcd.clear(); // Clears the lcd
  lcd.setCursor(0,0); // Set cursor to start of the second screen
  lcd.print("#T_H_S"); // Prints to the LCD
  delay(1000);
  firstRun = 0; // Sets firstRun to 0 so it wouldnt run the warmup code again
  lcd.clear(); // Clears the lcd
  lcd.print((char)0); // Print custom heart sign to LCD
  lcd.print(" "); // Prints to the LCD
  lcd.print(" bpm "); // Prints to the LCD
}

void playSong(const char * track)  // Takes the song_# pasted from the if statements below
{
  player.play_P(track, octave);  // Passes the song_# and octave to the player
}

void loop() {

  // Runs splashscreen
  if (firstRun == 1) {
    splashScreen();
  }

 // ------------- Debound code button one start code -------------
  reading_button_one = digitalRead(button_one);

  if (reading_button_one == HIGH && previous_button_one == LOW && millis() - time_button_one > debounce_one) { 
    time_button_one = millis(); 
    // Do something here
    buttonCounter_one ++; // increments when the button is pressed
    if (buttonCounter_one == 3) { // When it reaches 3 it resets
      buttonCounter_one = 0; // Resets the button counter
      }
  }
  previous_button_one = reading_button_one;
   
 // -------------- Debound code button one end code -------------

 // ------------- Debound code button two start code -------------
  reading_button_two = digitalRead(button_two);

  if (reading_button_two == HIGH && previous_button_two == LOW && millis() - time_button_two > debounce_two) { 
    time_button_two = millis(); 
    // Do something here, button doesnt do anything yet
    toggleCtoF ++;
    Serial.println(toggleCtoF);
    lcd.clear();
    if (toggleCtoF == 2) {
      toggleCtoF = 0;
      }
      }
  
  previous_button_two = reading_button_two;
   
 // -------------- Debound code button two end code -------------

  if (millis() >= waitUntilTemp) {
    tempRead_c = analogRead(tempPin); // read analog pin to get temp
    tempRead_c = tempRead_c * 0.48828125; // Converts reading to C
    tempRead_f = tempRead_c *9 / 5; // Converts reading to f
    tempRead_f = tempRead_f + 32; // Converts reading to f

    lcd.setCursor(0,1); // Set cursor to start of the second screen
    lcd.print((char)1); // Print custom temp sign to LCD
    lcd.print(" "); // Prints to the LCD
    if (toggleCtoF == 1) {
      lcd.print(tempRead_c); // Prints to the LCD
      lcd.print((char)2); // Print custom temp_c sign to LCD
      }
    else {
      lcd.print(tempRead_f);
      lcd.print((char)4); // Print custom temp_f sign to LCD
      }
    lcd.print(" "); // Prints to the LCD

    int batteryRead = analogRead(A2); // Reads battery pin
    batteryPercent = map(batteryRead, emptyBattery, fullBattery,0 ,100); // Maps the battery percent from high and lows

    lcd.setCursor(11,1); // Set cursor
    lcd.print((char)3); // Print custom battery icon to LCD
    lcd.print(batteryPercent); // Print to the LCD
    lcd.setCursor(16,1); // Set cursor
    lcd.print("%"); // Print to the LCD

  waitUntilTemp =+ 1000;
  }

  if (millis() >= waitUntil) {
    oldvalue = newvalue;
    newvalue = 0;
    for(int i=0; i<64; i++){ // Average over 16 measurements
      //newvalue += analogRead(A2);
      newvalue += analogRead(heartSensor);
    }
    newvalue = newvalue/64;
    // find triggering edge
    if(oldvalue<threshold && newvalue>=threshold){ 
      oldmillis = newmillis;
      newmillis = millis();

      // fill in the current time difference in ringbuffer
      timings[cnt%16]= (int)(newmillis-oldmillis); 
      int totalmillis = 0;
      // calculate average of the last 16 time differences
      for(int i=0;i<16;i++){
        totalmillis += timings[i];
      }

      // calculate heart rate
      int heartrate = 60000/(totalmillis/16);
      //Serial.println(heartrate,DEC);
      cnt++;
      if (buttonCounter_one == 0) { 
        digitalWrite(ledBeat, HIGH);
        }

      if (buttonCounter_one <= 1) {
        playSong(heartBeat); // Play button sound
      }

      lcd.clear(); // Clears the lcd
      lcd.setCursor(0,0); // Set cursor to start of the second screen
      lcd.print((char)0); // Print custom heart sign to LCD
      lcd.print(" "); // Prints to the LCD
      lcd.print(heartrate,DEC); // Prints to the LCD
      lcd.print(" bpm "); // Prints to the LCD
    }
    waitUntil += 5;
    }

    digitalWrite(ledBeat, LOW);

  }
