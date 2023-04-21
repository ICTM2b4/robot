#include <ezButton.h>
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
// setup the calculated speed based on the joystick values
int joystickXSpeed;
int joystickYSpeed;

void setup()
{
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

void checkJoystick()
{
    joystickButton.loop();
    joystickXValue = analogRead(JOYSTICK_X_PIN);
    joystickYValue = analogRead(JOYSTICK_Y_PIN);
    joystickButtonValue = joystickButton.getState();
    if (joystickButton.isPressed())
    {
        Serial.println("The button is pressed");
        // TODO do something here
    }
    if (joystickButton.isReleased())
    {
        Serial.println("The button is released");
        // TODO do something here
    }

    // X SPEED //
    if ((joystickXValue >= 490) && (joystickXValue <= 530))
    {
        // pleur x stil
        joystickXSpeed = 0;
        setMotorSpeed(X, 0);
    }
    if (joystickXValue > 550)
    {
        // pleur x links
        setMotorDirection(X, false);
        setMotorSpeed(X, map(joystickXValue, 550, 1020, 10, 254));
        joystickXSpeed = map(joystickXValue, 550, 1020, 10, 254);
    }

    if (joystickXValue < 470)
    {
        // pleur x rechts
        setMotorDirection(X, true);
        setMotorSpeed(X, map(joystickXValue, 470, 0, 10, 255));
        joystickXSpeed = map(joystickXValue, 470, 0, 10, 255);
    }

    // Y SPEED //
    if ((joystickYValue >= 490) && (joystickYValue <= 530))
    {
        // pleur y stil
        joystickYSpeed = 0;
        setMotorSpeed(Y, 0);
    }
    if (joystickYValue > 550)
    {
        // pleur y omlaag
        setMotorDirection(Y, true);
        setMotorSpeed(Y, map(joystickYValue, 550, 1020, 10, 254));
        joystickYSpeed = map(joystickYValue, 550, 1020, 10, 254);
    }
    if (joystickYValue < 470)
    {
        // pleur y omhoog
        setMotorDirection(Y, false);
        setMotorSpeed(Y, map(joystickYValue, 470, 0, 10, 254));
        joystickYSpeed = map(joystickYValue, 470, 0, 10, 254);
    }
    Serial.print("joystickXSpeed= ");
    Serial.print(joystickXSpeed);
    Serial.print(" ,joystickYSpeed= ");
    Serial.println(joystickYSpeed);
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