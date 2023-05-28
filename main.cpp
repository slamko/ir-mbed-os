#include "mbed.h"
#include "rc5.h"

// Blinking rate in milliseconds
#define BLINKING_RATE     500ms
DigitalOut led(LED1);

void blink() {
    led = !led;
    ThisThread::sleep_for(BLINKING_RATE);
}

RC5::Decoder d1 {D13, {
    {53, &blink},
    {35, &blink}
}};

int main()
{
    RC5::init(d1);
    // Initialise the digital pin LED1 as an output

    while (true) {
    }
}
