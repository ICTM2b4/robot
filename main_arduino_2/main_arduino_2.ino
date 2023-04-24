

// setup the motors
#define Z_MOTOR_DIRECTION 12
#define Z_MOTOR_SPEED 3

void setup()
{
    Serial.begin(9600);
    // setup motor
    pinMode(Z_MOTOR_DIRECTION, OUTPUT);
    pinMode(Z_MOTOR_SPEED, OUTPUT);
}

void loop()
{
    checkMessages();
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
 *  set the speed of the motor
 * @param speed 0 - 255
 */
void setMotorSpeed(int speed)
{
    analogWrite(Z_MOTOR_SPEED, speed);
}

/**
 *   set the direction of the right motor
 *   @param direction true = forward, false = backward
 */
void setMotorDirection(bool direction)
{
    if (direction)
    {
        digitalWrite(Z_MOTOR_DIRECTION, LOW);
        return;
    }
    digitalWrite(Z_MOTOR_DIRECTION, HIGH);
}