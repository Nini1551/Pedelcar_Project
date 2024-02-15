#include <Servo.h>
#define TIMEOUT_DURATION 2000 // Timeout duration in milliseconds

const int FRONT_TRIG_PIN = 5;
const int FRONT_ECHO_PIN = 4;
const int LEFT_TRIG_PIN = 3;
const int LEFT_ECHO_PIN = 2;
const int RIGHT_TRIG_PIN = 7;
const int RIGHT_ECHO_PIN = 6;
const int FORWARD_LED_PIN = 8;
const int BACKWARD_LED_PIN = 12;
const int SERVO_PIN = 9;
// const int BUTTON_PIN = 13;
const int MOTOR_PIN = 10;
const int BACKWARD_PIN = 11;

const int ANGLE_MAX = 135;
const int ANGLE_MIN = 45;
const int ANGLE_FORWARD = 90;

const float CRITICAL = 40;
const float AVOIDING = 150;
const float MAX_RANGE = 400;

const int REFRESH_TIME = 50;

const float SOUND_SPEED = 0.0343;

Servo servo;

int trig_pins[3] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN};
int echo_pins[3] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN};
char lib_pins[3][6] = {"Front", "Right", "Left"};

int trig_pin, echo_pin;
float duration, distance;
float distances[3];
int avoid;
int throttle = 0;

char receivedChar;
unsigned long lastReceiveTime = 0;
bool disconnectedPrinted = false;

void setup() {
  pinMode(FORWARD_LED_PIN, OUTPUT);
  pinMode(BACKWARD_LED_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  digitalWrite(BACKWARD_PIN, HIGH); // to go forward, backward pin need to set high
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  for (int i=0; i<3; i++) {
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
    lastReceiveTime = millis();
    // Reset the disconnection flag
    disconnectedPrinted = false;

    receivedChar = Serial1.read();
    Serial.print(receivedChar);
    if (receivedChar != '\n') {
      if(is_button_active()) {
        set_measures();
        if (is_in_danger()) {
          go_backward();
        } else {
          go_forward();
        }
      } else if (receivedChar == '0') {
        turn_off_backward_motor();
      }
      delay(REFRESH_TIME);
    }
  } else {
    // Check for timeout
    if (millis() - lastReceiveTime > TIMEOUT_DURATION && !disconnectedPrinted) {
      Serial.println("Bluethoot Disconnected");
      disconnectedPrinted = true;
    }
  }
}

void set_measures() { // Update distances detect by sensors
  for (int i=0; i<3; i++) {
    
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
  }
}

void go_right() { // Guide the wheel on the right direction
  servo.write(ANGLE_MAX);
}

void go_left() { // Guide the wheel on the left direction
  servo.write(ANGLE_MIN);
}

void go_straight() { // Guide the wheel on straight
	servo.write(ANGLE_FORWARD);
}

bool is_in_danger() { // If there is a obstacle near
  if (distances[0] < CRITICAL) {
  	return true;	
  }
  if (distances[1] < CRITICAL or distances[2] < CRITICAL) {
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

void go_backward() { // Start the reverse launche of the car
  turn_off_forward_motor();
  go_straight();
  turn_on_backward_motor();
  while (need_avoiding() and is_button_active()) {
  	set_measures();
    delay(REFRESH_TIME);
  }
  turn_off_backward_motor();
}

void go_forward() { // start the for
  turn_on_forward_motor();
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

void turn_on_forward_motor() { // Demare la marche avant du moteur
  digitalWrite(FORWARD_LED_PIN, HIGH);
  
  while (throttle < 255) {
  	ramping_throttle();
  }
}

void turn_off_forward_motor() { // Arrete la marche avant du moteur
  digitalWrite(FORWARD_LED_PIN, LOW);
  
  analogWrite(MOTOR_PIN,0);
  delay(1000);
}

void turn_on_backward_motor() { // Demarre la marche arrière du moteur
  digitalWrite(BACKWARD_LED_PIN, HIGH);

  turn_off_forward_motor();
  digitalWrite(BACKWARD_PIN,LOW);
  delay(1000);
  turn_on_forward_motor();
}

void turn_off_backward_motor() { // Arrete la marche arrière du moteur
  digitalWrite(BACKWARD_LED_PIN, LOW);
  
  turn_off_forward_motor();
  digitalWrite(BACKWARD_PIN, HIGH);
  delay(1000);
}

void ramping_throttle() { // Active petit à petit le throttle
  throttle+=5;
  analogWrite(MOTOR_PIN,throttle);
  delay(1);
}

bool is_button_active() { // Vrai lorsque le signal est activé
  return receivedChar == '1';
}