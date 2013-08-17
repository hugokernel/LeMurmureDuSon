
#include <avr/interrupt.h>
#include "Wire.h"
//#include <ADXL345.h>
//#include <HL1606strip.h>
#include <SerialCommand.h>
#include "init.h"

extern uint8_t Messages[6];
//extern uint8_t Sides[6];
uint8_t Colors[6] = {
    RED, YELLOW, GREEN, TEAL, BLUE, VIOLET
};

extern uint8_t Leds[6];

//#define P   Serial.print
//#define PLN Serial.println
//SerialCommand sCmd;

//uint8_t pos_current_side = SIDE_UNKNOW;

//uint8_t pos_counter = 0;
//bool pos_validated = false;

//uint8_t pos_shaked_counter = 0;
//bool pos_shaked = false;

//cube_pos_t cpos; // = cube_pos();

/*
inline void pos_reset() {
    //cpos = cube_pos();
    /*
    cpos.counter = 0;
    cpos.validated = false;
    cpos.shaked_counter = 0;
    cpos.shaked = false;
    cpos.current_side = SIDE_UNKNOW;
    //
}
*/

void cmd_play();
void cmd_record();
void unrecognized(const char *);

SerialCommand sCmd;

HL1606strip strip = HL1606strip(STRIP_D, STRIP_L, STRIP_C, 6);
ADXL345 accel = ADXL345();

Mds mds(strip, accel);

//ADXL345 accele = ADXL345();

void setup()
{
    Serial.begin(9600);

    delay(1500);
    sCmd.setDefaultHandler(unrecognized);
    sCmd.addCommand("play",     cmd_play);
    sCmd.addCommand("record",   cmd_record);
    sCmd.addCommand("stop",     cmd_stop);
    sCmd.addCommand("config",   cmd_config);
 
    mds.init();

/*
    delay(2000);
    Serial.print("Record...");
    mds.record(M8);
    delay(6000);
    mds.stop();
    Serial.print("Stop");
    delay(3000);
*/

/*
    Serial.println("Hello !");
    mds.play(M8);
    mds.rainbowParty(500);
    mds.stop();
*/

    mds.ledsOff();

    //pos_reset();

    interrupts(); // enable all interrupts
    return;

    // initialize timer1
    noInterrupts(); // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;

    TCNT1 = 34286; // preload timer 65536-16MHz/256/2Hz
    TCCR1B |= (1 << CS12); // 256 prescaler
    TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
}

//ISR(TIMER1_OVF_vect)
void paf()
{

    //TCNT1 = 0; // reset timer ct to 130 out of 255
    //TCNT1 = 34286; // preload timer
    TCNT1 = 0; // preload timer
    //TIFR1 = 0x100; // timer2 int flag reg: clear timer overflow flag
}

//int x, y, z, i;
//char data[3];

/* M1 : OK
 * M2 : OK
 * M3, A6 : BAD
 * M4, A7 : BAD
 */
#define TESTPAD M1

/*
 *
 * M1, M2, M5, M6, M7, M8
 * 0, 3, 4, 5 : M1, M6, M7, M8
 */

void loop() {
/*
    static bool processed = false;
    //cube_pos_t cpos;

    pinMode(TESTPAD, OUTPUT);

    digitalWrite(TESTPAD, HIGH);
    delay(2000);
    digitalWrite(TESTPAD, LOW);
    delay(2000);

    return;
*/
    //Serial.print(".");

    //event_t ev;
    //mds.detectEvent(ev);
    mds.detectEvent();

    sCmd.readSerial();
    delay(1);

    //P(".");
    //return;
    /*
    Serial.print(", out: ");
    Serial.println(ev.validated);
*/
    delay(1);

    //Serial.print("Processed : ");
    //Serial.println(ev.processed);

//    return;

    //paf();

    //paf(cpos);

//    mds.accel.get_Gxyz(mds.position.xyz);
//    return;

    // Red if charge in progress
    //if (READ(CHG)) {
    //    mds.ledsColor(RED);
    //}

    if (!mds.event.processed) {

        // Play mode
        if (mds.event.validated) {

            Serial.print("Play message #");
            Serial.println(mds.event.current_side);

            mds.ledsOff();

            // Side led on, play !
            mds.ledOn(Leds[mds.event.current_side], Colors[mds.event.current_side]);

            mds.play(mds.event.current_side);

            mds.event.processed = true;
        }
    } else {
        // Record mode
        //if (mds.event.shaked) {
        if (mds.position.largest > 1.5) {

            /*
            Serial.print("M3:");
            Serial.print(M3);
            Serial.print(", Messages[2]:");
            Serial.println(Messages[2]);
            */

            Serial.print("Record message #");
            Serial.print(mds.event.current_side);

            // Side led on and vibrate !
            mds.ledOn(Leds[mds.event.current_side], RED);
            //mds.vibrate(200);

            // Start record
            mds.record(mds.event.current_side);

            // 6 seconds later, stop !
            delay(mds.config.msgDuration);
            mds.stop();

            Serial.println(" Stop !");

            // Vibrate, led off !
            //mds.vibrate(200);
            mds.ledsOff();

            mds.event.processed = false;
        }
    }
}

/*
    mds.accel.getInterruptSource();

    if (mds.accel.isActivitySourceOnX()) {
        Serial.print("isActivitySourceOnX(): ");
        Serial.println(mds.accel.isActivitySourceOnX(), DEC);
    }

    if (mds.accel.isActivitySourceOnY()) {
        Serial.print("accel.isActivitySourceOnY(): ");
        Serial.println(mds.accel.isActivitySourceOnY(), DEC);
    }

    if (mds.accel.isActivitySourceOnZ()) {
        Serial.print("mds.accel.isActivitySourceOnZ(): ");
        Serial.println(mds.accel.isActivitySourceOnZ(), DEC);
    }

    if (mds.accel.isTapSourceOnX()) {
        Serial.print("mds.accel.isTapSourceOnX(): ");
        Serial.println(mds.accel.isTapSourceOnX(), DEC);
    }

    if (mds.accel.isTapSourceOnY()) {
        Serial.print("mds.accel.isTapSourceOnY(): ");
        Serial.println(mds.accel.isTapSourceOnY(), DEC);
    }

    if (mds.accel.isTapSourceOnZ()) {
        Serial.print("mds.accel.isTapSourceOnZ(): ");
        Serial.println(mds.accel.isTapSourceOnZ(), DEC);
    }

    //delay(500);
*/

/*
    mds.accel.readAccel(&x, &y, &z);
    Serial.print("XYZ COUNTS: ");
    Serial.print(x, DEC);
    Serial.print(" ");
    Serial.print(y, DEC);
    Serial.print(" ");
    Serial.print(z, DEC);
    Serial.print(".");
*/

/*
    Serial.print("XYZ Gs: ");
    for(i = 0; i<3; i++){
        Serial.print(mds.xyz[i], DEC);
        Serial.print(" ");
    }
    Serial.println("");
    //Serial.println(findPositionLargest(xyz, 3), DEC); 

    absValue = getHigher(mds.xyz, index, sign);
*/

void cmd_play() {
    char *arg;
    unsigned int index = 0;

    arg = sCmd.next();
    index = atoi(arg);
    if (index < 0 || index >= sizeof(Messages)) {
        PLN("Index range : 0...7");
    } else {
        P("Play ");
        P(index);

        mds.play(index);
        //delay(6000);
        //mds.stop();

        PLN("...Stop");
    }
}

void cmd_stop() {
    mds.stop();
    PLN(" Stop");
}

void cmd_record() {
    char *arg;
    unsigned int index = 0;
    uint8_t i = 0;

    arg = sCmd.next();
    index = atoi(arg);
    if (index < 0 || index >= sizeof(Messages)) {
        PLN("Index range : 0...7");
    } else {
        P("Record ");
        P(index);
        P(" : ");

        for (i = 3; i > 0; i--) {
            P(" ");
            P(i);
            delay(1000);
        }

        P(" : Speak !");
       
        mds.record(index);
        delay(mds.config.msgDuration);
        mds.stop();

        PLN("...Stop");
    }
}

void cmd_config() {
    char *arg;
    arg = sCmd.next();
    if (!strcmp(arg, "read")) {
        PLN("+----------------+");
        PLN("| Murmure Du Son |");
        PLN("+----------------+");
        P("version     : ");
        PLN(mds.config.version);
        P("msgDuration : ");
        PLN(mds.config.msgDuration);
    } else if (!strcmp(arg, "default")) {
        strcat(mds.config.version, MDS_VERSION);
        mds.config.msgDuration = MSG_DURATION;
        mds.config.debugMode = DEBUG_MODE;
        mds.saveConfig();
        PLN("Default config saved !");
    } else if (!strcmp(arg, "set")) {
        arg = sCmd.next();

        if (!strcmp(arg, "msgDuration")) {
            arg = sCmd.next();
            mds.config.msgDuration = atoi(arg);
            mds.saveConfig();
            PLN("msgDuration saved !");
        }
    } else {
        PLN("Arg not found !\n\r - config [read|default|set]");
    }
}

void unrecognized(const char *command) {
    PLN("Command not found !");
}

