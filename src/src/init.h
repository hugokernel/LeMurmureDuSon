
#include <Mds.h>

#define UP(x)   digitalWrite(x, HIGH)
#define DOWN(x) digitalWrite(x, LOW)
#define READ(x) digitalRead(x)

#define P   Serial.print
#define PLN Serial.println

#define PPSTR(string)       PP(PSTR(string))
#define PPSTRLN(string)     PP(PSTR(string)); PLN()

inline void PP(PGM_P s) {
    for (;;) {
        char c = pgm_read_byte(s++);
        if (c == 0)
            break;
        Serial.print(c);
    }
}

//Mds mds;

