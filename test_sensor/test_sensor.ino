const int FRONT_TRIG_PIN = 12 ;
const int FRONT_ECHO_PIN = 11 ;

float durationFront, distanceFront;

void setup() {
  // put your setup code here, to run once:
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(FRONT_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG_PIN, LOW);
  durationFront = pulseIn(FRONT_ECHO_PIN, HIGH);
  if (durationFront > 0) {
    distanceFront = (durationFront * 0.0343) / 2;
    Serial.print("Distance sensor Front: ");
    Serial.println(distanceFront);
  } else {
    Serial.println("Distance sensor Front: No echo received"); 
  }
  delay(1000);
}
