struct Point
{
    int x;
    int y;
};
int amountOfPoints;
String *convertMessageToPositionsArray(String message)
{
    amountOfPoints = message.length() / 4 + 1;
    String pointsArray[amountOfPoints];

    for (int i = 0; i < amountOfPoints; i++)
    {
        int pointIndex = message.indexOf('.');
        pointsArray[i] = message.substring(0, pointIndex);
        message = message.substring(pointIndex + 1, message.length());

        Serial.println(pointsArray[i]);
        // Serial.println(pointsArray[i].y);
    }

    return pointsArray;
}

void collectProducts(String pointsArray[])
{
    for (int i = 0; i < amountOfPoints; i++)
    {
        Serial.println(pointsArray[i]);
    }
}

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    checkMessages();
}

// ignore this
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
    if (message.startsWith("collectProducts(") && message.endsWith(")"))
    {
        message = message.substring(16, message.length() - 1);
        Serial.println(message);
        // convertMessageToPositionsArray(message);
        collectProducts(convertMessageToPositionsArray(message));
    }
}
