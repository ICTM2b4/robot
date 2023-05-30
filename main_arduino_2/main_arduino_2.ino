#include <Wire.h>
#include <SharpIR.h>

SharpIR distanceSensorZAxis(SharpIR::GP2Y0A41SK0F, A2);
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
bool sentStart = false;
int xMotorPosistion = 0;
int yMotorPosistion = 0;
int zMotorPosistion = 0;
bool requestXMotorPosistion = false;
int maxXMotorPosistions[] = {0, 4931};
int maxYMotorPosistions[] = {0, 3000};
bool isStart = true;
int MillisGoStart;
int lastYMotorPosistion = 0;
int lastXMotorPosistion = 0;
int startStates = 0;
bool pickingUpProduct = false;
bool zAxisExtended = false;
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
    startLoop();
    getZMotorPosistion();
    fixMotorPositions();
    Serial.println("current: " + String(xMotorPosistion) + " " + String(yMotorPosistion) + " " + String(zMotorPosistion));
}

/**
 * fix the motor positions if they are out of bounds
 */
void fixMotorPositions()
{
    if (isStart)
        return;

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
 *  Check the position of the motor and update the yMotorPosistion variable
 */
void setYMotorPosistion()
{
    yMotorPosistion = yMotorDirection ? yMotorPosistion - 1 : yMotorPosistion + 1;
}
/**
 *  Check the position of the motor and update the xMotorPosistion variable
 */
void setXMotorPosistion()
{
    xMotorPosistion = xMotorDirection ? xMotorPosistion - 1 : xMotorPosistion + 1;
}

/**
 *  this function checks the infrared sensors and returns the distance of the z axis
 */
void getZMotorPosistion()
{
    zMotorPosistion = distanceSensorZAxis.getDistance();
}
/**
 *functie voor het versturen van data
 */
void requestEvents()
{
    if (sentStart)
    {
        Wire.write(startStates);
        sentStart = false;
        return;
    }
    if (pickingUpProduct)
    {
        Serial.println("sent back");
        Wire.write(zAxisExtended);
        pickingUpProduct = false;
        return;
    }
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
    if (receive == 110)
        goToStart();
    if (receive == 111)
        sentStart = true;
    if (receive == 112)
        pickupProduct(1);
    if (receive == 113)
        pickupProduct(2);
    if (receive == 114)
        pickupProduct(3);
    if (receive == 115)
        returnZAxis();
}

void pickupProduct(int productNumber)
{
    pickingUpProduct = true;
    zAxisExtended = false;
    int limit = 0;
    if (productNumber == 1)
        limit = 20;
    if (productNumber == 2)
        limit = 13;
    if (productNumber == 3)
        limit = 9;
    while (zMotorPosistion < limit)
    {
        getZMotorPosistion();
        setMotorDirection(true);
        setMotorSpeed(100);
    }
    setMotorSpeed(0);
    zAxisExtended = true;
}

void returnZAxis()
{
    pickingUpProduct = true;
    while (zMotorPosistion != 5)
    {
        getZMotorPosistion();
        setMotorDirection(false);
        setMotorSpeed(100);
    }
    //delay to compensate for the arm not reaching the end
    delay(500);
    setMotorSpeed(0);
    zAxisExtended = false;
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
/*
go to start position
*/
void goToStart()
{
    isStart = true;
    lastYMotorPosistion = 999999;
    lastXMotorPosistion = 999999;
    MillisGoStart = millis();

    startStates = 0;
}
/*
loops the start function to check if the motors are not moving
*/
void startLoop()
{
    if (isStart)
    {
        if (millis() - MillisGoStart > 1000)
        {
            MillisGoStart = millis();
            if ((yMotorPosistion == lastYMotorPosistion) && (xMotorPosistion == lastXMotorPosistion))
            {
                startStates = 1;
                isStart = false;
            }
            else
            {
                startStates = 0;
            }

            lastYMotorPosistion = yMotorPosistion;
            lastXMotorPosistion = xMotorPosistion;
        }
    }
}