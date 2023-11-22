#include "mbed.h"
#include "rc5.h"
#include "sirc.h"

using namespace IR;

// Blinking rate in milliseconds
#define BLINKING_RATE     500ms
DigitalOut led(LED1);

void blink() {
    led = !led;
}
/*
RC5::Decoder d1 {D13, {
    {53, &blink},
    {35, &blink}
}};

RC5::Decoder d2 {D12, {
    {24, &blink}
}};
*/

SIRC::Decoder sony_telecommande 
    { D5, 
        { 
            {21, &blink}
        }
    }; 

int main()
{
    
    while (true) {
    }
}
