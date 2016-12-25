#include "mbed.h"

DigitalIn input(p20);
Timer timer;

int main() {
    timer.start();
    while(1) {
        printf("%d:%d\r\n", timer.read_ms(), input.read());
    }
}
