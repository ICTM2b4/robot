#include <ezButton.h>
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
    joystickButton.setDebounceTime(50);
}

void loop()
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

        joystickXSpeed = 0;
    }

    if (joystickXValue > 550)

    {

        joystickXSpeed = map(joystickXValue, 550, 1020, 10, 255);
    }

    if (joystickXValue < 470)

    {
        Serial.print("<470:");

        joystickXSpeed = map(joystickXValue, 470, 0, 10, 255);
    }

    // Y SPEED //

    if ((joystickYValue >= 490) && (joystickYValue <= 530))
    {

        joystickYSpeed = 0;
    }

    if (joystickYValue > 550)

    {

        joystickYSpeed = map(joystickYValue, 550, 1020, 10, 255);
    }

    if (joystickYValue < 470)

    {
        joystickYSpeed = map(joystickYValue, 470, 0, 10, 255);
    }

    Serial.print("joystickXSpeed= ");

    Serial.print(joystickXSpeed);

    Serial.print(" ,joystickYSpeed= ");

    Serial.println(joystickYSpeed);
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
    }
    if (joystickXValue > 550)
    {
        // pleur x links
        joystickXSpeed = map(joystickXValue, 550, 1020, 10, 255);
    }

    if (joystickXValue < 470)
    {
        // pleur x rechts
        joystickXSpeed = map(joystickXValue, 470, 0, 10, 255);
    }

    // Y SPEED //
    if ((joystickYValue >= 490) && (joystickYValue <= 530))
    {
        // pleur y stil
        joystickYSpeed = 0;
    }
    if (joystickYValue > 550)
    {
        // pleur y omlaag
        joystickYSpeed = map(joystickYValue, 550, 1020, 10, 255);
    }
    if (joystickYValue < 470)
    {
        // pleur y omhoog
        joystickYSpeed = map(joystickYValue, 470, 0, 10, 255);
    }
    Serial.print("joystickXSpeed= ");
    Serial.print(joystickXSpeed);
    Serial.print(" ,joystickYSpeed= ");
    Serial.println(joystickYSpeed);
}