/*
Project:- Hydro
tasks
-----
1. take input from 3 flow meters
2. check for leakages
3. close the valve if break
4. send alert on gsm upon break
5. Show output on display

*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#include <GSM.h>

#define PINNUMBER "" // SIM cards may have a PIN number that enables their functionality. If your SIM has no PIN, you can leave it blank

// initialize the library instance
GSM gsmAccess;
GSM_SMS sms;

volatile int flow_frequency1; // Measures flow sensor pulses
volatile int flow_frequency2; // Measures flow sensor pulses
volatile int flow_frequency3; // Measures flow sensor pulses

unsigned int l_hour;            // Calculated litres/hour
unsigned char flowsensor1 = 3;  // Sensor Input
unsigned char flowsensor2 = 4;  // Sensor Input
unsigned char flowsensor3 = 18; // Sensor Input ## Because Arduino Mega has ( 2, 3, 18, 19, 20, 21) as interrupt
unsigned long currentTime;
unsigned long cloopTime;
int relay = 7;

int val1 = 0;
int val2 = 0;
int val3 = 0;
int junc1 = 0;
int junc2 = 0;

char remoteNum[20] = "9847******"; // number to send alert messages to
char txtMsg[200];                  //The message

void flow1() // Interrupt function
{
    flow_frequency1++;
}
void flow2() // Interrupt function
{
    flow_frequency2++;
}
void flow3() // Interrupt function
{
    flow_frequency3++;
}

void setup()
{
    lcd.begin(16, 2);
    lcd.print("Working Status");

    pinMode(flowsensor1, INPUT);
    pinMode(flowsensor2, INPUT);
    pinMode(flowsensor3, INPUT);

    digitalWrite(flowsensor1, HIGH); // Optional Internal Pull-Up ##Learn pull up
    digitalWrite(flowsensor2, HIGH); // Optional Internal Pull-Up
    digitalWrite(flowsensor3, HIGH); // Optional Internal Pull-Up

    Serial.begin(9600);
    boolean notConnected = true; // connection state

    while (notConnected)
    {
        if (gsmAccess.begin(PINNUMBER) == GSM_READY)
        {
            notConnected = false;
        }
        else
        {
            Serial.println("Not connected");
            delay(1000);
        }
    }

    Serial.println("GSM initialized");

    // ## learn about flow sensor interfacing with arduino here - https://bc-robotics.com/tutorials/using-a-flow-sensor-with-arduino/
    // ## Learn about inturupts here - https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/

    attachInterrupt(digitalPinToInterrupt(flowsensor1), flow1, RISING); // Setup Interrupt
    attachInterrupt(digitalPinToInterrupt(flowsensor2), flow2, RISING); // Setup Interrupt
    attachInterrupt(digitalPinToInterrupt(flowsensor3), flow3, RISING); // Setup Interrupt
    sei();                                                              // Enable interrupts
    currentTime = millis();
    cloopTime = currentTime;
}

void loop()
{
    currentTime = millis();
    // Every second, calculate and print litres/hour
    if (currentTime >= (cloopTime + 1000))
    {
        cloopTime = currentTime; // Updates cloopTime
        // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
        val1 = (flow_frequency1 * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
        val2 = (flow_frequency1 * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
        val3 = (flow_frequency1 * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour

        // Checking each junciton and setting flags if (val1 != val2)
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

        // Triggering outputs based on juntion flags
        if (junc1 == 1 && junc2 == 1)
        {
            digitalWrite(relay, HIGH); // Turns off the valve
            lcd.clear();
            lcd.print("Working status");
            lcd.setCursor(0, 1);
            lcd.print("Leak junc 1&2");

            Serial.println("SENDING");
            Serial.println();
            Serial.println("Message:");
            Serial.println(txtMsg);

            strcpy(txtMsg, "Leak junc 1&2");
            // txtMsg[i] = "Leak junc 1&2"; // send the message
            sms.beginSMS(remoteNum);
            sms.print(txtMsg);
            sms.endSMS();
            Serial.println("\nCOMPLETE!\n");
        }

        else if (junc1 == 1 && junc2 == 0)
        {
            digitalWrite(relay, HIGH); // Turns off the valve
            lcd.clear();
            lcd.print("Working status");
            lcd.setCursor(0, 1);
            lcd.print("Leak junction1");
            Serial.write("Leak in junction1");

            Serial.println("SENDING");
            Serial.println();
            Serial.println("Message:");
            Serial.println(txtMsg);

            strcpy(txtMsg, "Leak in junction1");
            // txtMsg[i] = "Leak in junction1"; // send the message
            sms.beginSMS(remoteNum);
            sms.print(txtMsg);
            sms.endSMS();
            Serial.println("\nCOMPLETE!\n");
        }

        else if (junc2 == 1 && junc1 == 0)
        {
            digitalWrite(relay, HIGH); // Turns off the valve
            lcd.clear();
            lcd.print("Working status");
            lcd.setCursor(0, 1);
            lcd.print("Leak junction2");
            Serial.write("Leak in junction2");

            Serial.println("SENDING");
            Serial.println();
            Serial.println("Message:");
            Serial.println(txtMsg);

            strcpy(txtMsg, "Leak in junction2");
            // txtMsg[i] = "Leak in junction2"; // send the message
            sms.beginSMS(remoteNum);
            sms.print(txtMsg);
            sms.endSMS();
            Serial.println("\nCOMPLETE!\n");
        }

        else // Normal execution, without any leakage
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

        flow_frequency1 = 0; // Reset Counter
        flow_frequency2 = 0; // Reset Counter
        flow_frequency3 = 0; // Reset Counter

        Serial.print("Flow meter 1:");
        Serial.print(val1, DEC); // Print litres/hour # DEC(decimal, or base 10)
        Serial.println(" L/hour");
        Serial.print("Flow meter 2:");
        Serial.print(val2, DEC); // Print litres/hour # DEC(decimal, or base 10)
        Serial.println(" L/hour");
        Serial.print("Flow meter 3:");
        Serial.print(val3, DEC); // Print litres/hour # DEC(decimal, or base 10)
        Serial.println(" L/hour");
    }
}
