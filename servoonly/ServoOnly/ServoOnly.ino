/* 
 *  Servo brake controller for RC winch
 *  When Winch footswitch is released servo is extended to brake position, 
 *  pauses for a dwell time (braking) before returning to retracted position
 *  
 *  Author: Alison Butcher
 *  Version: 1.1
 *  Date: 16 November 2020
 */


#include <Servo.h>
Servo servo;   

// IO Addresses
#define FOOTSWITCH_IN     2             // Address of Footswitch input (note: active low)
#define PWM_OUTPUT_PIN    9             // Address of PWM output pin for servo control
#define MAX_POT           A2            // Address of potentiometer controlling max position
#define MIN_POT           A1            // Address of potentiometer controller min position
#define DWELL_POT         A0            // Address of potentiometer controlling dwell time of brake

// Constants
#define DWELL_MIN         300           // Min Dwell time in milliseconds
#define DWELL_MAX         10000         // Max Dwell time in milliseconds
#define DBOUNCE           100           // Debounce time in milliseconds
#define SERVO_MAX         130           // Servo position for Braking
#define SERVO_MIN         50            // Servo position for Retracting 

// Globals
int max_t = SERVO_MAX;                  // Servo position for Braking
int min_t = SERVO_MIN;                  // Servo position for Retracting
unsigned long dwell = DWELL_MAX;        // Braking Dwell time before retracting in milliseconds
unsigned long prev_ms = 0;              // Used by Dwell timing
bool footswitch = false;                // current state of footswitch
bool prev_footswitch = false;           // previous state of footswitch
bool brake_on;                          // true when brake is activated

/*
 * Setup runs once at power on
 */
void setup() {
  servo.attach(PWM_OUTPUT_PIN);               // define output pin for PWM to servo
  pinMode(FOOTSWITCH_IN, INPUT_PULLUP);       // define input pin for footswitch
  pinMode(LED_BUILTIN, OUTPUT);               // define output 'brake on' indicator led
  brakeOff();                                 // always start with brake disengaged
}

/*
 * Main Program
 */
void loop() {

  // Get potentionmeter values
  dwell = getDwell();
  min_t = getPoint(MIN_POT);
  max_t = getPoint(MAX_POT);
  
  // Get debounced footswitch state
  fsDebounce();

  unsigned long current_ms = millis();

  // Brake Control
  if (!footswitch) {                      // Any time footswitch is activated brake turns off
    brakeOff();
  } else {                                // not footswitch
    if (!prev_footswitch) {
      brakeOn();                          // brake on is triggered on rising edge only
      prev_ms = current_ms;               // get start time for dwell 
    }
  }
  prev_footswitch = footswitch;

  // Brake off after dwell
  if (brake_on && (current_ms - prev_ms) > dwell) {
    brakeOff();
  }

}

/*
 * Handles debouncing of footswitch input
 */
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

/*
 * Moves brake to engaged (braking) position
 */
void brakeOn() {
  servo.write(max_t);
  digitalWrite(LED_BUILTIN, HIGH);
  brake_on = true;
}

/*
 * Moves brake to disengaged (release) position
 */
void brakeOff() {
  servo.write(min_t);
  digitalWrite(LED_BUILTIN, LOW);
  brake_on = false;
}


/*
 * Given reference p to an analog input returns a scaled value for servo travel (min or max)
 */
int getPoint(int p) {
  int q = analogRead(p);                            // read raw position of Potentiometer
  q = map(q, 0, 1023, SERVO_MIN, SERVO_MAX);        // Scale position to maximum servo travel
  return q;
}

/*
 * Reads dwell from dwell potentiometer and scales value
 */
int getDwell() {
  int q = analogRead(DWELL_POT);                                    // aquire raw dwell from potentiometer
  unsigned long dwell = map(q, 0, 1023, DWELL_MIN, DWELL_MAX);      // scaled dwell value 
  return dwell;
}
