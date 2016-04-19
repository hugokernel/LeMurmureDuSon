
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "Wire.h"
//#include <ADXL345.h>
//#include <HL1606strip.h>
#include <SerialCommand.h>
#include "init.h"
//#include "HL1606stripPWM.h"

#define LED_COUNT 6

//#define MUTE

typedef struct {
    char* command;
    void (*function)();
    char* description;
} command;

#ifdef HL1606
HL1606strip strip = HL1606strip(STRIP_D, STRIP_L, STRIP_C, LED_COUNT);

#define BLACK   0b000
#define WHITE   0b111
#define RED     0b100
#define YELLOW  0b101
#define GREEN   0b001
#define TEAL    0b011
#define BLUE    0b010
#define VIOLET  0b110

//HL1606stripPWM strippwm = HL1606stripPWM(LED_COUNT, STRIP_L);

// Last led mapping : 125304

uint8_t Colors[] = {
    RED, YELLOW, GREEN, TEAL, BLUE, VIOLET, WHITE, BLACK
};
#else
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, STRIP, NEO_GRB + NEO_KHZ800);

#define BLACK   strip.Color(0, 0, 0)
#define WHITE   strip.Color(255, 255, 255)
#define RED     strip.Color(255, 0, 0)
#define YELLOW  strip.Color(255, 255, 0)
#define GREEN   strip.Color(0, 255, 0)
#define TEAL    strip.Color(2, 132, 130)
#define BLUE    strip.Color(0, 0, 255)
#define VIOLET  strip.Color(102, 51, 153)

uint32_t Colors[] = {
    RED, YELLOW, GREEN, TEAL, BLUE, VIOLET, WHITE, BLACK
};
#endif

extern uint8_t Messages[8];
//extern uint8_t Sides[6];

#define MSG_HELLO      6 
#define MSG_CHARGE_OK  7

extern uint8_t Leds[6];

void cmd_help();
void cmd_led();
void cmd_play();
void cmd_record();
void cmd_stop();
void cmd_config();
void cmd_info();
void cmd_demo();
void cmd_color();
void cmd_fstatus();
void cmd_mute();
void cmd_poweron();
void cmd_poweroff();

void unrecognized(const char *);

SerialCommand sCmd;

ADXL345 accel = ADXL345();

Mds mds(strip, accel);

volatile bool tick = false;

uint8_t pattern_demo_rainbow[] = {
    2, 4, 2, 4, 2
};

uint8_t pattern_demo_fade[] = {
    2, 3, 2, 3, 2
};

#define LED_STATUS_ON   ledDelayOn = 0; ledStatus = true;
#define LED_STATUS_OFF  ledDelayOn = 0; ledStatus = false;

unsigned int ledDelayOn = 0;
bool ledStatus = false;

void setLed(uint8_t index, uint32_t color) {
    mds.ledsBrightness(255);
    mds.ledOn(index, color);

    LED_STATUS_ON
}

void setLeds(uint32_t color) {
    mds.ledsBrightness(255);
    mds.ledsColor(color);

    LED_STATUS_ON
}

void fadeIn(uint32_t color) {
    setLeds(color);
    for (uint8_t c = 1; c < 255; c++) {
        mds.ledsBrightness(c);
        delay(2);
    }
}

void fadeOut() {
    for (uint8_t c = 255; c > 0; c--) {
        mds.ledsBrightness(c);
        delay(2);
    }
    mds.ledsBrightness(0);
    ledStatus = false;
}

void ledsOff() {
    mds.ledsOff();

    LED_STATUS_OFF
}

void cplay(uint8_t index) {
    //Serial.print(mds.position.largest);
    PPSTR("Play msg #");
    PLN(index);

    //mds.ledsColor(WHITE);
    setLeds(WHITE);

    // Side led on, play !
    //mds.ledOn(Leds[mds.event.current_side], Colors[mds.event.current_side]);
    //mds.ledOn(Leds[mds.event.current_side], BLUE);
    setLed(Leds[index], BLUE);
}

void crecord(uint8_t index) {
    ledsOff();

    if (index < LED_COUNT) {
        setLed(Leds[index], RED);
//        mds.ledsBrightness(5);
    }
}

void cstop() {
    //mds.ledsColor(WHITE);
    setLeds(WHITE);
}

void powerOff(void) {
    // Todo: Stop Watchdog
    fadeOut();
    PPSTRLN("Power down !");
    wdt_reset();
    delay(2000);
    PPSTRLN("Suicide !");
    wdt_reset();
    delay(200);
    mds.powerOff();
    wdt_reset();
    delay(2000);
}

void breathing(uint8_t cycle, uint8_t _delay) {
    uint8_t c = 1;
    uint8_t upper = true;
    for (; cycle > 0; cycle--) {
        for (uint8_t i = 0; i < 255; i++) {
            mds.ledsBrightness(c);
            delay(_delay);

            wdt_reset();

            if ((!upper && c == 1) || c == 255) {
                upper = !upper;
            }
            c = (upper) ? c + 1 : c - 1;
        }
    }
}

void watchdogOn() {
    // Clear the reset flag, the WDRF bit (bit 3) of MCUSR.
    MCUSR = MCUSR & B11110111;
      
    // Set the WDCE bit (bit 4) and the WDE bit (bit 3) 
    // of WDTCSR. The WDCE bit must be set in order to 
    // change WDE or the watchdog prescalers. Setting the 
    // WDCE bit will allow updtaes to the prescalers and 
    // WDE for 4 clock cycles then it will be reset by 
    // hardware.
    WDTCSR = WDTCSR | B00011000; 

    // Set the watchdog timeout prescaler value to 1024 K 
    // which will yeild a time-out interval of about 8.0 s.
    WDTCSR = B00100001;

    // Enable the watchdog timer interupt.
    WDTCSR = WDTCSR | B01000000;
    MCUSR = MCUSR & B11110111;
}

command commands[] = {
    { "help",       cmd_help,       "Help !" },
    { "?",          cmd_help,       "Help !" },

    { "led",        cmd_led,        "Power on led" },
    { "play",       cmd_play,       "Play sound" },
    { "record",     cmd_record,     "Record sound" },
    { "stop",       cmd_stop,       "Stop" },
    { "config",     cmd_config,     "Read config" },
    { "info",       cmd_info,       "Get info" },
    { "demo",       cmd_demo,       "Run demo" },
    { "color",      cmd_config,     "Color" },
    { "fstatus",    cmd_fstatus,    "Set face" },
    { "mute",       cmd_mute,       "Toggle mute" },
    { "bug",        cmd_bug,        "Test" },
    { "on",         cmd_poweron,    "Power on" },
    { "off",        cmd_poweroff,   "Power off" }
};

void cmd_help() {
    PPSTRLN("Commands list :");
    for (uint8_t i = 0; i < sizeof(commands) / sizeof(command); i++) {
        PPSTR("- ");
        P(commands[i].command);
        PPSTR(" ");
        PLN(commands[i].description);
    }
}
#include <avr/power.h>
#include <avr/sleep.h>
void setup()
{
    Serial.begin(9600);

    sCmd.setDefaultHandler(unrecognized);
    for (uint8_t i = 0; i < sizeof(commands) / sizeof(command); i++) {
        sCmd.addCommand(commands[i].command, commands[i].function);
    }

    //Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, STRIP_C, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    mds.init();

#ifdef MUTE
    mds.mute(true);
#endif

    ledsOff();

    // Battery save !
    if (mds.getBatteryState() > BATTERY_STATE_LOW) {
        setLeds(WHITE);
    }

    mds.setCallbacks(cplay, crecord, cstop);
/*
    strippwm.setPWMbits(5);
    strippwm.setSPIdivider(32);
    strippwm.setCPUmax(80);
*/

    //pos_reset();

//    interrupts(); // enable all interrupts
//    return;

    // initialize timer1
    noInterrupts(); // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;

    TCNT1 = 0;//34286; // preload timer 65536-16MHz/256/2Hz
    TCCR1B |= (1 << CS11); // 256 prescaler
    TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt

    interrupts(); // enable all interrupts

    mds.play(MSG_HELLO);
    delay(2000);

    //wdt_enable(WDTO_2S);
    watchdogOn();

    PPSTRLN("Starting sketch !");
}

ISR(TIMER1_OVF_vect)
{
    //TCNT1 = 0; // reset timer ct to 130 out of 255
    //TCNT1 = 34286; // preload timer
    TCNT1 = 0; // preload timer
    //TIFR1 = 0x100; // timer2 int flag reg: clear timer overflow flag

    tick = true;
}

double data[10];
uint8_t data_index = 0;
double result = 0;

bool lastChargingStatus = false;
uint8_t statusWhenStandOnCharger = BATTERY_STATE_UNKNOW;

int x, y, z;
void loop() {

    while (!tick) {
        sCmd.readSerial();
        delay(1);
    }

    // Stop timer 1 overflow
    TIMSK1 ^= (1 << TOIE1);

    // Led on ?
    if (ledStatus && ledDelayOn++ > 400) {
        fadeOut();
    }
    
    if (!mds.isOnCharger() && !ledStatus && ledDelayOn++ > 4000) {
        PPSTRLN("Off");
        powerOff();
        while(1);
    }

    // Test battery
    switch (mds.getBatteryState()) {
        case BATTERY_STATE_FULL:
        case BATTERY_STATE_LOW:
            break;
        case BATTERY_STATE_CRITICAL:
            ledsOff();
            PPSTRLN("Battery voltage critical : Charge me !");
            delay(500);
            break;
        case BATTERY_STATE_STOP:
            powerOff();
            return;
    }

    if (mds.isOnCharger()) {

        // Cube just be on the charger
        if (!lastChargingStatus) {
            statusWhenStandOnCharger = mds.getBatteryState();
            PPSTR("Cube just be on charger, status : ");
            PLN(statusWhenStandOnCharger);
            if (mds.isCharging() && statusWhenStandOnCharger <= BATTERY_STATE_LOW) {
                fadeIn(RED);
            } else {
                fadeIn(WHITE);
                //breathing(10, 3);
                //fadeIn(WHITE);
            }
        }

        lastChargingStatus = true;

        // Reset event
        //mds.event.processed = false;
        goto lesgotocaimal;
    } else {
        // If last time, charger is in progress
        if (lastChargingStatus) {
            fadeOut();

            if (statusWhenStandOnCharger < BATTERY_STATE_FULL) { 
                mds.play(MSG_CHARGE_OK);
            }
        }

        lastChargingStatus = false;
    }

    mds.detectEvent();

    if (!mds.event.processed) {

        /*
        if (mds.isRecording()) {
            mds.event.processed = true;
//Serial.print("skip");
            return;
        }
        */

        // Play mode
        if (mds.event.validated) {

            if (mds.isRecording()) {
                Serial.print("Stop recording !");
                mds.stop();
            }

            mds.play(mds.event.current_side);

            mds.event.processed = true;
        }
    } else {

        if (mds.event.shaked) {
            if (mds.isRecording()) {
                Serial.print("Stop recording !");

                mds.stop();
            } else {

                mds.vibrate(150);
                delay(500);
                mds.vibrate(150);
                delay(500);
                mds.vibrate(150);
                delay(500);

                Serial.print("Record msg #");
                Serial.print(mds.event.current_side);

                mds.record(mds.event.current_side);

                // 6 seconds later, stop !
                //delay(mds.config.msgDuration);
            }

            mds.event.shaked = false;
            mds.event.shaked_counter = 0;
            //mds.event.processed = true;
        }

        /*
        // Record mode
        if (mds.event.shaked) {
Serial.print("shaked");
            if (mds.isRecording()) {
                mds.stop();

                mds.resetEvent();
                mds.event.processed = true;

                Serial.println(" Stop !");

                // Vibrate, led off !
                //mds.vibrate(200);
                mds.ledsOff();

                mds.event.processed = false;
            } else {
                P("Shake detected !");
                mds.vibrate(50);

                Serial.print("Record msg #");
                Serial.print(mds.event.current_side);

                // Side led on and vibrate !
                mds.ledOn(Leds[mds.event.current_side], RED);
                //mds.vibrate(200);

                // Start record
                mds.record(mds.event.current_side);

                mds.resetEvent();
                mds.event.processed = true;

                // 6 seconds later, stop !
                //delay(mds.config.msgDuration);
            }
        }
        */
    }

    // Test pattern
    if (mds.testPath(pattern_demo_rainbow, sizeof(pattern_demo_rainbow))) {

        for (int i = 0; i < 10; i++) {
            mds.vibrate(50);
            mds.rainbowParty(100);
        }

        memset(mds.path, 0x00, sizeof(mds.path));
        fadeIn(WHITE);
    } else if (mds.testPath(pattern_demo_fade, sizeof(pattern_demo_fade))) {

        for (int i = 0; i < sizeof(Colors) / sizeof(WHITE); i++) {
            fadeOut();
            fadeIn(Colors[i]);
        }

        memset(mds.path, 0x00, sizeof(mds.path));
        fadeIn(WHITE);
    }


lesgotocaimal:
    wdt_reset();
    tick = false;
    TIMSK1 |= (1 << TOIE1);
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

void cmd_led() {
    char *arg;
    unsigned int index, color = 0;

    arg = sCmd.next();
    if (!strcmp(arg, "set")) {
        arg = sCmd.next();
        index = atoi(arg);
        if (index < 0 || index >= sizeof(Leds)) {
            PPSTRLN("Index range : 0...7");
        } else {
            PPSTR("Set led ");
            P(index);

            arg = sCmd.next();
            color = atoi(arg);
            if (index < 0 || index >= sizeof(Colors)) {
                PPSTRLN("Color range : 0...5");
            return;
            }

            PPSTR(" to color ");
            P(color);

            setLed(Leds[index], Colors[color]);

            PLN("");
        }
    } else if (!strcmp(arg, "all")) {
        arg = sCmd.next();
        color = atoi(arg);
        if (index < 0 || index >= sizeof(Colors)) {
            PPSTRLN("Color range : 0...5");
            return;
        }

        PPSTR("Set all led to color ");
        P(color);

        //mds.ledsColor(Colors[color]);
        setLeds(Colors[color]);
        PLN("");
    } else if (!strcmp(arg, "off")) {
        
        fadeOut();
        PPSTRLN("Leds off !");

    } else if (!strcmp(arg, "fade")) {
       
        PPSTR("Fade leds ");
        P(index);

        arg = sCmd.next();
        color = atoi(arg);
        if (index < 0 || index >= sizeof(Colors)) {
            PPSTRLN("Color range : 0...5");
            return;
        }

        PPSTR(" to color ");
        PLN(color);

        fadeOut();
        fadeIn(Colors[color]);

    } else if (!strcmp(arg, "bright")) {
        arg = sCmd.next();
        color = atoi(arg);

        PPSTR("Set leds brightness to ");
        PLN(color);

        mds.ledsBrightness(color);

    } else if (!strcmp(arg, "map")) {

        arg = sCmd.next();
        if (!strcmp(arg, "get")) {
            arg = sCmd.next();
        
            PPSTR("Mapping : ");

            for (int i = 0; i < sizeof(Leds); i++) {
                P(Leds[i]);
            }

            PLN("");
        
        } else if (!strcmp(arg, "set")) {
            arg = sCmd.next();
            //index = atoi(arg);

            for (int i = 0; i < sizeof(mds.config.ledMapping); i++) {
                mds.config.ledMapping[i] = arg[i] - ((int)'0');
                Leds[i] = mds.config.ledMapping[i];
            }

            mds.saveConfig();
            PPSTR("New mapping set to ");
            PLN(arg);
        }
    } else {
        PPSTRLN("Unknow command !");
        PPSTRLN("Commands : set, all, off, fade, bright, map");
    }
}

void cmd_color() {
    char *arg;
    unsigned int index, color = 0;

    arg = sCmd.next();
    color = atoi(arg);
    if (index < 0 || index >= sizeof(Colors)) {
        PPSTRLN("Color range : 0...5");
    }

    PPSTR("Set color ");// (RED, YELLOW, GREEN, TEAL, BLUE, VIOLET,  WHITE, BLACK) ");
    PLN(color);

    //mds.ledsColor(Colors[color]);
    setLeds(Colors[color]);
}

void cmd_play() {
    char *arg;
    unsigned int index = 0;

    arg = sCmd.next();
    index = atoi(arg);
    if (index < 0 || index >= sizeof(Messages)) {
        PPSTRLN("Index range : 0...7");
    } else {
        PPSTR("Play ");
        P(index);

        mds.play(index);
        //delay(6000);
        //mds.stop();

        PPSTRLN("...Stop");
    }
}

void cmd_stop() {
    mds.stop();
    PPSTRLN(" Stop");
}

void cmd_record() {
    char *arg;
    unsigned int index = 0;
    uint8_t i = 0;

    arg = sCmd.next();
    index = atoi(arg);
    if (index < 0 || index >= sizeof(Messages)) {
        PPSTRLN("Index range : 0...7");
    } else {
        PPSTR("Record ");
        P(index);
        PPSTR(" : ");

        for (i = 3; i > 0; i--) {
            PPSTR(" ");
            P(i);
            wdt_reset();
            delay(1000);
        }

        PPSTR(" : Speak !");

        mds.record(index);

        wdt_reset();
        delay(mds.config.msgDuration);
        mds.stop();

        PPSTRLN("...Stop");
    }
}

void cmd_config() {
    int i = 0;
    char *arg;
    arg = sCmd.next();
    if (!strcmp(arg, "read")) {
        PPSTRLN("Murmure Du Son");
        PPSTR("version     : ");
        PLN(mds.config.version);
        PPSTR("msgDuration : ");
        PLN(mds.config.msgDuration);
        PPSTR("ledMapping  : ");
        for (i = 0; i < sizeof(mds.config.ledMapping); i++) {
            P((char)(mds.config.ledMapping[i] + ((int)'0')));
        }
        PPSTRLN("");
    } else if (!strcmp(arg, "default")) {
        strcat(mds.config.version, MDS_VERSION);
        mds.config.msgDuration = MSG_DURATION;
        mds.config.debugMode = DEBUG_MODE;
        mds.saveConfig();
        PPSTRLN("Default config saved !");
    } else if (!strcmp(arg, "set")) {
        arg = sCmd.next();

        if (!strcmp(arg, "msgDuration")) {
            arg = sCmd.next();
            mds.config.msgDuration = atoi(arg);
            mds.saveConfig();
            PPSTRLN("msgDuration saved !");
        } else {
            PPSTRLN("Unknow var !");
        }
    } else {
        PPSTRLN("Arg not found !\n\r - config [read|default|set]");
    }
}

void cmd_info() {
    double data[3];
    int error = 0;
    float headingDegrees, heading;
    char *arg;
    arg = sCmd.next();

    error = mds.compass.SetScale(1.3);
    if (error != 0) {
        //Serial.println(mds.compass.GetErrorText(error));
    }

    error = mds.compass.SetMeasurementMode(Measurement_Continuous);
    if (error != 0) {
        //Serial.println(mds.compass.GetErrorText(error));
    }

    while (true) {

        if (Serial.read() == 13) {
            break;
        }
        /*
        //while (Serial.available() > 0) {
            if (Serial.read() == '\n') {
                break;
                //return;
            }
        //}
        */

        PPSTRLN("[Power]");
        PPSTR(" Battery : ");
        P(mds.getBatteryVoltage());
        PPSTRLN("V");

        PPSTR(" Status  : ");
        switch (mds.getBatteryState()) {
            case BATTERY_STATE_FULL:
                PPSTRLN("Full");
                break;
            case BATTERY_STATE_LOW:
                PPSTRLN("Low");
                break;
            case BATTERY_STATE_CRITICAL:
                PPSTRLN("Critical");
                break;
            case BATTERY_STATE_STOP:
                PPSTRLN("Stop");
                break;
        }

        PPSTR(" Charge  : ");
        if (mds.isOnCharger()) {
            if (!mds.isCharging()) {
                PPSTRLN("Standby");
            } else {
                PPSTRLN("In progress");
            }
        } else {
            PPSTRLN("None");
        }

        PPSTR(" On charger : ");
        if (mds.isOnCharger()) {
            PPSTRLN("True");
        } else {
            PPSTRLN("False");
        }

        PPSTR(" Charger : ");
        P(mds.getChargerVoltage());
        PPSTRLN("V");

        MagnetometerRaw raw = mds.compass.ReadRawAxis();
        heading = atan2(raw.YAxis, raw.XAxis);

        // Correct for when signs are reversed.
        if (heading < 0) {
            heading += 2 * PI;
        }

        PPSTRLN("\n[Sensor]");
        PPSTR(" Magnetometer : ");
        P(heading * 180 / M_PI);
        PPSTRLN(" degree");

/*
        PPSTR("Magnetometer x:");
        P(raw.XAxis);
        PPSTR(", y:");
        P(raw.YAxis);
        PPSTR(", z:");
        PLN(raw.ZAxis);
*/

        accel.get_Gxyz(data);
        PPSTR(" Accel x:");
        P(data[0]);
        PPSTR(", y:");
        P(data[1]);
        PPSTR(", z:");
        PLN(data[2]);

        PPSTR(" Temp : ");
        PLN(mds.getTemperature());

        if (strcmp(arg, "loop")) {
            break;
        }

        PPSTRLN("--");

        delay(1000);
        wdt_reset();
    }

    PPSTRLN("End");
}

void unrecognized(const char *command) {
    PPSTR("'");
    P(command);
    PPSTRLN("' command not found !");
}

void magnetometerDump()
{
    // Retrive the raw values from the compass (not scaled).
    MagnetometerRaw raw = mds.compass.ReadRawAxis();

    // Retrived the scaled values from the compass (scaled to the configured scale).
    MagnetometerScaled scaled = mds.compass.ReadScaledAxis();

    // Calculate heading when the magnetometer is level, then correct for signs of axis.
    float heading = atan2(raw.YAxis, raw.XAxis);

    // Correct for when signs are reversed.
    if (heading < 0) {
        heading += 2 * PI;
    }

    // Convert radians to degrees for readability.
    float headingDegrees = heading * 180/M_PI;

    // Output the data via the serial port.
    Serial.print("Raw:\t");
    Serial.print(raw.XAxis);
    Serial.print("   ");
    Serial.print(raw.YAxis);
    Serial.print("   ");
    Serial.print(raw.ZAxis);
    Serial.print("   \tScaled:\t");

    Serial.print(scaled.XAxis);
    Serial.print("   ");
    Serial.print(scaled.YAxis);
    Serial.print("   ");
    Serial.print(scaled.ZAxis);

    Serial.print("   \tHeading:\t");
    Serial.print(heading);
    Serial.print(" Radians   \t");
    Serial.print(headingDegrees);
    Serial.println(" Degrees   \t");
}

void cmd_demo() {
    uint8_t index = 0;
    uint8_t c = 1;
    uint8_t upper = true;
    char *arg;
    arg = sCmd.next();
    index = atoi(arg);

    if (index == 0) {
        PPSTR("RainbowParty demo...");
        while (true) {
            mds.rainbowParty(500);

            while (Serial.available() > 0) {
                if (Serial.read()) {
                    PPSTRLN("End !");
                    return;
                }
            }

            wdt_reset();
        }
    } else if (index == 1) {
        PPSTR("Brightness demo...");
        mds.ledsColor(WHITE);

        while (true) {
            mds.ledsBrightness(c);
            delay(5);

            while (Serial.available() > 0) {
                if (Serial.read()) {
                    PPSTRLN("End !");
                    return;
                }
            }

            wdt_reset();

            if ((!upper && c == 1) || c == 255) {
                upper = !upper;
            }
            c = (upper) ? c + 1 : c - 1;
        }
    } else {
        PPSTRLN("Unknow demo ! (type demo 0 or demo 1)");
    }

    delay(1000);
    //mds.ledsOff();
    ledsOff();
}

void cmd_fstatus() {
    uint8_t index, status = 0;
    int i = 0;
    char *arg;
    arg = sCmd.next();
    index = atoi(arg);

    PPSTRLN("Status (0:empty, 1:full, 2:read)");

    if (index < 0 || index > 5) {
        PPSTRLN("Index range : 0...6");
        return;
    }

    arg = sCmd.next();
    status = atoi(arg);
    if (index < 0 || index > 2) {
        PPSTRLN("Index range : 0...6");
        return;
    }

    mds.status[index] = (Status)status;

    PPSTR("Face ");
    P(mds.status[index]);
    PPSTR(" set to ");
    PLN(status);
}

void cmd_mute() {
    bool mute = 0;
    char *arg;

    arg = sCmd.next();

    mute = atoi(arg);
    mds.mute((bool)mute);

    PPSTR("Mute : ");
    PLN(mute);
}

void cmd_bug() {
    PPSTRLN("Bug!");
    while (1);
}

void cmd_poweron() {
    PPSTRLN("Power on !");
    mds.powerOn();
}

void cmd_poweroff() {
    PPSTRLN("Power off !");
    //mds.powerOff();
    powerOff();
}

