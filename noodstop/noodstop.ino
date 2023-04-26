const int interruptPin = 2;
bool noodstopstate = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), noodstop, FALLING);

  Serial.begin(9600);
}

void loop() {
  if (noodstopstate) {
    noodstoploop();
    return;
  }
  Serial.println("noodstop niet actief");
}
/**
 * noodstop interupt function
*/
void noodstop() {
  noodstopstate = true;
  Serial.println("noodstop knop ingedrukt");
}
/**
 *noodstop reset function
  */ 
void noodstoploop() {


  String serialInput;
  if (Serial.available() > 0) {
    serialInput = Serial.readString();
  }
  if (serialInput == "reset") {
    Serial.println("noodstop gereset");
    noodstopstate = false;
  }

  Serial.println("noodstop actief");
}
