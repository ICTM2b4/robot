#include <Wire.h>
// params for receiving data from old arduino
#define I2C_SLAVE_ADDRESS 11
#define PAYLOAD_SIZE 2

int sent = 0;
int receive = 0;
bool direction = false;
int Zspeed = 0;

// setup the motors
#define Z_MOTOR_DIRECTION 12
#define Z_MOTOR_SPEED 3

void setup()
{
    Serial.begin(9600);
    // setup motor
    pinMode(Z_MOTOR_DIRECTION, OUTPUT);
    pinMode(Z_MOTOR_SPEED, OUTPUT);
    // setup comunicatie I2C
    Wire.begin(I2C_SLAVE_ADDRESS);
    Serial.begin(9600);
    // setup functies voor I2C
    Wire.onRequest(requestEvents);
    Wire.onReceive(receiveEvents);
}

void loop()
{
    checkMessages();
}

/**
 *  check if there is a message in the serial buffer
 *  if there is a message, read it and execute the related command
 */
void checkMessages()
{
    if (Serial.available() <= 0)
        return;
    String ontvangen = Serial.readString();
    Serial.println(ontvangen);

    if (ontvangen == "Z+")
    {
        setMotorDirection(true);
        setMotorSpeed(255);
    }
    else if (ontvangen == "Z-")
    {
        setMotorDirection(false);
        setMotorSpeed(255);
    }

    else if (ontvangen == "Z0")
    {
        setMotorSpeed(0);
    }
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
    Serial.println(receive);
    if (receive == 1)
        setMotorDirection(true);
    if (receive == 2)
        setMotorDirection(false);
    if (receive == 3)
        setMotorSpeed(0);
    if (receive >= 4)
        setMotorSpeed(receive);
}

/**
 *  set the speed of the motor
 * @param speed 0 - 255
 */
void setMotorSpeed(int speed)
{
    analogWrite(Z_MOTOR_SPEED, speed);
}

/**
 *   set the direction of the right motor
 *   @param direction true = forward, false = backward
 */
void setMotorDirection(bool direction)
{
    if (direction)
    {
        digitalWrite(Z_MOTOR_DIRECTION, LOW);
        return;
    }
    digitalWrite(Z_MOTOR_DIRECTION, HIGH);
}