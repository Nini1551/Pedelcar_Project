// SET THE DISTANCES BEFORE ANYTHING

const int FRONT_TRIG_PIN = 6;
const int FRONT_ECHO_PIN = 7;
const int LEFT_TRIG_PIN = 9;
const int LEFT_ECHO_PIN = 10;
const int RIGHT_TRIG_PIN = 12;
const int RIGHT_ECHO_PIN = 11;

const int LED_PIN = 31;

const float MIN_DISTANCE = 15;

int trig_pins[3] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN};
int echo_pins[3] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN};
char lib_pins[3][6] = {"Front", "Right", "Left"};
int trig_pin, echo_pin;
float duration, distance;
float distances[3] = {100, 100, 100};

char receivedChar;

bool in_danger = false;

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<3; i++) {
    pinMode(trig_pins[i], OUTPUT);
    pinMode(echo_pins[i], INPUT);
  }
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial1.available()) {
    receivedChar = Serial1.read();
    Serial.print(receivedChar);
    if (receivedChar != '\n') {
      // in_danger = (distances[0] < MIN_DISTANCE) || (distances[1] < MIN_DISTANCE) || (distances[2] < MIN_DISTANCE);
      if (receivedChar == '1' && !(in_danger)) {
        digitalWrite(LED_PIN, HIGH);
        print_distances();
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
}

float get_distance(float duration) {
  return (duration * 0.0343) / 2;
}


float get_duration(int index) {
    trig_pin = trig_pins[index];
    echo_pin = echo_pins[index];
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);
    return pulseIn(echo_pin, HIGH);
}

void print_distance(float duration, int index) {
  if (duration > 0){
    distances[index] = get_distance(duration);
    Serial.print("Distance sensor ");
    Serial.print(lib_pins[index]);
    Serial.print(" :");
    Serial.println(distances[index]);
  } else {
    Serial.print("Distance sensor ");
    Serial.print(lib_pins[index]);
    Serial.println(" : No echo received");
  }
}

void print_distances() {
  for (int i=0; i<3; i++) {
    duration = get_duration(i);
    print_distance(duration, i);
  }
  Serial.println();
}