#include <ezButton.h>

// define en include voor commmunicatie
#include <Wire.h>
#define I2C_SLAVE1_ADDRESS 11 // slave adress
#define PAYLOAD_SIZE 2

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
#define JOYSTICK_BUTTON_PIN 4
// setup the joystick with the ezButton library
ezButton joystickButton(JOYSTICK_BUTTON_PIN);
// setup the joystick values
int joystickXValue = 0;
int joystickYValue = 0;
int joystickButtonValue = 0;
// toggle between the XY and Z axis
bool joystickZAxis = false;
// toggle between joystick and serial control
bool allowJoystickControl = true;
// motor positions
int xMotorPosistion;
int yMotorPosistion;
bool yMotorDirection;
// enable debug mode
bool debug = true;
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
    // goToStart();
}

void loop()
{
    if (debug)
    {
        printPosition();
    }

    checkMessages();
    if (allowJoystickControl)
        checkJoystick();
    getMotorPositions();
}

/**
 * print the current position of the robot in the serial monitor
 */
void printPosition()
{
    Serial.println("current: " + String(xMotorPosistion) + " " + String(yMotorPosistion) + "");
}
/**
 * this function sends the robots arm to the start position
 */
void goToStart()
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(110);
    Wire.endTransmission();
    bool isStart = true;
    long MillisGoStart = millis();
            setMotorDirection(X, true);
            setMotorSpeed(X, 255);
            setMotorDirection(Y, true);
            setMotorSpeed(Y, 255);
    while (isStart)
    {

        if (millis() - MillisGoStart < 500)
        {
            Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
            Wire.write(111);
            Wire.endTransmission();
            Wire.requestFrom(I2C_SLAVE1_ADDRESS, 2);
            int receivedValue = Wire.read();
            MillisGoStart = millis();
            if(receivedValue == 0){
                setMotorSpeed(X, 0);
                setMotorSpeed(Y, 0);
                xMotorPosistion = 0;
                yMotorPosistion = 0;
                resetMotorPositions();
                isStart = false;
                }
            else if(receivedValue == 3){
            setMotorDirection(X, true);
            setMotorSpeed(X, 255);
            setMotorDirection(Y, true);
            setMotorSpeed(Y, 255);
            }
            else if(receivedValue == 2){
            setMotorSpeed(X, 0);
        }
             else if(receivedValue == 1){
            setMotorSpeed(Y, 0);
        }
        }
    }
}

void goToCords(int x, int y)
{
    while (xMotorPosistion != x || yMotorPosistion != y)
    {
        if (xMotorPosistion < x)
        {
            setMotorDirection(X, false);
            setMotorSpeed(X, 255);
        }
        else if (xMotorPosistion > x)
        {
            setMotorDirection(X, true);
            setMotorSpeed(X, 255);
        }
        else
        {
            setMotorSpeed(X, 0);
        }

        if (yMotorPosistion < y)
        {
            setMotorDirection(Y, false);
            setMotorSpeed(Y, 255);
        }
        else if (yMotorPosistion > y)
        {
            setMotorDirection(Y, true);
            setMotorSpeed(Y, 255);
        }
        else
        {
            setMotorSpeed(Y, 0);
        }
        getMotorPositions();
    }
}

/**
 * this function resets the motor positions on both arduinos
 */
void resetMotorPositions()
{
    xMotorPosistion = 0;
    yMotorPosistion = 0;
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(107);
    Wire.endTransmission();
}
/**
 *This function gets the motor positions from the second arduino
 */
void getMotorPositions()
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(108);
    Wire.endTransmission();

    Wire.requestFrom(I2C_SLAVE1_ADDRESS, sizeof(5000));
    int receivedXValue = 0;
    Wire.readBytes((byte *)&receivedXValue, sizeof(receivedXValue));
    Wire.endTransmission();

    xMotorPosistion = receivedXValue;

    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(109);
    Wire.endTransmission();

    Wire.requestFrom(I2C_SLAVE1_ADDRESS, sizeof(5000));
    int receivedYValue = 0;
    Wire.readBytes((byte *)&receivedYValue, sizeof(receivedYValue));
    yMotorPosistion = receivedYValue;
    Wire.endTransmission();
}

/**
 * this function sends data to the HMI application
 */
void sentMessage(String message)
{
    Serial.print(message);
}

/**
 *  check if there is a message in the serial buffer
 *  if there is a message, read it and execute the related command
 */
void checkMessages()
{
    // check if there is a message, if not, return
    if (Serial.available() <= 0)
        return;
    // read the message
    String message = "";
    while (Serial.available())
    {
        char c = Serial.read();
        message += c;
        delay(2);
    }
    // toggle between joystick and serial control
    if (message == "enableControlFromHMI")
        allowJoystickControl = false;
    if (message == "disableControlFromHMI")
        allowJoystickControl = true;
    // go to start position
    if (message == "goToStart")
        goToStart();
    if (message == "goToCords")
        goToCords(1000, 1000);
    // manual control
    if (message == "X+")
    {
        setMotorDirection(X, true);
        setMotorSpeed(X, 255);
    }
    else if (message == "X-")
    {
        setMotorDirection(X, false);
        setMotorSpeed(X, 255);
    }
    else if (message == "Y+")
    {
        setMotorDirection(Y, true);
        setMotorSpeed(Y, 255);
    }
    else if (message == "Y-")
    {
        setMotorDirection(Y, false);
        setMotorSpeed(Y, 255);
    }
    else if (message == "Z+")
    {
        setMotorDirection(Z, false);
        setMotorSpeed(Z, 255);
    }
    else if (message == "Z-")
    {
        setMotorDirection(Z, true);
        setMotorSpeed(Z, 255);
    }
    else if (message == "X0")
    {
        setMotorSpeed(X, 0);
    }
    else if (message == "Y0")
    {
        setMotorSpeed(Y, 0);
    }
    else if (message == "Z0")
    {
        setMotorSpeed(Z, 0);
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
        analogWrite(X_MOTOR_SPEED, (speed > 200) ? 200 : speed);
        break;
    case Y:
        if (yMotorDirection)
            analogWrite(Y_MOTOR_SPEED, (speed > 150) ? 150 : speed);
        else
            analogWrite(Y_MOTOR_SPEED, (speed > 200) ? 200 : speed);
        break;
    case Z:
        sentSpeedData((speed > 100) ? 100 : speed);
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
        sentDirectionData(X, direction);
        break;
    case Y:
        target = Y_MOTOR_DIRECTION;
        yMotorDirection = direction;
        sentDirectionData(Y, direction);
        break;
    case Z:
        sentDirectionData(Z, direction);
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
 * @param target X, Y or Z
 * @param direction true = forward, false = backward
 */
void sentDirectionData(axis target, bool direction)
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    if (target == X)
        Wire.write(direction ? 103 : 104);
    if (target == Y)
        Wire.write(direction ? 105 : 106);
    if (target == Z)
        Wire.write(direction ? 102 : 101);
    Wire.endTransmission();
}
/**
 * sent speed to second arduino for the Z axis
 * @param speed 0 - 100
 */
void sentSpeedData(int speed)
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(speed);
    Wire.endTransmission();
}