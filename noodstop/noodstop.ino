const int interruptPin = 2;
bool emergencyButtonstate = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), emergencyButton, FALLING);

  Serial.begin(9600);
}

void loop() {
  if (emergencyButtonstate) {
    emergencyButtonloop();
    return;
  }
  Serial.println("emergencyButton niet actief");
}
/**
 * emergencyButton interupt function
*/
void emergencyButton() {
  emergencyButtonstate = true;
  Serial.println("emergencyButton knop ingedrukt");
}
/**
 *emergencyButton reset function
  */ 
void emergencyButtonloop() {


  String serialInput;
  if (Serial.available() > 0) {
    serialInput = Serial.readString();
  }
  if (serialInput == "reset") {
    Serial.println("emergencyButton gereset");
    emergencyButtonstate = false;
  }

  Serial.println("emergencyButton actief");
}
