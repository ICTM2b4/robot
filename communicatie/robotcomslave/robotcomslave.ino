#include <Wire.h>

# define I2C_SLAVE_ADDRESS 11 // slave address

#define PAYLOAD_SIZE 2
int sent=0;
int receive=0;


void setup()
{
  // setup comunicatie I2C
  Wire.begin(I2C_SLAVE_ADDRESS);
  Serial.begin(9600);        
  // setup functies voor I2C
  Wire.onRequest(requestEvents);
  Wire.onReceive(receiveEvents);
}

void loop(){
  
}


// functie voor het versturen van data
void requestEvents()
{

  Wire.write(sent);
}

// functie voor het ontvangen van data
void receiveEvents(int numBytes)
{  

  receive = Wire.read();

  Serial.println(receive);
  sent = receive + 1;
}