int amountOfPoints;
String pointsArray[25];
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
    if (amountOfPoints > 25)
        return;
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
        convertMessageToPositionsArray(message);
        collectProducts();
    }
}
