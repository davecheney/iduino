// iduinio - The interactive Arduino

#include <avr/wdt.h>

unsigned char R0;  // scratch register
unsigned char* R1;  // address register
unsigned char* R2;  // source register

void setup()  { 
  Serial.begin(115200);
} 

void printchar(unsigned char c) {
  if ((c >= 0) && (c<= 9)) {
    Serial.write(c + 0x30);
  } 
  else if ((c >= 0x0a) && (c <=0x0f)) {
    Serial.write(c + 0x61 - 0x0a);
  }
}   

void loop()  {
  if (Serial.available()) {
    signed int c = Serial.read();
    unsigned char b = c & 0xff;
    switch (b) {
    case '@':
      R1 = (unsigned char*)R0;
      break;
    case '#':
      R2 = (unsigned char*)R0;
      break;
    case '+':
      *R1++;
      break;
    case '-':
      *R1--;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      R0 <<=4;
      R0 |= (b & 0xf);      
      break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
      R0 <<=4;
      R0 |= ((b - 87)) & 0xf;
      break;
    case 'n':
      R1++;
      break;
    case '[':
      R0 = 0; // optional
      break;
    case 'l':
      R0 = *R2;
      break;
    case 'p':
      Serial.write(b);
      Serial.write('\r');
      Serial.write('\n');
      printchar((*R1) >> 4);
      printchar((*R1) &0xf);
      Serial.write('\r');
      Serial.write('\n');
      return; // don't print input character
    case 's':
      *R1 = R0;
      break;
    case 'z':
      *R1 = R0; 
      R1++;
      break;
    case '|': // OR the value in *R1 with R0
      *R1 |= R0;
      break;
    case '~': // BIC the value in *R1 with R0, unsets the bits in R0.
      *R1 &= ~R0;
      break; 
    case '.':  // delay 100ms
      delay(100);
      break;
    case '!':  // reset
      cli();                  // Clear interrupts
      wdt_enable(WDTO_15MS);      // Set the Watchdog to 15ms
      while(1);            // Enter an infinite loop
      break;
    case ']':
    case '\n':
    case '\r':
      break; // ignore
    default:
      b = '?';
    }
    Serial.write(b);
  }
}










