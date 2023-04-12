#include <stdio.h>
#include <wiringPi.h>

#define LED_VERMELHO 20

int main (void)
{
    wiringPiSetup();

    pinMode(LED_VERMELHO, OUTPUT);
    for(;;){
        digitalWrite(LED_VERMELHO, HIGH);    //On
        delay(1000);
        digitalWrite(LED_VERMELHO, LOW);
        delay(1000);
    }
    return 0;
}