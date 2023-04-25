#include <ezButton.h>

// define en include voor commmunicatie
#include <Wire.h>
#define I2C_SLAVE1_ADDRESS 11 // slava adress
#define PAYLOAD_SIZE 2

int sent = 0;
// int recieve=0;

// setup the motors
#define X_MOTOR_DIRECTION 12
#define X_MOTOR_SPEED 3
#define Y_MOTOR_DIRECTION 13
#define Y_MOTOR_SPEED 11
// define a new type called axis with the values X, Y and Z
enum axis
{
    X,
    Y,
    Z
};
// setup the joystick pins
#define JOYSTICK_X_PIN A2
#define JOYSTICK_Y_PIN A3
#define JOYSTICK_BUTTON_PIN 2
// setup the joystick with the ezButton library
ezButton joystickButton(JOYSTICK_BUTTON_PIN);
// setup the joystick values
int joystickXValue = 0;
int joystickYValue = 0;
int joystickButtonValue = 0;
// toggle between the XY and Z axis
bool joystickZAxis = false;

void setup()
{
    // setup comunicatie I2C
    Wire.begin();

    Serial.begin(9600);
    // setup motor
    pinMode(X_MOTOR_DIRECTION, OUTPUT);
    pinMode(X_MOTOR_SPEED, OUTPUT);
    pinMode(Y_MOTOR_DIRECTION, OUTPUT);
    pinMode(Y_MOTOR_SPEED, OUTPUT);
    // setup the joystick
    joystickButton.setDebounceTime(50);
}

void loop()
{
    checkMessages();
    checkJoystick();
    sentData();
}

void sentData()
{
    // Send value to slave
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(sent);
    Wire.endTransmission();
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

    if (ontvangen == "X+")
    {
        setMotorDirection(X, true);
        setMotorSpeed(X, 255);
    }
    else if (ontvangen == "X-")
    {
        setMotorDirection(X, false);
        setMotorSpeed(X, 255);
    }
    else if (ontvangen == "Y+")
    {
        setMotorDirection(Y, true);
        setMotorSpeed(Y, 255);
    }
    else if (ontvangen == "Y-")
    {
        setMotorDirection(Y, false);
        setMotorSpeed(Y, 255);
    }
    else if (ontvangen == "X0")
    {
        setMotorSpeed(X, 0);
    }
    else if (ontvangen == "Y0")
    {
        setMotorSpeed(Y, 0);
    }
}
/**
 * this function checks the joystick values and sets the targeted axis's motor speed and direction
 */
void checkJoystick()
{
    joystickButton.loop();
    joystickXValue = analogRead(JOYSTICK_X_PIN);
    joystickYValue = analogRead(JOYSTICK_Y_PIN);
    joystickButtonValue = joystickButton.getState();
    if (joystickButton.isPressed())
        joystickZAxis = !joystickZAxis;
    if (joystickZAxis)
    {
        // set other axis stil to prevent drifting
        setMotorSpeed(Y, 0);
        setMotorSpeed(X, 0);
        joystickControlZAxis();
        return;
    }
    // set Z axis stil to prevent drifting
    setMotorSpeed(Z, 0);
    joystickControlXAxis();
    joystickControlYAxis();
}

/**
 * this function sets the motor speed and direction of the X axis.
 * this is based on the joystick value
 */
void joystickControlXAxis()
{
    // X SPEED //
    if ((joystickXValue >= 490) && (joystickXValue <= 530))
    {
        // set x stil
        setMotorSpeed(X, 0);
    }
    if (joystickXValue > 550)
    {
        // set x links
        setMotorDirection(X, false);
        setMotorSpeed(X, map(joystickXValue, 550, 1020, 10, 254));
    }

    if (joystickXValue < 470)
    {
        // set x rechts
        setMotorDirection(X, true);
        setMotorSpeed(X, map(joystickXValue, 470, 0, 10, 254));
    }
}

/**
 * this function sets the motor speed and direction of the Y axis.
 * this is based on the joystick value
 */
void joystickControlYAxis()
{
    // Y SPEED //
    if ((joystickYValue >= 490) && (joystickYValue <= 530))
    {
        // set y stil
        setMotorSpeed(Y, 0);
    }
    if (joystickYValue > 550)
    {
        // set y omlaag
        setMotorDirection(Y, true);
        setMotorSpeed(Y, map(joystickYValue, 550, 1020, 10, 254));
    }
    if (joystickYValue < 470)
    {
        // set y omhoog
        setMotorDirection(Y, false);
        setMotorSpeed(Y, map(joystickYValue, 470, 0, 10, 254));
    }
}

/**
 * this function sets the motor speed and direction of the Z axis.
 * this is based on the joystick value
 */
void joystickControlZAxis()
{
    if ((joystickYValue >= 490) && (joystickYValue <= 530))
    {
        // set Z stil
        setMotorSpeed(Z, 0);
    }
    if (joystickYValue > 550)
    {
        // set Z omlaag
        setMotorDirection(Z, true);
        setMotorSpeed(Z, map(joystickYValue, 550, 1020, 10, 254));
    }
    if (joystickYValue < 470)
    {
        // set Z omhoog
        setMotorDirection(Z, false);
        setMotorSpeed(Z, map(joystickYValue, 470, 0, 10, 254));
    }
}

/**
 *  set the speed of the motor
 * @param axis X, Y or Z
 * @param speed 0 - 255
 */
void setMotorSpeed(axis axis, int speed)
{
    switch (axis)
    {
    case X:
        analogWrite(X_MOTOR_SPEED, speed);
        break;
    case Y:
        analogWrite(Y_MOTOR_SPEED, speed);
        break;
    case Z:
        sentSpeedData((speed > 3) ? speed : 3);
        break;
    default:
        break;
    }
}

/**
 *   set the direction of the right motor
 *   @param axis X, Y or Z
 *   @param direction true = forward, false = backward
 */
void setMotorDirection(axis axis, bool direction)
{
    int target = 0;
    switch (axis)
    {
    case X:
        target = X_MOTOR_DIRECTION;
        break;
    case Y:
        target = Y_MOTOR_DIRECTION;
        break;
    case Z:
        sentDirectionData(direction);
        return;
        break;
    default:
        break;
    }

    if (direction)
    {
        digitalWrite(target, HIGH);
        return;
    }
    digitalWrite(target, LOW);
}
/**
 * sent direction to second arduino
 * @param direction true = forward, false = backward
 */
void sentDirectionData(bool direction)
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(direction ? 2 : 1);
    Wire.endTransmission();
}
/**
 * sent speed to second arduino
 * @param speed 3 - 255 | 2 to stop
 */
void sentSpeedData(int speed)
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(speed);
    Wire.endTransmission();
}