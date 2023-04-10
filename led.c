#include wiringPi.h

#define LED_PIN 8

int main(void) {
    wiringPiSetup();
    pinMode(LED_PIN, OUTPUT);

    while(1) {
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
        delay(1000);
    }

    return 0;
}