#include <Servo.h>
Servo servo;   

// IO Addresses
#define FOOTSWITCH_IN     2             // Address of Footswitch input
#define PWM_OUTPUT_PIN    9             // Address of PWM output pin for servo control
#define MAX_POT           A2            // Address of potentiometer controlling max position
#define MIN_POT           A1            // Address of potentiometer controller min position
#define DWELL_POT         A0            // Address of potentiometer controlling dwell time of brake

// Constants
#define DWELL_MIN         300           // Min Dwell time in milliseconds
#define DWELL_MAX         10000         // Max Dwell time in milliseconds
#define DBOUNCE           100           // Debounce time in milliseconds
#define SERVO_MAX         135           // Servo position for Braking
#define SERVO_MIN         45            // Servo position for Retracting 

// Globals
int max_t = SERVO_MAX;                  // Servo position for Braking
int min_t = SERVO_MIN;                  // Servo position for Retracting
unsigned long dwell = DWELL_MAX;        // Braking Dwell time before retracting in milliseconds
bool dwell_timing = false;
unsigned long prevMillis = 0;           // Used by Dwell timing
bool footswitch = false;

// Setup function runs once at power on
void setup() {
  servo.attach(PWM_OUTPUT_PIN);               // define output pin for PWM to servo
  pinMode(FOOTSWITCH_IN, INPUT_PULLUP);       // define input pin for footswitch
  pinMode(LED_BUILTIN, OUTPUT);               // define output 'brake on' indicator led
  servo.write(SERVO_MIN);                     // Release brake when power on
}

// Main program Loop
void loop() {

  // Get potentionmeter values
  dwell = getDwell();
  min_t = getPoint(MIN_POT);
  max_t = getPoint(MAX_POT);
  

  // Get debounced footswitch state
  fsDebounce();

  // brake control
  if (footswitch == false) {
    brakeOn();
    // TODO: trigger dwell time
  } else {
    brakeOff();
  }

  // Dwell timer
  unsigned long currentMillis = millis();
  if ((currentMillis - prevMillis) > dwell) {
    prevMillis = currentMillis;
    
  }

}

// Handles Debouncing of Footswitch
void fsDebounce() {
  
  static unsigned long prevDBTime = 0;                            // Previous debounce time (at last check)
  static int prevFootState = LOW;                                 // Previous (debounced/valid) footswitch status
  static int prevBounceState = LOW;                               // Previous RAW bounce state of footswitch input

  int footState = digitalRead(FOOTSWITCH_IN);                     // read footswitch state

  // Footswitch detect contact bounce
  if (footState != prevBounceState) {
    prevDBTime = millis();        // reset debounce timing
    prevBounceState = footState;
  }

  // Footswitch debounced state change
  if ((millis() - prevDBTime) > DBOUNCE) {
    // If here then its a debounced and valid state
    
    if (prevFootState == HIGH && footState == LOW) {
      // footswitch is off
      footswitch = false;
    } else if (prevFootState == LOW && footState == HIGH) {
      // footswitch is on
      footswitch = true;
    }
    prevFootState = footState;
  }
}

void brakeOn() {
  servo.write(max_t);
  digitalWrite(LED_BUILTIN, HIGH);
}

void brakeOff() {
  servo.write(min_t);
  digitalWrite(LED_BUILTIN, LOW);
}



// Given a reference p to an analog input this function returns a scaled value to servo travel
int getPoint(int p) {
  int q = analogRead(p);                            // read raw position of Potentiometer
  q = map(q, 0, 1023, SERVO_MIN, SERVO_MAX);        // Scale position to maximum servo travel
  return q;
}

// Reads dwell from Dwell Potentiometer and scales value
int getDwell() {
  int q = analogRead(DWELL_POT);                                    // aquire raw dwell from potentiometer
  unsigned long dwell = map(q, 0, 1023, DWELL_MIN, DWELL_MAX);      // scaled dwell value 
  return dwell;
}
