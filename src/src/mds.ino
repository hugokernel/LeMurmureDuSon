
#include "Wire.h"
#include "ADXL345.h"
#include "HL1606strip.h"
//#include <SerialCommand.h>

#define VIBRATOR A1     // PC1

#define CHG 3

#define M1 A2
#define M2 A3
#define M3 A6 // ?
#define M4 A7 // ?
#define M5 4
#define M6 7
#define M7 6
#define M8 5

#define REC_PLAY 8
#define LED 2

#define UP(x)   digitalWrite(x, HIGH)
#define DOWN(x) digitalWrite(x, LOW)
#define READ(x) digitalRead(x)

ADXL345 accel;

#define STRIP_D 11
#define STRIP_C 13
#define STRIP_L 10
HL1606strip strip = HL1606strip(STRIP_D, STRIP_L, STRIP_C, 6);

//#define P   Serial.print
//#define PLN Serial.println
//SerialCommand sCmd;

uint8_t Messages[] = {
    M1, M2, M3, M4, M5, M6, M7
};

uint8_t Leds[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};

inline void msgSelect(uint8_t index) {
    DOWN(Messages[index]);
}

inline void msgUnselect() {
    for (char i = 0; i < sizeof(Messages); i++) {
        UP(Messages[i]);
    }
}

inline bool isPlaying() {
    return !READ(LED);
}

bool record(uint8_t index) {
    DOWN(REC_PLAY);
    // Todo: Sleep for 30ns
    DOWN(Messages[index]);
}

bool play(uint8_t index) {
    UP(REC_PLAY);
    // Todo: Sleep for 30ns
    DOWN(Messages[index]);
}

bool stop() {
    msgUnselect();
}

inline void vibrate(int time) {
    UP(VIBRATOR);
    delay(time);
    DOWN(VIBRATOR);
}

inline void ledOff() {
    uint8_t c = 0;
    for (c = 0; c < strip.numLEDs(); c++) {
        strip.setLEDcolor(c, BLACK);
    }

    strip.writeStrip();
}

inline void ledOn(uint8_t index, uint8_t color) {
    strip.setLEDcolor(index, color);
    strip.writeStrip();
}

void setup()
{
    Serial.begin(9600);

    pinMode(VIBRATOR, OUTPUT);
    pinMode(CHG, INPUT);

    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
    pinMode(M3, OUTPUT);
    pinMode(M4, OUTPUT);
    pinMode(M5, OUTPUT);
    pinMode(M6, OUTPUT);
    pinMode(M7, OUTPUT);

    pinMode(REC_PLAY, OUTPUT);
    pinMode(LED, INPUT);

    DOWN(VIBRATOR);

    msgUnselect();

    UP(REC_PLAY);


    accel.powerOn();

    //accel.setRangeSetting(2);

    accel.setTapThreshold(0x40);
    accel.setTapDuration(0x30);
    accel.setDoubleTapLatency(0x40);
    accel.setDoubleTapWindow(0xFF);
    accel.setTapDetectionOnX(true);
    accel.setTapDetectionOnY(true);
    accel.setTapDetectionOnZ(true);
    accel.setInterrupt(1, true);
    accel.setInterruptMapping(1, true);
 /*
    accel.setAxisOffset(2, 3, 4);
    accel.setActivityThreshold(48);
    accel.setTapDuration(0x30);

    accel.setActivityX(true);
    accel.setActivityY(true);
    accel.setActivityZ(true);
*/

 /*

    colorWipe(RED, 40);
    record(M8);
    delay(3000);
    stop();

    // Setup callbacks for SerialCommand commands
    //sCmd.setDefaultHandler(unrecognized);
    //sCmd.addCommand("accel", cmd_accel);

    colorWipe(YELLOW, 40);
    play(M8);
    delay(3000);
    stop();
*/
}

/*
ISR(TIMER1_OVF_vect)
{
    //TCNT1 = 0;     // reset timer ct to 130 out of 255
    //TCNT1 = 34286;            // preload timer
    TCNT1 = 0;            // preload timer
    //TIFR1 = 0x100;    // timer2 int flag reg: clear timer overflow flag
}
*/

/*
void rainbowParty(uint8_t wait) {
  uint8_t i, j;

  for (i=0; i< strip.numLEDs(); i+=6) {
    // initialize strip with 'rainbow' of colors
    strip.setLEDcolor(i, RED);
    strip.setLEDcolor(i+1, YELLOW);
    strip.setLEDcolor(i+2, GREEN);
    strip.setLEDcolor(i+3, TEAL);
    strip.setLEDcolor(i+4, BLUE);
    strip.setLEDcolor(i+5, VIOLET);

  }
  strip.writeStrip();

  for (j=0; j < strip.numLEDs(); j++) {

    // now set every LED to the *next* LED color (cycling)
    uint8_t savedcolor = strip.getLEDcolor(0);
    for (i=1; i < strip.numLEDs(); i++) {
      strip.setLEDcolor(i-1, strip.getLEDcolor(i));  // move the color back one.
    }
    // cycle the first LED back to the last one
    strip.setLEDcolor(strip.numLEDs()-1, savedcolor);
    strip.writeStrip();
    delay(wait);
  }
}

// have one LED 'chase' around the strip
void chaseSingle(uint8_t color, uint8_t wait) {
  uint8_t i;

  // turn everything off
  for (i=0; i< strip.numLEDs(); i++) {
    strip.setLEDcolor(i, BLACK);
  }

  for (i=0; i < strip.numLEDs(); i++) {
    strip.setLEDcolor(i, color);
    if (i != 0) {
      // make the LED right before this one OFF
      strip.setLEDcolor(i-1, BLACK);
    }
    strip.writeStrip();
    delay(wait);
  }
  // turn off the last LED before leaving
  strip.setLEDcolor(strip.numLEDs() - 1, BLACK);
}

// fill the entire strip, with a delay between each pixel for a 'wipe' effect
void colorWipe(uint8_t color, uint8_t wait) {
  uint8_t i;

  for (i=0; i < strip.numLEDs(); i++) {
      strip.setLEDcolor(i, color);
      strip.writeStrip();
      delay(wait);
  }
}
*/

int getHigher(double data[], int &index, bool &isPlus) {
    int index = 0;
    double largest = abs(data[0]);
    isPlus = (data[0] > 0);
    for (int i = 0; i< 3; i++) {
        if (largest < abs(data[i])) {
            largest = abs(data[i]);
            isPlus = (data[i] > 0);
            index = i;
        }
    }

    return largest;
}

int x, y, z, i;
char data[3];

void loop() {

    bool isPlus = false;
    unsigned int absValue = 0;
    double xyz[3], gains[3], gains_orig[3];

/*
   colorWipe(RED, 40);
   colorWipe(YELLOW, 40);
   colorWipe(GREEN, 40);
   colorWipe(TEAL, 40);
   colorWipe(BLUE, 40);
   colorWipe(VIOLET, 40);
   colorWipe(WHITE, 40);
   colorWipe(BLACK, 40);

   // then a chase
   chaseSingle(RED, 40);
   chaseSingle(YELLOW, 40);
   chaseSingle(GREEN, 40);
   chaseSingle(TEAL, 40);
   chaseSingle(VIOLET, 40);
   chaseSingle(WHITE, 40);

    rainbowParty(60);
*/
/*
    char chg = READ(CHG);
    Serial.write('.');
    Serial.write((int)chg);
*/
    //sCmd.readSerial();
    /*
    delay(5000);
    Serial.write('.');

    return;

    DOWN(VIBRATOR);
    Serial.write("Record");
    record(0);
    delay(5000);
    stop();

    UP(VIBRATOR);
    Serial.write("Play");
    play(0);
    delay(5000);
    stop();

    while(!READ(LED));

    Serial.write("End!");

    delay(5000);
*/

    accel.getInterruptSource();

    if (accel.isActivitySourceOnX()) {
        Serial.print("isActivitySourceOnX(): ");
        Serial.println(accel.isActivitySourceOnX(), DEC);
    }

    if (accel.isActivitySourceOnY()) {
        Serial.print("accel.isActivitySourceOnY(): ");
        Serial.println(accel.isActivitySourceOnY(), DEC);
    }

    if (accel.isActivitySourceOnZ()) {
        Serial.print("accel.isActivitySourceOnZ(): ");
        Serial.println(accel.isActivitySourceOnZ(), DEC);
    }

    if (accel.isTapSourceOnX()) {
        Serial.print("accel.isTapSourceOnX(): ");
        Serial.println(accel.isTapSourceOnX(), DEC);
    }

    if (accel.isTapSourceOnY()) {
        Serial.print("accel.isTapSourceOnY(): ");
        Serial.println(accel.isTapSourceOnY(), DEC);
    }

    if (accel.isTapSourceOnZ()) {
        Serial.print("accel.isTapSourceOnZ(): ");
        Serial.println(accel.isTapSourceOnZ(), DEC);
    }

    //delay(500);

//    memset(&x, 0x00, sizeof(x));
//    memset(&y, 0x00, sizeof(y));
//    memset(&z, 0x00, sizeof(z));

/*
    accel.readAccel(&x, &y, &z);

    Serial.print("XYZ COUNTS: ");
    Serial.print(x, DEC);
    Serial.print(" ");
    Serial.print(y, DEC);
    Serial.print(" ");
    Serial.print(z, DEC);
    Serial.print(".");
*/

    accel.get_Gxyz(xyz);

    Serial.print("XYZ Gs: ");
    for(i = 0; i<3; i++){
        Serial.print(xyz[i], DEC);
        Serial.print(" ");
    }
    Serial.println("");
    //Serial.println(findPositionLargest(xyz, 3), DEC); 

    absValue = getHigher(xyz, index, isPlus);

    // Recording :
    // 1. Lock sur une valeur d'un axe supérieure à 0.7
    // 2. On doit rester sur cette position > 0.7 pendant minimum 1 seconde
    // 3. Accélération sur un axe > 1.6
    // 4. Si tjrs sur le meme axe, on lance le record

    // Record mode
    if (absValue > 1.6) {
        if (!isPlus) {
            index *= 2;
        }

        ledOn(Leds[index], RED);
        vibrate(200);

        record(Messages[index]);

        delay(6000);
        stop();

        vibrate(200);

        stripOff();
    // Play mode
    } else if (absValue > 0.7) {
        
    }
}

/*
void cmd_accel() {
    PLN("Accel");
}

void unrecognized(const char *command) {
    PLN("Command not found !");
}
*/
