const int LED_PIN = 31;

char receivedChar;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial1.available()) {
    receivedChar = Serial1.read();
    if (receivedChar != '\n') {
      Serial.println(receivedChar);
      if (receivedChar == '1') {
        digitalWrite(LED_PIN, HIGH);
      } else if (receivedChar == '0') {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }
}
