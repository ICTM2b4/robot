#include <Wire.h>

# define I2C_SLAVE1_ADDRESS 11


#define PAYLOAD_SIZE 2

int sent=1;
int recieve=0;

void setup()
{
  // setup comunicatie I2C
  Wire.begin();        
  Serial.begin(9600);  

}
void readData()
{
  //Request value of n to slave
  Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1);   
  recieve = Wire.read(); 
  Serial.println(recieve);
  sent = recieve + 1;
}
void sentData(){
   
  //Send value to slave
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
  Wire.write(sent); 
  Wire.endTransmission();   
 
}

void loop()
{
  delay(500);
  sentData();
delay(500);
readData();

 
}