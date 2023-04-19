#define X_MOTOR_DIRECTION 12
#define X_MOTOR_SPEED 3
#define Y_MOTOR_DIRECTION 13
#define Y_MOTOR_SPEED 11

enum axis
{
    X,
    Y,
    Z
};

void setup()
{
    // setup X motor
    pinMode(X_MOTOR_DIRECTION, OUTPUT);
    pinMode(X_MOTOR_SPEED, OUTPUT);
}

void loop()
{
    setMotorDirection(X, true);
    setMotorSpeed(X, 255);
    delay(1000);
    setMotorDirection(X, false);
    setMotorSpeed(X, 255);
    delay(1000);
    setMotorSpeed(X, 0);

    setMotorDirection(Y, true);
    setMotorSpeed(Y, 255);
    delay(1000);
    setMotorDirection(Y, false);
    setMotorSpeed(Y, 255);
    delay(1000);
    setMotorSpeed(Y, 0);
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