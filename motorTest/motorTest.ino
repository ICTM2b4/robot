#define X_MOTOR_DIRECTION 12
#define X_MOTOR_SPEED 3



void setup()
{
    // setup X motor
    pinMode(X_MOTOR_DIRECTION, OUTPUT);
    pinMode(X_MOTOR_SPEED, OUTPUT);
}

void loop()
{
    setXMotorDirection(true);
    setXMotorSpeed(255);
    delay(1000);
    setXMotorDirection(false);
    setXMotorSpeed(255);
    delay(1000);
}

/**
 *  set the speed of the right motor
 * @param speed 0 - 255
 */
void setXMotorSpeed(int speed)
{
    analogWrite(X_MOTOR_SPEED, speed);
}

/**
 *   set the direction of the right motor
 *   @param direction true = forward, false = backward
 */
void setXMotorDirection(bool direction)
{
    if (direction)
    {
        digitalWrite(X_MOTOR_DIRECTION, HIGH);
        return;
    }
    digitalWrite(X_MOTOR_DIRECTION, LOW);
}