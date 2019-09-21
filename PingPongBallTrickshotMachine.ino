#include <DirectIO.h>

Output<2> motorDriverIN1;

// release:
//motorDriverIN1 HIGH;
//motorDriverIN2 LOW;
// grab:
//motorDriverIN1 LOW;
//motorDriverIN2 HIGH;
Input<4> button(false);
Input<5> phototransistor(false);
Output<8> buzzer;

int releasingCycleCount = 0;
int buttonCoolDownCount = 0;
int fullRotationCount = 0;
bool phototransistorOldState = false;

enum GrabState
{
    IDLE,
    STARTGRABBING,
    GRABBING,
    STARTRELEASING,
    RELEASING,
    RELEASEDONE
};
GrabState grabState;

void setup()
{
    motorDriverIN1 = false;
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);

    grabState = IDLE;
}

void loop()
{
    bool buttonIsHigh = button;

    if (grabState == IDLE && buttonIsHigh)
    {
        motorDriverIN1 = false;
        analogWrite(3, 120);
        grabState = STARTGRABBING;
        delayMicroseconds(1500);
    }

    if (grabState == STARTGRABBING && !buttonIsHigh)
    {
        grabState = GRABBING;
    }

    if (grabState == GRABBING && buttonIsHigh)
    {
        digitalWrite(3, false);
        motorDriverIN1 = true;

        grabState = RELEASING;
    }

    if (grabState == RELEASING)
    {
        releasingCycleCount++;
        if (releasingCycleCount > 80)
        {
            releasingCycleCount = 0;
            motorDriverIN1 = false;
            grabState = RELEASEDONE;
        }
    }

    if (grabState == RELEASEDONE && !buttonIsHigh)
    {
        grabState = IDLE;
    }

    phototransistorOldState = phototransistor;

    // bit bang phototransistor and button in order to execute the "ball release" code as timely as possible.
    while (grabState == GRABBING && !(phototransistor || button))
        ;

    if (grabState == GRABBING && phototransistor && !phototransistorOldState)
    {
        // positive flank on phototransistor
        fullRotationCount++;
        if (fullRotationCount > 50)
        {
            fullRotationCount = 0;
            digitalWrite(3, false);
            motorDriverIN1 = true;

            grabState = RELEASING;
        }
    }

    //Buzzer
    if (phototransistor)
    {
        buzzer = !buzzer;
    }

    // delay x microseconds each cycle
    delayMicroseconds(150);
}