#include <ezButton.h>

// define en include voor commmunicatie
#include <Wire.h>
#define I2C_SLAVE1_ADDRESS 11 // slave adress
#define PAYLOAD_SIZE 2
// define  emergencyButton
const int interruptPin = 2;
bool emergencyButtonstate = false;
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
bool pickingOrder = false;
// motor positions
int xMotorPosistion;
int yMotorPosistion;
int ZMotorPosistion;
// motor directions
bool yMotorDirection;
// locations of the warehouse
int xLocations[5] = {4631, 3912, 3220, 2521, 1801};
int yLocations[5] = {2317, 1814, 1288, 776, 269};
// store the closest grid number
int closestX = 0;
int closestY = 0;
bool isCloseToStart = false;
// data for collecting products
int amountOfPoints;
String pointsArray[25];

void setup()
{
    // setup comunicatie I2C
    Wire.begin();
    Serial.begin(9600);
    // setup emergencyButton
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), emergencyButton, FALLING);
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
    if (emergencyButtonloop())
        return;
    if (checkMessagesFromSerial())
        processMessages();
    if (!pickingOrder)
        checkJoystick();
    getMotorPositions();
    setClosestGridNumber();
}

/**
 * this function sends the current position of the robot to the HMI
 */
void printGridPosition()
{
    if (isCloseToStart)
    {
        Serial.println("5,4");
        return;
    }
    Serial.println(String(closestX) + "," + String(closestY));
}

/**
 * this functions takes a look at the robot's current position and checks if it is close to a grid number
 * if it is close to start it will set the isCloseToStart variable to true
 */
void setClosestGridNumber()
{
    if (xMotorPosistion < 1200)
    {
        isCloseToStart = true;
        return;
    }
    isCloseToStart = false;
    closestX = findClosestIndex(xMotorPosistion, xLocations, 5);
    closestY = findClosestIndex(yMotorPosistion, yLocations, 5);
}

/**
 * this function checks if the robot is close to a grid number based the given params
 * @param target the target value
 * @param arr the array with the grid numbers
 * @param size the size of the array
 */
int findClosestIndex(int target, int arr[], int size)
{
    int closestValue = arr[0]; // Initialize with the first value in the array
    int index = 0;
    for (int i = 1; i < size; i++)
    {
        // Calculate the absolute difference between the target value and the current array element
        int diff = abs(target - arr[i]);

        // Check if the current element is closer to the target than the previous closest value
        if (diff < abs(target - closestValue))
        {
            closestValue = arr[i];
            index = i;
        }
    }

    return index;
}

/**
 * print the current position of the robot in the serial monitor
 */
void printPosition()
{
    Serial.println("current: " + String(xMotorPosistion) + " " + String(yMotorPosistion) + " " + String(ZMotorPosistion));
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
    while (isStart == true)
    {
        if (millis() - MillisGoStart > 1000)
        {
            Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
            Wire.write(111);
            Wire.endTransmission();
            Wire.requestFrom(I2C_SLAVE1_ADDRESS, 2);
            int receivedValue = Wire.read();

            MillisGoStart = millis();
            if (receivedValue == 1)
            {
                setMotorSpeed(X, 0);
                setMotorSpeed(Y, 0);
                xMotorPosistion = 0;
                yMotorPosistion = 0;
                resetMotorPositions();
                isStart = false;
            }
            else if (receivedValue == 0)
            {
                setMotorDirection(X, true);
                setMotorSpeed(X, 255);
                setMotorDirection(Y, true);
                setMotorSpeed(Y, 255);
            }
        }
    }
}
/**
 * this function sends the robot to a position in the warehouse, this is done based on the cords
 * @param x the x position
 * @param y the y position
 */
void goToCords(int x, int y)
{
    while ((xMotorPosistion != x || yMotorPosistion != y) && !emergencyButtonstate)
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
        setClosestGridNumber();
        if (checkMessagesFromSerial())
            processMessages();
    }
    setMotorSpeed(X, 0);
    setMotorSpeed(Y, 0);
}

void pickupProduct(int productNumber)
{
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    switch (productNumber)
    {
    case 1:
        Wire.write(112);
        break;
    case 2:
        Wire.write(113);
        break;
    case 3:
        Wire.write(114);
        break;
    default:
        break;
    }
    Wire.endTransmission();

    while (true)
    {
        Wire.requestFrom(I2C_SLAVE1_ADDRESS, 2);
        int receivedValue = Wire.read();
        if (receivedValue == 1)
            break;
    }

    getMotorPositions();
    goToCords(xMotorPosistion, yMotorPosistion + 150);
    // Serial.println("Y reached");
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(115);
    Wire.endTransmission();

    while (true)
    {
        // Serial.println("wait till arm reached first position");
        Wire.requestFrom(I2C_SLAVE1_ADDRESS, 2);
        int receivedValue = Wire.read();
        if (receivedValue == 0)
            break;
    }
    // Serial.println("product has been picked up");
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
    // get the x position
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(108);
    Wire.endTransmission();

    Wire.requestFrom(I2C_SLAVE1_ADDRESS, sizeof(5000));
    int receivedXValue = 0;
    Wire.readBytes((byte *)&receivedXValue, sizeof(receivedXValue));
    Wire.endTransmission();
    xMotorPosistion = receivedXValue;

    // get the y position
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(109);
    Wire.endTransmission();

    Wire.requestFrom(I2C_SLAVE1_ADDRESS, sizeof(5000));
    int receivedYValue = 0;
    Wire.readBytes((byte *)&receivedYValue, sizeof(receivedYValue));
    Wire.endTransmission();
    yMotorPosistion = receivedYValue;
    // get the z position
    Wire.beginTransmission(I2C_SLAVE1_ADDRESS);
    Wire.write(116);
    Wire.endTransmission();

    Wire.requestFrom(I2C_SLAVE1_ADDRESS, sizeof(5000));
    int receivedZValue = 0;
    Wire.readBytes((byte *)&receivedZValue, sizeof(receivedZValue));
    Wire.endTransmission();
    ZMotorPosistion = receivedZValue;
}

/**
 * check if there is a message in the serial buffer
 * @return true if there is a message, false if there is no message
 */
bool checkMessagesFromSerial()
{
    if (Serial.available() <= 0)
        return false;
    return true;
}
/**
 * read the message from the serial buffer
 * @return the message
 */
String getMessageFromSerial()
{
    String message = "";
    while (Serial.available())
    {
        char c = Serial.read();
        message += c;
        delay(2);
    }
    return message;
}

/**
 *  check if there is a message in the serial buffer
 *  if there is a message, read it and execute the related command
 */
void processMessages()
{
    // read the message
    String message = getMessageFromSerial();

    if (message == "getGridPosition")
        printGridPosition();

    // if the robot is picking a order don't process any other messages
    if (pickingOrder)
        return;
    // commands for automatic control from the HMI
    if (message.startsWith("collectProducts(") && message.endsWith(")"))
    {
        message = message.substring(16, message.length() - 1);
        convertMessageToPositionsArray(message);
        collectProducts();
    }

    // commands for manual control
    if (message == "goToStart")
        goToStart();
    if (message.startsWith("goToCords(") && message.endsWith(")"))
        goToCords(getCordFromMessage(X, message), getCordFromMessage(Y, message));
    if (message.startsWith("goToPosition(") && message.endsWith(")"))
    {
        message.remove(0, 13);
        message.remove(message.length() - 1);
        goToPosition(getPositionFromMessage(X, message), getPositionFromMessage(Y, message));
    }
}

/**
 * this function converts a message to an array of multiple x an y positions
 * @param message the message
 * @return the array of positions
 */
void convertMessageToPositionsArray(String message)
{
    amountOfPoints = message.length() / 4 + 1;
    memset(pointsArray, 0, sizeof(pointsArray));
    if (amountOfPoints > 25)
        return;

    for (int i = 0; i < amountOfPoints; i++)
    {
        int pointIndex = message.indexOf('.');
        pointsArray[i] = message.substring(0, pointIndex);
        message = message.substring(pointIndex + 1, message.length());
    }
}

void collectProducts()
{
    pickingOrder = true;
    if (amountOfPoints > 25)
        return;
    goToStart();
    int productCount = 1;
    for (int i = 0; i < amountOfPoints; i++)
    {
        // Serial.println("going to: " + pointsArray[i]);
        goToPosition(getPositionFromMessage(Y, pointsArray[i]), getPositionFromMessage(X, pointsArray[i]));
        pickupProduct(productCount);
        productCount++;
        if (productCount == 4)
            productCount = 1;

        if ((i + 1) % 3 == 0)
        {
            goToStart();
            delay(1000);
        }
    }
    goToStart();
    pickingOrder = false;
}

/**
 * this function sends the robot to a position in the warehouse
 * @param row the row of the warehouse
 * @param column the column of the warehouse
 */
void goToPosition(int row, int column)
{
    goToCords(xLocations[column - 1], yLocations[row - 1]);
}

/**
 * this function extracts the position from a message
 * @param target X or Y
 * @param message the message
 * @return the position
 */
int getPositionFromMessage(axis target, String message)
{

    int commaIndex = message.indexOf(',');

    if (commaIndex != -1)
    {
        if (target == X)
        {
            String xString = message.substring(0, commaIndex);
            return xString.toInt();
        }
        if (target == Y)
        {
            String yString = message.substring(commaIndex + 1);
            return yString.toInt();
        }
    }
    return 0;
}

/**
 * this function extracts the cords from a message
 * @param target X or Y
 * @param message the message
 * @return the cords
 */
int getCordFromMessage(axis target, String message)
{
    message.remove(0, 10);
    message.remove(message.length() - 1);

    int commaIndex = message.indexOf(',');

    if (commaIndex != -1)
    {
        if (target == X)
        {
            String xString = message.substring(0, commaIndex);
            return xString.toInt();
        }
        if (target == Y)
        {
            String yString = message.substring(commaIndex + 1);
            return yString.toInt();
        }
    }
    return 0;
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
        if (ZMotorPosistion == 5)
            joystickZAxis = !joystickZAxis;
        else
        {
            getMotorPositions();
            goToCords(xMotorPosistion, yMotorPosistion + 100);
        }
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

bool emergencyButtonloop()
{
    if (emergencyButtonstate)
    {
        String serialInput;
        if (Serial.available() > 0)
        {
            serialInput = Serial.readString();
        }
        if (serialInput == "reset")
        {
            Serial.println("emergencyButton gereset");
            emergencyButtonstate = false;
        }
        if (digitalRead(JOYSTICK_BUTTON_PIN) == LOW)
        {
            Serial.println("emergencyButton gereset");
            emergencyButtonstate = false;
        }
        analogWrite(X_MOTOR_SPEED, 0);
        analogWrite(Y_MOTOR_SPEED, 0);
        sentSpeedData(0);
        return true;
    }
    else
        return false;
}
/**
 * emergencyButton interupt function
 */
void emergencyButton()
{
    emergencyButtonstate = true;
    Serial.println("emergencyButton knop ingedrukt");
    analogWrite(X_MOTOR_SPEED, 0);
    analogWrite(Y_MOTOR_SPEED, 0);
}