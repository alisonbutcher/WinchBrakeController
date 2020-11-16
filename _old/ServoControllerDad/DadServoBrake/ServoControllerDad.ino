#include <Servo.h>

Servo servo;                     // Initialise the servo library Object

// Constants (fixed variables)
const int FOOTSWITCH = 2;        // Normally closed footswitch input on pin 2
const int START_POT = A0;        // start_pot (potentiometer) is on analog input 0
const int END_POT = A1;          // end_pot (potentiometer) is on analog input 1
const int TRAVEL_DELAY = 0.65;   // Time taken for servo to travel from each point (Consider replacing this delay constant with a potentiometer)
const int PWM_OUTPUT_PIN = 9;    // number of output pin for servo control (eg: pwm)

// Setup function runs once at power on
void setup() {
  pinMode(FOOTSWITCH, INPUT);        // initialise footswitch as input
  servo.attach(PWM_OUTPUT_PIN);      // define output pin for PWM to servo
}


// Main program Loop
void loop() {
  int startd = 0;                  // scaled servo start point in degrees (scaled from potentionmeter)
  int endd = 180;                  // scaled servo end point in degrees (scaled from potentionmeter)
  int fs = 0;                      // footswitch state
  int fs_prev = 0;                 // footswitch state at last check
  boolean brake_state = true;      // TRUE when brake is engaged

  
  startd = getPoint(START_POT);       // aquire start point from potentiometer
  endd = getPoint(END_POT);           // aquire end point from potentiometer

  delay(50);                          // delay to ignore bounce on switch contacts
  fs = digitalRead(FOOTSWITCH);

  // Did footswitch just change state
  if(fs != fs_prev){
    //Yes
    if(fs == HIGH) {                 // was it from LOW to HIGH  
      // Activate Braking
      servo.write(endd);
      brake_state = true;
    } else if(fs == LOW) {           // was it from HIGH to LOW                 
      // Deactive Braking (if we are here it means the motor has started)
      servo.write(startd);
      brake_state = false;
    }
  }
  fs_prev = fs;                       // update prev footswitch state with actual current footswitch state

  // TODO: Add delay to release brake again after a short time period from when it has been activated

}



// Given a reference p to an analog input this function returns a scaled value in degrees of half circle of travel
int getPoint(int p) {
  
  int q = analogRead(p);              // read raw position of Potentiometer
  q = map(q, 0, 1023, 0, 179);        // Scale position to degrees
  return q;
}
