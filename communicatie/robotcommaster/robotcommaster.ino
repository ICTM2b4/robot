#include <Wire.h>
# define I2C_SLAVE1_ADDRESS 11 // slava adress
#define PAYLOAD_SIZE 2

int sent=1;
int recieve=0;

void setup()
{
  // setup comunicatie I2C
  Wire.begin();        
  Serial.begin(9600);  

}
/**
*functie voor het ontvangen van data
*/
void readData()
{
  //Request value from slave
  Wire.requestFrom(I2C_SLAVE1_ADDRESS, 1);   
  recieve = Wire.read(); 
  Serial.println(recieve);
  sent = recieve + 1;
}
/**
*functie voor het versturen van data
*/
void sentData(){
   
  //Send value to slave
  Wire.beginTransmission(I2C_SLAVE1_ADDRESS); 
  Wire.write(sent); 
  Wire.endTransmission();   
 
}

void loop()
{
//   delay(500);
//   sentData();
// delay(500);
// readData();
  int valueToSend = 5000;
  
  // Send the value to the slave Arduino
  Wire.beginTransmission(8);  // Address of the slave Arduino
  Wire.write((byte*)&valueToSend, sizeof(valueToSend));
  Wire.endTransmission();
  
  delay(500);  // Wait for the slave Arduino to process the data
  
  // Request the value back from the slave Arduino
  Wire.requestFrom(8, sizeof(valueToSend));
  int receivedValue = 0;
  Wire.readBytes((byte*)&receivedValue, sizeof(receivedValue));
  
  Serial.println(receivedValue);  // Print the received value
  
  delay(1000);
 
}
