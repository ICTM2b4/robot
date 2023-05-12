#include <Wire.h>
// params for receiving data from old arduino
#define I2C_SLAVE_ADDRESS 11
#define PAYLOAD_SIZE 2

int sent = 0;
int receive = 0;

// setup the motors
#define Z_MOTOR_DIRECTION 13
#define Z_MOTOR_SPEED 11
#define Z_MOTOR_POSITION_PIN 2
#define X_MOTOR_POSITION_PIN 3
bool zMotorDirection;
int zMotorPosistion = 0;
void setup()
{
    Serial.begin(9600);
    // setup motor
    pinMode(Z_MOTOR_DIRECTION, OUTPUT);
    pinMode(Z_MOTOR_SPEED, OUTPUT);
    pinMode(Z_MOTOR_POSITION_PIN, INPUT_PULLUP);
    // setup interrupt for motor position sensors (X and Z axis)
    attachInterrupt(digitalPinToInterrupt(Z_MOTOR_POSITION_PIN), setZMotorPosistion, RISING);
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
    Serial.println(zMotorPosistion);
}

/**
 *  Check the position of the motor and update the zMotorPosistion variable
 *  keep the zMotorPosistion variable between 0 and 920 for accuracy
 */
void setZMotorPosistion()
{
    if (zMotorPosistion == 0 && !zMotorDirection || zMotorPosistion == 920 && zMotorDirection)
        return;
    zMotorPosistion = zMotorDirection ? zMotorPosistion + 1 : zMotorPosistion - 1;
}
/**
 * Check the position of the motor and send it to the main arduino
 */
void setXMotorPosistion()
{
    // send message to old arduino
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write("");
    Wire.endTransmission();
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
    if (receive == 101)
        setMotorDirection(true);
    if (receive == 102)
        setMotorDirection(false);
    if (receive >= 0 && receive <= 100)
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