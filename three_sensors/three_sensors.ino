const int FRONT_TRIG_PIN = 6;
const int FRONT_ECHO_PIN = 7;
const int LEFT_TRIG_PIN = 9;//
const int LEFT_ECHO_PIN = 10;//
const int RIGHT_TRIG_PIN = 12;//
const int RIGHT_ECHO_PIN = 11;//

int trig_pins[3] = {FRONT_TRIG_PIN, LEFT_TRIG_PIN, RIGHT_TRIG_PIN};
int echo_pins[3] = {FRONT_ECHO_PIN, LEFT_ECHO_PIN, RIGHT_ECHO_PIN};
char lib_pins[3][6] = {"Front", "Right", "Left"};
int trig_pin, echo_pin;
float duration, distance;

void setup() {
  // put your setup code here, to run once:
  for (int i=0; i<3; i++) {
    pinMode(trig_pins[i], OUTPUT);
    pinMode(echo_pins[i], INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
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
      Serial.print("Distance sensor ");
      Serial.print(lib_pins[i]);
      Serial.print(" :");

      Serial.println(distance);
    } else {
      Serial.print("Distance sensor ");
      Serial.print(lib_pins[i]);
      Serial.print(" : No echo received");
    }
  }

  delay(1000);
}
