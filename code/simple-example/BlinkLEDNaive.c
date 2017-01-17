#include <avr/io.h>

int main() {
   DDRD = 0b10000000;
   PORTD = 0b00000000;
   while (1) {
     int i;
     for (i = 0; i < 100000; i++);
     PORTD ^= 0b10000000;
   }
   return 0;
 }
