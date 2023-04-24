#include <Wire.h>

# define I2C_SLAVE_ADDRESS 11 // slave address

#define PAYLOAD_SIZE 2
int sent=0;
int receive=0;
bool direction = false;
int Zspeed = 0;


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
  Serial.print("speed: ");
  Serial.print(Zspeed);
  Serial.print( "  richting; ");
  Serial.println( direction);
 
  
}



/**
*functie voor het versturen van data
*/
void requestEvents()
{

  Wire.write(sent);
}


/**
*functie voor het ontvangen van data
*/
void receiveEvents(int numBytes)
{  

  receive = Wire.read();

  if (receive == 0)
  {
    Zspeed = 0;
  }
  //vooruit
  else if (receive < 100)
  {
    Zspeed = map(receive, 1, 100, 10, 254);
    direction = true;
  }
  // achteruit
  else if (receive >101) {
    Zspeed = map(receive, 101, 200, 10, 254);
    direction = false;
  }
}
