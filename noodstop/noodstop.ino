const int interruptPin = 2;
int resetNoodStop = 0;
bool noodstopstate = false;


void setup()
{
  // put your setup code here, to run once:
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), noodstop, FALLING);

  Serial.begin(9600);

}

void loop()
{

  // put your main code here, to run repeatedly:
  noodstoploop(); 
  delay(100);
  if(noodstopstate){
    Serial.println("noodstop aan");
  }
  else{
    Serial.println("noodstop uit");
  }
}



void noodstop()
{
  noodstopstate = true;
  Serial.println("noodstop knop ingedrukt");
detachInterrupt(digitalPinToInterrupt(interruptPin));


  
}
void noodstoploop()
{


  if (noodstopstate)
  {
    while (((resetNoodStop + 5000) > millis()) || resetNoodStop == 0)
    {
     Serial.println("noodstop aan");
      bool noodstopknop = !digitalRead(interruptPin);
      if (noodstopknop && resetNoodStop == 0)
      {
        resetNoodStop = millis();
      }
      else if (noodstopknop == false)
      {
        resetNoodStop = 0;
      }
    }
    noodstopstate = false;
    resetNoodStop = 0;
    while(!digitalRead(interruptPin)){}

    attachInterrupt(digitalPinToInterrupt(interruptPin), noodstop, FALLING);

  }
  
}
