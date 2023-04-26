void setup() {
    Serial.begin(9600);
    pinMode(A2, INPUT);
   pinMode(A3, INPUT);
}

void loop() {
    Serial.println(analogRead(A2));
    Serial.println(analogRead(A3));
    delay(1000);
}