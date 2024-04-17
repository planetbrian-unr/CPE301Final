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
#include <Clock.h>  // rtc for event reporting
#include <DHT11.h>  // temp/humidity readings

/* UART0 Register Values */
#define RDA 0x80
#define TBE 0x20

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


void setup() {
  U0Init(9600);
  setup_timer_regs();
}

void loop() {

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

void putChar(unsigned char U0pdata) /* Serial.write(u_char) */ {
  while((*myUCSR0A & TBE) == 0);
  *myUDR0 = U0pdata;
}

/* Timer1 Functions */
void setup_timer_regs() {
  *myTCCR1A= 0x00, *myTCCR1B= 0X00, *myTCCR1C= 0x00;  // setup the timer control registers
  *myTIFR1 |= 0x01, *myTIMSK1 |= 0x01;  // reset TOV flag and enable TOV interrupt
}

ISR(TIMER1_OVF_vect) /* Timer Overflow ISR */ {
  *myTCCR1B &= 0xF8;  // Stop the Timer
  *myTCNT1 = (unsigned int) (65535 - (unsigned long) (currentTicks));  // Load the Count
  *myTCCR1B |= 0x01;  // Start the Timer
  if(currentTicks != 65535) *portB ^= 0x40;  // if it's not the STOP amount, XOR to toggle PB6
}
