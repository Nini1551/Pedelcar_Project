#include <Servo.h>
#define TIMEOUT_DURATION 2000 // Timeout duration in milliseconds

const int FRONT_TRIG_PIN = 11;
const int FRONT_ECHO_PIN = 10;
const int LEFT_TRIG_PIN = 8;
const int LEFT_ECHO_PIN = 9;
const int RIGHT_TRIG_PIN = 6;
const int RIGHT_ECHO_PIN = 7;
const int BACK_TRIG_PIN = 39;
const int BACK_ECHO_PIN = 38;

const int FORWARD_LED_PIN = 12;
// const int BACKWARD_LED_PIN = 30;
// const int LED_PIN = 12;

const int SERVO_PIN = 3;
// const int BUTTON_PIN = 13;
const int MOTOR_PIN = DAC1;
const int BACKWARD_PIN = 4;

const int ANGLE_MAX = 180;
const int ANGLE_MIN = 0;
const int ANGLE_FORWARD = 90;

const float CRITICAL = 4;
const float AVOIDING = 15;
const float MAX_RANGE = 400;

const int REFRESH_TIME = 50;
const int MOTOR_DELAY = 500;

const float SOUND_SPEED = 0.0343;

Servo servo;

int trig_pins[4] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN, BACK_TRIG_PIN};
int echo_pins[4] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN, BACK_ECHO_PIN};
char lib_pins[4][6] = {"Front", "Right", "Left", "Back"};

int trig_pin, echo_pin;
float duration, distance;
float distances[4];
int avoid;
int throttle = 0;
int max_throttle = 255;

char receivedChar;
unsigned long lastReceiveTime = 0;
bool disconnectedPrinted = false;

void setup() {
  pinMode(FORWARD_LED_PIN, OUTPUT);
  // pinMode(BACKWARD_LED_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  digitalWrite(BACKWARD_PIN, HIGH); // to go forward, backward pin need to set high
  // pinMode(BUTTON_PIN, INPUT);
  for (int i=0; i<4; i++) {
    pinMode(trig_pins[i], OUTPUT);
    pinMode(echo_pins[i], INPUT);
  }
  servo.attach(SERVO_PIN);

  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  if (Serial1.available()) {
    // Update last receive time
    // lastReceiveTime = millis();
    // Reset the disconnection flag
    // disconnectedPrinted = false;

    receivedChar = Serial1.read();
    Serial.println(receivedChar);
    if(receivedChar == '1') { 

      set_measures();
      if (is_in_danger()) {
        Serial.println("Danger");
        go_backward();
      } else {
        Serial.println("Forward");
        go_forward();
      }
    } else if (receivedChar == '0') {
      turn_off_backward_motor();
      go_straight();
    }
    delay(REFRESH_TIME);

  } /*else {
    // Check for timeout
    if (millis() - lastReceiveTime > TIMEOUT_DURATION && !disconnectedPrinted) {
      Serial.println("Bluethoot Disconnected");
      disconnectedPrinted = true;
    }
  }*/
  serial_flush();
}

void set_measures() { // Update distances detect by sensors
  for (int i=0; i<4; i++) {
    
    trig_pin = trig_pins[i];
    echo_pin = echo_pins[i];

    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);

    duration = pulseIn(echo_pin, HIGH);
    if (duration > 0){
      distance = cos(PI/6) * (duration * SOUND_SPEED) / 2;
      distances[i] = distance;
    } else {
      distances[i] = MAX_RANGE;
    }
    set_max_speed();
  }
}

void go_right() { // Guide the wheel on the right direction
  servo.write(ANGLE_MIN);
}

void go_left() { // Guide the wheel on the left direction
  servo.write(ANGLE_MAX);
}

void go_straight() { // Guide the wheel on straight
	servo.write(ANGLE_FORWARD);
}

bool is_in_danger() { // If there is a obstacle near
  if (distances[0] < CRITICAL or distances[1] < CRITICAL or distances[2] < CRITICAL) {
  	return true;	
  } 
  return false;
}

int need_avoiding() { // Return the index of the direction needed (0: straight, 1: left, 2: right)
  float front_distance = distances[0];
  float left_distance = distances[1]; 
  float right_distance = distances[2]; 
  
  if ((left_distance < AVOIDING and right_distance < AVOIDING) or front_distance < AVOIDING) {
    if (left_distance < right_distance) {
      return 1;
    } else {
      return 2;
    }
  }
  if (left_distance < AVOIDING) {
    return 1;
  } 
  if (right_distance < AVOIDING) {
  	return 2;	
  } 
  return 0;
}

void go_backward() { // Start the reverse launch  of the car
  turn_off_motor();
  go_straight();
  turn_on_backward_motor();
  while (need_avoiding() and is_button_active() and distances[4] >= CRITICAL) {
    if (Serial1.available()){
      receivedChar = Serial1.read();
      if (receivedChar == '\n') {
        receivedChar = '1';
      }
      set_measures(); 
      delay(REFRESH_TIME);
      serial_flush();
    }
  }
  turn_off_backward_motor();
}

void go_forward() { // start the forward travel of the car
  turn_on_motor();
  set_measures();
  avoid = need_avoiding();

  if(avoid == 1){
    go_right();
  } else if(avoid == 2) {
    go_left();
  } else if (avoid == 0) {
    go_straight();
  }
}

void turn_on_motor() { // start the motor
  digitalWrite(FORWARD_LED_PIN, HIGH);
  
  while (throttle < 245) {
  	ramping_throttle();
  }
  if (throttle > max_throttle) {
    throttle = max_throttle;
    analogWrite(MOTOR_PIN, throttle);
  }
}

void turn_off_motor() { // stop the motor
  digitalWrite(FORWARD_LED_PIN, LOW);
  
  throttle = 0;
  analogWrite(MOTOR_PIN,0);
  delay(MOTOR_DELAY);
}

void turn_on_backward_motor() { // start the motor in reverse
  // digitalWrite(BACKWARD_LED_PIN, HIGH);

  turn_off_motor();
  digitalWrite(BACKWARD_PIN,LOW);
  delay(MOTOR_DELAY);
  turn_on_motor();
}

void turn_off_backward_motor() { // stop the motor in reverse
  // digitalWrite(BACKWARD_LED_PIN, LOW);
  
  turn_off_motor();
  digitalWrite(BACKWARD_PIN, HIGH);
  delay(MOTOR_DELAY);
}

void ramping_throttle() { // increment the throttle
  throttle += 5;
  if(throttle > 245) {
    throttle = 245;
  }
  analogWrite(MOTOR_PIN,throttle);
  delay(1);
  Serial.println(throttle);
}

bool is_button_active() { // Vrai lorsque le signal est activé
  return receivedChar == '1';
}

void serial_flush() {
  while (Serial1.available() > 0) {
    char t = Serial1.read();
  }
}

void set_max_speed(){
  if (distances[0] < distances[1] and distances[0] < distances[2]){
    max_throttle = int(distances[0]/1.6);
  } else if (distances[1] < distances[0] and distances[1] < distances[2]){
    max_throttle = int(distances[1]/1.6);
  } else if (distances[2] < distances[0] and distances[2] < distances[1]){
    max_throttle = int(distances[2]/1.6);
  }
}