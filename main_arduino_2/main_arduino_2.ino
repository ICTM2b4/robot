#include <Wire.h>
// params for receiving data from old arduino
#define I2C_SLAVE_ADDRESS 11
#define PAYLOAD_SIZE 2

int sent = 0;
int receive = 0;

// setup the motors
#define Z_MOTOR_DIRECTION 12
#define Z_MOTOR_SPEED 3
#define Z_MOTOR_POSITION_PIN 2
bool zMotorDirection;
int motorPosition = 0;
bool showedZero;
void setup()
{
    Serial.begin(9600);
    // setup motor
    pinMode(Z_MOTOR_DIRECTION, OUTPUT);
    pinMode(Z_MOTOR_SPEED, OUTPUT);
    pinMode(Z_MOTOR_POSITION_PIN, INPUT_PULLUP);
    // setup interrupt for motor position sensor (z-as)
    attachInterrupt(digitalPinToInterrupt(Z_MOTOR_POSITION_PIN), checkMotorPosition, RISING);
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
    Serial.println(motorPosition);
}

/**
 *  Check the position of the motor and update the motorPosition variable
 *  keep the motorPosition variable between 0 and 920 for accuracy
 */
void checkMotorPosition()
{
    if (motorPosition == 0 && !zMotorDirection || motorPosition == 920 && zMotorDirection)
        return;
    motorPosition = zMotorDirection ? motorPosition + 1 : motorPosition - 1;
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
    if (speed > 100)
        speed = 100;
    analogWrite(Z_MOTOR_SPEED, speed);
}

/**
 *   set the direction of the right motor
 *   @param direction true = forward, false = backward
 */
void setMotorDirection(bool direction)
{
    zMotorDirection = direction;
    if (direction)
    {
        digitalWrite(Z_MOTOR_DIRECTION, LOW);
        return;
    }
    digitalWrite(Z_MOTOR_DIRECTION, HIGH);
}