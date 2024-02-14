#include <Servo.h>

const int FRONT_TRIG_PIN = 5;
const int FRONT_ECHO_PIN = 4;
const int LEFT_TRIG_PIN = 3;
const int LEFT_ECHO_PIN = 2;
const int RIGHT_TRIG_PIN = 7;
const int RIGHT_ECHO_PIN = 6;
const int FORWARD_MOTOR_PIN = 8;
const int BACKWARD_MOTOR_PIN = 12;
const int SERVO_PIN = 9;
const int BUTTON_PIN = 13;

const int ANGLE_MAX = 135;
const int ANGLE_MIN = 45;
const int ANGLE_FORWARD = 90;

const float CRITICAL = 40;
const float AVOIDING = 150;
const float MAX_RANGE = 400;

const int REFRESH_TIME = 50;

const float SOUND_SPEED = 0.0343;

int trig_pins[3] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN};
int echo_pins[3] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN};
char lib_pins[3][6] = {"Front", "Right", "Left"};
char receivedChar;

int trig_pin, echo_pin;
float duration, distance;
float distances[3];
int avoid;

Servo servo;

void setup() {
  
  pinMode(FORWARD_MOTOR_PIN, OUTPUT);
  pinMode(BACKWARD_MOTOR_PIN, OUTPUT);
  for (int i=0; i<3; i++) {
    pinMode(trig_pins[i], OUTPUT);
    pinMode(echo_pins[i], INPUT);
  }
  servo.attach(SERVO_PIN);
  Serial.begin(9600);
}

void loop() {
  if(digitalRead(BUTTON_PIN)==1) {
    set_measures();
    if (is_in_danger()) {
      go_backward();
    } else {
      go_forward();
    }
  }
  else if (digitalRead(BUTTON_PIN)==0){
    turn_off_forward_motor(); 
  }
  delay(REFRESH_TIME);
}

void set_measures() {
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

void go_right() {
  servo.write(ANGLE_MAX);
}

void go_left() {
  servo.write(ANGLE_MIN);
}

void go_straight() {
	servo.write(ANGLE_FORWARD);
}

bool is_in_danger() {
  if (distances[0] < CRITICAL) {
  	return true;	
  }
  if (distances[1] < CRITICAL or distances[2] < CRITICAL) {
  	return true;	
  } 
  return false;
}

int need_avoiding() {
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

void go_backward() {
  turn_off_forward_motor();
  go_straight();
  turn_on_backward_motor();
  while (need_avoiding() and digitalRead(BUTTON_PIN)==1) {
  	set_measures();
    delay(REFRESH_TIME);
  }
  turn_off_backward_motor();
}

void go_forward() {
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

void turn_on_forward_motor() {
  digitalWrite(FORWARD_MOTOR_PIN, HIGH);
}

void turn_off_forward_motor() {
  digitalWrite(FORWARD_MOTOR_PIN, LOW);
}

void turn_on_backward_motor() {
  digitalWrite(BACKWARD_MOTOR_PIN, HIGH);
}

void turn_off_backward_motor() {
  digitalWrite(BACKWARD_MOTOR_PIN, LOW);
}