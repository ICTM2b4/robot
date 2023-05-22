#include <Wire.h>

#define I2C_SLAVE_ADDRESS 11 // slave address

#define PAYLOAD_SIZE 2
int sent = 0;
int receive = 0;

// void setup()
// {
//   // setup comunicatie I2C
//   Wire.begin(I2C_SLAVE_ADDRESS);
//   Serial.begin(9600);
//   // setup functies voor I2C
//   Wire.onRequest(requestEvents);
//   Wire.onReceive(receiveEvents);
// }

void setup()
{
  Wire.begin(8);                // Join the I2C bus as a slave with address 8
  Wire.onRequest(sendValue);    // Register the sendValue function to respond to requests
  Wire.onReceive(receiveValue); // Register the receiveValue function to handle incoming data

  Serial.begin(9600);
}

void loop()
{
  delay(100);
}

void sendValue()
{
  int valueToSend = 5000;
  Wire.write((byte *)&valueToSend, sizeof(valueToSend));
}

void receiveValue(int byteCount)
{
  int receivedValue = 0;

  if (byteCount == sizeof(receivedValue))
  {
    Wire.readBytes((byte *)&receivedValue, sizeof(receivedValue));
    // Process the received value, if needed

    // Send the value back to the master Arduino
    Wire.write((byte *)&receivedValue, sizeof(receivedValue));
  }
}

// void loop(){

// }

// /**
// *functie voor het versturen van data
// */
// void requestEvents()
// {

//   Wire.write(sent);
// }

// /**
// *functie voor het ontvangen van data
// */
// void receiveEvents(int numBytes)
// {

//   receive = Wire.read();

//   Serial.println(receive);
//   sent = receive + 1;
// }