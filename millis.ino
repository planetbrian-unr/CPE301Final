/*
    Group Name: TBD
    Group Members: Timothy Ang, Randall Cheng, Darren Ly, Brian Wu
    Assignment: Final Project
    Class: CPE 301 Spring 2024
    Due Date: 5/10/2024
*/

/* Allowed Libraries */
#include <Stepper.h>  // button/potentiometer-controlled vent dir control
#include <LiquidCrystal.h>  // output required messages
#include <RTClib.h>
#include <DHT.h>  // temp/humidity readings

/* UART0 Register Values */
#define RDA 0x80
#define TBE 0x20

// define pins for the stepper motor
#define STEPPER_PIN1 40
#define STEPPER_PIN2 41
#define STEPPER_PIN3 42
#define STEPPER_PIN4 43

// define pins for the buttons
#define BUTTON_RIGHT 9
#define BUTTON_LEFT 10
#define BUTTON_ON_OFF 6
#define BUTTON_RESET 26

// define pins for the LEDs
#define LED_PINR 32
#define LED_PINY 30
#define LED_PING 33
#define LED_PINB 31

//Vent thing
Stepper stepper(60, STEPPER_PIN1, STEPPER_PIN3, STEPPER_PIN2, STEPPER_PIN4);

//LCD
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

//Water pin and threshold
#define WATER_SENSOR A0
#define THRESHOLD 300 // the threshold for that sensor

//Humidity and Temperature
#define DHT_PIN 8 // Assuming DHT11 is connected to digital pin 8
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE); // Initialize DHT object

/* UART0 Pointers */
volatile unsigned char *myUCSR0A = (unsigned char *) 0xC0;
volatile unsigned char *myUCSR0B = (unsigned char *) 0xC1;
volatile unsigned char *myUCSR0C = (unsigned char *) 0xC2;
volatile unsigned int  *myUBRR0  = (unsigned int  *) 0xC4;
volatile unsigned char *myUDR0   = (unsigned char *) 0xC6;

/* Timer1 Pointers */
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned char *myTIFR1  = (unsigned char *) 0x36;
volatile unsigned int  *myTCNT1  = (unsigned int  *) 0x84;

//Fan pin
#define RELAY_PIN 7  // Define the pin for the relay control

//States
enum SystemState {
  DISABLED,
  IDLE,
  ERROR_STATE,
  RUNNING
};

//Track states
SystemState currentState = DISABLED;
SystemState previousState = DISABLED;
bool systemEnabled = false;

RTC_DS3231 rtc;  // Initialize the RTC object

/* For millis() */
unsigned long time_now = 0;


void setup() {
  U0Init(9600);
  setup_timer_regs();
  
  lcd.begin(16, 2); // set up number of columns and rows
  lcd.setCursor(0, 0); // move cursor to (0, 0)

  pinMode(WATER_SENSOR, INPUT);
  pinMode(RELAY_PIN, OUTPUT);  // Initialize relay pin as output
  digitalWrite(RELAY_PIN, LOW);

  dht.begin(); // Initialize DHT sensor
  
  pinMode(BUTTON_RIGHT, INPUT);
  pinMode(BUTTON_LEFT, INPUT);
  pinMode(BUTTON_ON_OFF, INPUT);
  pinMode(BUTTON_RESET, INPUT);

  pinMode(LED_PINR, OUTPUT);
  pinMode(LED_PINY, OUTPUT);
  pinMode(LED_PINB, OUTPUT);
  pinMode(LED_PING, OUTPUT);

  stepper.setSpeed(60);  // Set stepper motor speed to 60 RPM

  rtc.begin();

  // set the time on the RTC module
  DateTime now = DateTime(2024, 4, 23, 0, 0, 0);
  rtc.adjust(now);
}

void loop() {
  int waterLevel = analogRead(WATER_SENSOR); // Read water level sensor
  float temperature = dht.readTemperature(); // Read temperature
  float humidity = dht.readHumidity(); // Read humidity

  turnState(); // Turn right or left

  switch(currentState){
    case DISABLED:
      disabledState();
      break;
    case IDLE:
      idledState(temperature, humidity, waterLevel);
      break;
    case ERROR_STATE:
      errorState(temperature, humidity, waterLevel);
      break;
    case RUNNING:
      runningState(temperature, humidity, waterLevel);
      break;
  }
}

void turnState(){
  if(digitalRead(BUTTON_RIGHT) == LOW){
    stepper.step(60); // Clockwise
  }
  if(digitalRead(BUTTON_LEFT) == LOW){
    stepper.step(-60); // Counterclockwise
  }
}

void disabledState(){
  digitalWrite(LED_PINY, HIGH);
  digitalWrite(LED_PINR, LOW);
  digitalWrite(LED_PING, LOW);
  digitalWrite(LED_PINB, LOW);
  digitalWrite(RELAY_PIN, LOW);
  lcd.clear();
  if (digitalRead(BUTTON_ON_OFF) == LOW) {
    systemEnabled = !systemEnabled; // Toggle system state
  }
  if (systemEnabled) {
    currentState = IDLE;
    previousState = DISABLED;
  }
}

void idledState(float temperature, float humidity, int waterLevel){
  digitalWrite(LED_PING, HIGH);
  digitalWrite(LED_PINR, LOW);
  digitalWrite(LED_PINY, LOW);
  digitalWrite(LED_PINB, LOW);

  time_now = millis();
  while(millis() < time_now + 500); // delay(500);


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  if((currentState == IDLE && previousState == DISABLED) || (currentState == IDLE && previousState == RUNNING)){
    DateTime now = rtc.now();
    int year = now.year();
    int month = now.month();
    int day = now.day();
    int hour = now.hour();
    int minute = now.minute();
    int second = now.second();
    char numbers[10] = {'0','1','2','3','4','5','6','7','8','9'};
    int onesYear = year % 10;
    int tensYear = year / 10 % 10;
    int onesMonth = month % 10;
    int tensMonth = month / 10 % 10;
    int onesDay = day % 10;
    int tensDay = day / 10 % 10;
    int onesHour = hour % 10;
    int tensHour = hour / 10 % 10;
    int onesMinute = minute % 10;
    int tensMinute = minute / 10 % 10;
    int onesSecond = second % 10;
    int tensSecond = second / 10 % 10;
    
    U0putchar('M');
    U0putchar(':');
    U0putchar('D');
    U0putchar(':');
    U0putchar('Y');

    U0putchar(' ');
    
    U0putchar('H');
    U0putchar(':');
    U0putchar('M');
    U0putchar(':');
    U0putchar('S');

    U0putchar(' ');

    U0putchar(numbers[tensMonth]);
    U0putchar(numbers[onesMonth]);
    U0putchar(':');
    U0putchar(numbers[tensDay]);
    U0putchar(numbers[onesDay]);
    U0putchar(':');
    U0putchar(numbers[tensYear]);
    U0putchar(numbers[onesYear]);
    
    U0putchar(' ');

    U0putchar(numbers[tensHour]);
    U0putchar(numbers[onesHour]);
    U0putchar(':');
    U0putchar(numbers[tensMinute]);
    U0putchar(numbers[onesMinute]);
    U0putchar(':');
    U0putchar(numbers[tensSecond]);
    U0putchar(numbers[onesSecond]);

    U0putchar('\n');

    time_now = millis();
    while(millis() < time_now + 1000); // debounce, delay(1000);

    previousState = IDLE;
  }
  if (waterLevel < THRESHOLD) {
    currentState = ERROR_STATE;
  }
  else if(systemEnabled == true){
    currentState = RUNNING;
  }
  else{
    currentState = DISABLED;
  }
}

void errorState(float temperature, float humidity, int waterLevel){
  digitalWrite(RELAY_PIN, LOW);
  waterLevel = analogRead(WATER_SENSOR); // Read water level sensor
  if(digitalRead(BUTTON_RESET) == LOW && waterLevel >= THRESHOLD) {
    currentState = IDLE;
    previousState = ERROR_STATE;
  }
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water level is");
    lcd.setCursor(0, 1);
    lcd.print("too low");
    digitalWrite(LED_PINR, HIGH);
    digitalWrite(LED_PING, LOW);
    digitalWrite(LED_PINY, LOW);
    digitalWrite(LED_PINB, LOW);
    
    time_now = millis();
    while(millis() < time_now + 500); // delay(500);
  }
}

void runningState(float temperature, float humidity, int waterLevel){
  digitalWrite(LED_PINR, LOW);
  digitalWrite(LED_PING, LOW);
  digitalWrite(LED_PINY, LOW);
  digitalWrite(LED_PINB, HIGH);
  digitalWrite(RELAY_PIN, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  
  time_now = millis();
  while(millis() < time_now + 500); // delay(500);
  
  if(temperature < 23){
    currentState = IDLE;
    previousState = RUNNING;
  }
  if(waterLevel < THRESHOLD){
    currentState = ERROR_STATE;
    previousState = RUNNING;
  }
  if (digitalRead(BUTTON_ON_OFF) == LOW) {
    currentState = IDLE;
    previousState = RUNNING;
    systemEnabled = !systemEnabled;
    
    time_now = millis();
    while(millis() < time_now + 500); // delay(500);
  }
}

/* UART0 Functions */
void U0Init(int U0baud) /* Serial.begin(int) */ {
 *myUCSR0A = 0x20, *myUCSR0B = 0x18, *myUCSR0C = 0x06;
 *myUBRR0  = (16000000 / 16 / U0baud - 1);
}

unsigned char kbhit() /* Serial.available() */ {
  return *myUCSR0A & RDA;
}

unsigned char getChar() /* Serial.read() */ {
  return *myUDR0;
}

void U0putchar(unsigned char U0pdata) /* Serial.write(u_char) */ {
  while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}

/* Timer1 Functions */
void setup_timer_regs() {
  *myTCCR1A= 0x00, *myTCCR1B= 0X00, *myTCCR1C= 0x00;  // setup the timer control registers
  *myTIFR1 |= 0x01, *myTIMSK1 |= 0x01;  // reset TOV flag and enable TOV interrupt
}


