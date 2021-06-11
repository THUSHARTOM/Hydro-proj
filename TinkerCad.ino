#include <LiquidCrystal.h>

int analogPin1 = A0;
int analogPin2 = A1;
int analogPin3 = A2;
int digitalPin = 13;
int relay = 7;

int val1 = 0;
int val2 = 0;
int val3 = 0;
int junc1 = 0;
int junc2 = 0;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    lcd.begin(16, 2);
    lcd.print("Working Status");
    pinMode(relay, OUTPUT);
    pinMode(digitalPin, OUTPUT);
    pinMode(analogPin1, INPUT);
    pinMode(analogPin2, INPUT);
    pinMode(analogPin3, INPUT);
    // digitalWrite(relay, HIGH);
    Serial.begin(9600); //  setup serial
}

void loop()
{
    val1 = analogRead(analogPin1);
    val2 = analogRead(analogPin2);
    val3 = analogRead(analogPin3);

    if (val1 != val2)
    {
        junc1 = 1;
    }

    if (val2 != val3)
    {
        junc2 = 1;
    }

    if (val1 == val2 && val2 == val3)
    {
        junc1 = 0;
        junc2 = 0;
    }

    if (junc1 == 1 && junc2 == 1)
    {
        digitalWrite(relay, HIGH);
        lcd.clear();
        lcd.print("Working status");
        lcd.setCursor(0, 1);
        lcd.print("Leak junc 1&2");
        Serial.write("Leak junc 1&2");
        Serial.println(" ");
    }

    else if (junc1 == 1 && junc2 == 0)
    {
        digitalWrite(relay, HIGH);
        lcd.clear();
        lcd.print("Working status");
        lcd.setCursor(0, 1);
        lcd.print("Leak junction1");
        Serial.write("Leak in junction1");
        Serial.println(" ");
    }

    else if (junc2 == 1 && junc1 == 0)
    {
        digitalWrite(relay, HIGH);
        lcd.clear();
        lcd.print("Working status");
        lcd.setCursor(0, 1);
        lcd.print("Leak junction2");
        Serial.write("Leak in junction2");
        Serial.println(" ");
    }

    else
    {
        digitalWrite(relay, LOW);
        lcd.clear();
        lcd.print("Working status");
        lcd.setCursor(0, 1);
        lcd.print("Working ok");
        Serial.write("Working ok");
        Serial.println(" ");
    }
    junc1 = 0;
    junc2 = 0;
    delay(500);
}
