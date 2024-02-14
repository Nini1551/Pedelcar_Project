#include <Servo.h>
const int FRONT_TRIG_PIN = 4;
const int FRONT_ECHO_PIN = 5;
const int LEFT_TRIG_PIN = 2;
const int LEFT_ECHO_PIN = 3;
const int RIGHT_TRIG_PIN = 6;
const int RIGHT_ECHO_PIN = 7;
const int MOTOR_PIN_LED = 8;
const int SERVO_PIN = 9;
const int BUTTON_PIN = 10;

const int ANGLE_MAX = 160;
const int ANGLE_MIN = 20;
const int ANGLE_INCR = 10;

int trig_pins[3] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN};
int echo_pins[3] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN};
char lib_pins[3][6] = {"Front", "Right", "Left"};
char receivedChar;

int trig_pin, echo_pin;
float duration, distance;
float distances[3];

Servo servo;
int angle=70;

void setup() {
  
  pinMode(MOTOR_PIN_LED, OUTPUT);
  for (int i=0; i<3; i++) {
    pinMode(trig_pins[i], OUTPUT);
    pinMode(echo_pins[i], INPUT);
  }
  servo.attach(SERVO_PIN);
  Serial.begin(9600);
}

void loop() {
  measure();
  if (digitalRead(BUTTON_PIN)==1) {
    digitalWrite(MOTOR_PIN_LED, HIGH);
    measure();
    if(distances[1]>(distances[0]+distances[2])){
      right();
      Serial.println(angle);
    }
    if(distances[2]>(distances[0]+distances[1])){
      left();
      Serial.println(angle);
    }
  }
  if (digitalRead(BUTTON_PIN)==0){
    digitalWrite(MOTOR_PIN_LED, LOW); 
  }
  delay(50);
}

void measure() {
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
          distance = (duration * 0.0343) / 2;
          distances[i] = distance;
        }
   }
}

void measurePrint() {
  for (int i=0; i<3; i++) {
    Serial.print("Distance sensor ");
    Serial.print(lib_pins[i]);
    Serial.print(" :");
    Serial.print(distances[i]);
    Serial.println(" cm");
  }
}
void error() {
  for (int i=0; i<3; i++) {
    Serial.print("Distance sensor ");
    Serial.print(lib_pins[i]);
    Serial.println(" : No echo received");

  }  
}
void right() {
  if (angle<=ANGLE_MAX){
  	angle+=ANGLE_INCR;
  }
  servo.write(angle);
}
void left() {
  if (angle>=ANGLE_MIN){
  	angle-=ANGLE_INCR;
  }
  servo.write(angle);
}