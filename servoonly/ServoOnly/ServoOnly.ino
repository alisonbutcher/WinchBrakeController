#include <Servo.h>
Servo servo;                  

// IO Addresses
const int DWELL_POT = A0;               // Address of potentiometer controlling dwell time of brake
const int MIN_POT = A1;                 // Address of potentiometer controller min position
const int MAX_POT = A2;                 // Address of potentiometer controlling max position
const int PWM_OUTPUT_PIN = 9;           // Address of PWM output pin for servo control

// Constants
const int SERVO_MAX = 80;               // Max travel position of servo (adjust for each specific servo)
const int SERVO_MIN = 24;               // Min travel position of servo (adjust for each specific servo)
const unsigned long DWELL_MIN = 300;    // Min Dwell time in milliseconds
const unsigned long DWELL_MAX = 10000;  // Max Dwell time in milliseconds
const unsigned long DBOUNCE = 200;      // Debounce time in milliseconds

// Globals
int max_t = SERVO_MAX;                  // Servo position for Braking
int min_t = SERVO_MIN;                  // Servo position for Retracting
unsigned long dwell = DWELL_MAX;        // Braking Dwell time before retracting
bool braking = false;
bool servo_on = false;
bool dwell_timing = false;
unsigned long previousMillis = 0;       // Used by Dwell timing

// Setup function runs once at power on
void setup() {
  servo.attach(PWM_OUTPUT_PIN);               // define output pin for PWM to servo
  attachInterrupt(0, winchOnISR, RISING);     // define interrupt for winch on
  attachInterrupt(1, winchOffISR, FALLING);   // define interrupt for winch off
  pinMode(LED_BUILTIN, OUTPUT);               // brake on indicator led
}


// Main program Loop
void loop() {

  // Get potentionmeter values
  dwell = getDwell();
  min_t = getPoint(MIN_POT);
  max_t = getPoint(MAX_POT);

  unsigned long currentMillis = millis();

  // brake off conditions
  if (servo_on) {
    
    // winch started
    if (!braking) brakeOff();

    // dwell timer
    if (currentMillis - previousMillis >= dwell) {
      brakeOff();
    }
    previousMillis = currentMillis;
  }

  // brake on conditions
  if (!servo_on && braking) {
    servo_on = true;
    digitalWrite(LED_BUILTIN, HIGH);
    servo.write(max_t);
  }

}

void brakeOff() {
  servo_on = false;
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


void winchOnISR()
{
 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 if (interrupt_time - last_interrupt_time > DBOUNCE)
 {
   braking = false;
 }
 last_interrupt_time = interrupt_time;
}

void winchOffISR() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DBOUNCE) {
    braking = true;
  }
   last_interrupt_time = interrupt_time;
}
