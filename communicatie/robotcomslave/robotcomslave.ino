#include <Wire.h>

# define I2C_SLAVE_ADDRESS 11 // slave address

#define PAYLOAD_SIZE 2
int sent=0;
int receive=0;


void setup()
{
  Wire.begin(I2C_SLAVE_ADDRESS);
  Serial.begin(9600);        
  Wire.onRequest(requestEvents);
  Wire.onReceive(receiveEvents);
}

void loop(){
  
}



void requestEvents()
{

  Wire.write(sent);
}

void receiveEvents(int numBytes)
{  

  receive = Wire.read();

  Serial.println(receive);
  sent = receive + 1;
}