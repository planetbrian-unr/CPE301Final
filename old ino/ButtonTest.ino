/*
#define BUTTON_ON_OFF 6 --> Port H3

#define BUTTON_RESET 26 --> Port A4
#define BUTTON_ON_OFF 2 --> Port E4
#define BUTTON_RIGHT 9 --> Port H6
#define BUTTON_LEFT 10 --> Port B4

#define LED_PINR 32 --> Port C5
#define LED_PINY 30 --> Port C7
#define LED_PING 33 --> Port C4
#define LED_PINB 31 --> Port C6
*/

//Port A Register Pointers
volatile unsigned char* port_a = (unsigned char*) 0x22; 
volatile unsigned char* ddr_a  = (unsigned char*) 0x21; 
volatile unsigned char* pin_a  = (unsigned char*) 0x20; 

//Port C Register Pointers
volatile unsigned char* port_c = (unsigned char*) 0x28; 
volatile unsigned char* ddr_c  = (unsigned char*) 0x27; 
volatile unsigned char* pin_c  = (unsigned char*) 0x26; 

//Port E Register Pointers
volatile unsigned char* port_e = (unsigned char*) 0x2E; 
volatile unsigned char* ddr_e  = (unsigned char*) 0x2D; 
volatile unsigned char* pin_e  = (unsigned char*) 0x2C; 

//Port H Register Pointers
volatile unsigned char* port_h = (unsigned char*) 0x2B; 
volatile unsigned char* ddr_h  = (unsigned char*) 0x2A; 
volatile unsigned char* pin_h  = (unsigned char*) 0x29; 

//Port D Register Pointers
volatile unsigned char* port_d = (unsigned char*) 0x25; 
volatile unsigned char* ddr_d  = (unsigned char*) 0x24; 
volatile unsigned char* pin_d  = (unsigned char*) 0x23; 

enum SystemState {
  DISABLED,
  IDLE,
  ERROR_STATE,
  RUNNING
};

SystemState currentState = DISABLED;
SystemState previousState = DISABLED;
bool systemEnabled = false;

void setup() {
  Serial.begin(9600);

  *ddr_e &= ~(0x01 << 4); //pinMode(BUTTON_ON_OFF, INPUT);
  *ddr_a &= ~(0x01 << 4); //pinMode(BUTTON_RESET, INPUT);
  *ddr_h &= ~(0x01 << 6); //pinMode(BUTTON_LEFT, INPUT);
  *ddr_d &= ~(0x01 << 4); //pinMode(BUTTON_RIGHT, INPUT);
  
  *ddr_c |= (0x01 << 4); //pinMode(YELLOW, OUTPUT);
  *ddr_c |= (0x01 << 5); //pinMode(BLUE, OUTPUT);
  *ddr_c |= (0x01 << 6); //pinMode(RED, OUTPUT);
  *ddr_c |= (0x01 << 7); //pinMode(GREEN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), toggleSystem, HIGH);
}

void loop() {
  switch(currentState){
    case DISABLED:
      disabledState();
      break;
    case IDLE:
      idledState();
      break;
  }

  if (*pin_a & (0x01 << 4))){ //Statement to see if pin is high -- Do !(*pin_a & (0x01 << 4) for digitalRead(BUTTON_RESET) == LOW))
    *port_c &= ~(0x01 << 4);
    *port_c &= ~(0x01 << 5);
    *port_c &= ~(0x01 << 6);
    *port_c |= (0x01 << 7);
  } else{
    Serial.println("Low");
  }

  //This switch statement is just me testing out toggling in between the two states.
  
  /*
  !(*pin_a & (0x01 << 4) for digitalRead(BUTTON_RESET) == LOW);
  *pin_a & (0x01 << 4 for digitalRead(BUTTON_RESET) == HIGH);

  !(*pin_e & (0x01 << 4) for digitalRead(BUTTON_ON_OFF) == LOW);
  *pin_e & (0x01 << 4 for digitalRead(BUTTON_ON_OFF) == HIGH)

  !(*pin_h & (0x01 << 4) for digitalRead(BUTTON_RIGHT) == LOW);
  *pin_h & (0x01 << 4 for digitalRead(BUTTON_RIGHT) == HIGH)

    !(*pin_b & (0x01 << 4) for digitalRead(BUTTON_LEFT) == LOW);
  *pin_b & (0x01 << 4 for digitalRead(BUTTON_LEFT) == HIGH)
  */
}

void toggleSystem(){
  if (*pin_e & (0x01 << 4)) {
    systemEnabled = !systemEnabled; // Toggle system state
  }
}

void disabledState(){
    *port_c &= ~(0x01 << 4); //  digitalWrite(YELLOW, LOW);
    *port_c &= ~(0x01 << 5); //  digitalWrite(BLUE, LOW);
    *port_c &= ~(0x01 << 6); //  digitalWrite(RED, LOW);
    *port_c &= ~(0x01 << 7); //  digitalWrite(GREEN, LOW);

    if (systemEnabled) {
    currentState = IDLE;
    }
}

void idledState(){
    *port_c |= (0x01 << 4); //  digitalWrite(YELLOW, HIGH);
    *port_c |= (0x01 << 5); //  digitalWrite(BLUE, HIGH);
    *port_c |= (0x01 << 6); //  digitalWrite(RED, HIGH);
    *port_c |= (0x01 << 7); //  digitalWrite(GREEN, HIGH);
    if (!systemEnabled) {
    currentState = DISABLED;
    }
}
