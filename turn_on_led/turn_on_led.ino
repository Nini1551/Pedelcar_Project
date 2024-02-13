const int LED = 31 ;
char receivedChar;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial1.available()) {
    receivedChar = Serial1.read();
    Serial.print("Received : ");
    Serial.println(receivedChar);

    if (receivedChar == '1'){
      digitalWrite(LED, HIGH);
    } else if (receivedChar == '0') {
      digitalWrite(LED, LOW);
    }
  }

}
