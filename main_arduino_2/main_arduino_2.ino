#include <Wire.h>
// params for receiving data from old arduino
#define I2C_SLAVE_ADDRESS 11
#define PAYLOAD_SIZE 2

int sent = 0;
int receive = 0;

// setup the motors
#define Z_MOTOR_DIRECTION 13
#define Z_MOTOR_SPEED 11
#define X_MOTOR_POSITION_PIN 3
#define Y_MOTOR_POSITION_PIN 2
bool zMotorDirection;
bool xMotorDirection;
bool yMotorDirection;
int xMotorPosistion = 0;
int yMotorPosistion = 0;
bool requestXMotorPosistion = false;
int maxXMotorPosistions[] = {0, 4931};
int maxYMotorPosistions[] = {0, 3000};
void setup()
{
    Serial.begin(9600);
    // setup motor
    pinMode(Z_MOTOR_DIRECTION, OUTPUT);
    pinMode(Z_MOTOR_SPEED, OUTPUT);
    pinMode(Y_MOTOR_POSITION_PIN, INPUT_PULLUP);
    // setup interrupt for motor position sensors (X and Z axis)
    attachInterrupt(digitalPinToInterrupt(Y_MOTOR_POSITION_PIN), setYMotorPosistion, RISING);
    attachInterrupt(digitalPinToInterrupt(X_MOTOR_POSITION_PIN), setXMotorPosistion, RISING);
    // setup comunicatie I2C
    Wire.begin(I2C_SLAVE_ADDRESS);
    Serial.begin(9600);
    // setup functies voor I2C
    Wire.onRequest(requestEvents);
    Wire.onReceive(receiveEvents);
}

void loop()
{
    fixMotorPositions();
    Serial.println("current: " + String(xMotorPosistion) + " " + String(yMotorPosistion) + "");
}

/**
 * fix the motor positions if they are out of bounds
 */
void fixMotorPositions()
{
    if (xMotorPosistion < maxXMotorPosistions[0])
        xMotorPosistion = 0;
    if (xMotorPosistion > maxXMotorPosistions[1])
        xMotorPosistion = maxXMotorPosistions[1];
    if (yMotorPosistion < maxYMotorPosistions[0])
        yMotorPosistion = 0;
    if (yMotorPosistion > maxYMotorPosistions[1])
        yMotorPosistion = maxYMotorPosistions[1];
}
/**
 *  Check the position of the motor and update the zMotorPosistion variable
 *  keep the zMotorPosistion variable between 0 and 920 for accuracy
 */
void setYMotorPosistion()
{
    yMotorPosistion = yMotorDirection ? yMotorPosistion - 1 : yMotorPosistion + 1;
}
/**
 * Check the position of the motor and send it to the main arduino
 */
void setXMotorPosistion()
{
    xMotorPosistion = xMotorDirection ? xMotorPosistion - 1 : xMotorPosistion + 1;
}

/**
 *functie voor het versturen van data
 */
void requestEvents()
{
    if (requestXMotorPosistion)
    {
        Wire.write((byte *)&xMotorPosistion, sizeof(xMotorPosistion));
    }
    if (!requestXMotorPosistion)
    {
        Wire.write((byte *)&yMotorPosistion, sizeof(yMotorPosistion));
    }
}

/**
 *functie voor het ontvangen van data
 */
void receiveEvents(int numBytes)
{
    receive = Wire.read();
    if (receive == 101)
        setMotorDirection(true);
    if (receive == 102)
        setMotorDirection(false);
    if (receive >= 0 && receive <= 100)
        setMotorSpeed(receive);
    if (receive == 103)
        xMotorDirection = true;
    if (receive == 104)
        xMotorDirection = false;
    if (receive == 105)
        yMotorDirection = true;
    if (receive == 106)
        yMotorDirection = false;
    if (receive == 107)
        resetMotorPositions();
    if (receive == 108)
        requestXMotorPosistion = true;
    if (receive == 109)
        requestXMotorPosistion = false;
}

void resetMotorPositions()
{
    xMotorPosistion = 0;
    yMotorPosistion = 0;
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