
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "Wire.h"
//#include <ADXL345.h>
//#include <HL1606strip.h>
#include <SerialCommand.h>
#include "init.h"
//#include "HL1606stripPWM.h"

#define LED_COUNT 6

//#define MUTE

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

extern uint8_t Leds[6];

void cmd_led();
void cmd_play();
void cmd_record();
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
    P("Play message #");
    PLN(mds.event.current_side);

    //mds.ledsColor(WHITE);
    setLeds(WHITE);

    // Side led on, play !
    //mds.ledOn(Leds[mds.event.current_side], Colors[mds.event.current_side]);
    //mds.ledOn(Leds[mds.event.current_side], BLUE);
    setLed(Leds[mds.event.current_side], BLUE);
}

void crecord(uint8_t index) {
    mds.vibrate(150);
    delay(500);
    mds.vibrate(150);
    delay(500);
    mds.vibrate(150);
    delay(500);

    Serial.print("Record message #");
    Serial.print(mds.event.current_side);

    ledsOff();
    setLed(Leds[mds.event.current_side], RED);
}

void cstop() {
    //mds.ledsColor(WHITE);
    setLeds(WHITE);
}

void setup()
{
    Serial.begin(9600);

    //delay(1500);
    sCmd.setDefaultHandler(unrecognized);
    sCmd.addCommand("help",     cmd_help);
    sCmd.addCommand("led",      cmd_led);
    sCmd.addCommand("play",     cmd_play);
    sCmd.addCommand("record",   cmd_record);
    sCmd.addCommand("stop",     cmd_stop);
    sCmd.addCommand("config",   cmd_config);
    sCmd.addCommand("info",     cmd_info);
    sCmd.addCommand("demo",     cmd_demo);
    sCmd.addCommand("color",    cmd_color);
    sCmd.addCommand("fstatus",  cmd_fstatus);
    sCmd.addCommand("mute",     cmd_mute);
    //sCmd.addCommand("vibrate",  cmd_vibrate);


    //Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, STRIP_C, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

/*
    strip.setPixelColor(0, WHITE);
    strip.setPixelColor(1, RED);
    strip.setPixelColor(2, BLUE);
    strip.show();
//    delay(2000);

    //while(1);
*/
/*
    delay(2000);
    PLN("START");

    while (1) {
        Serial.print("[");
        for(uint16_t i=0; i<strip.numPixels(); i++) {
        //for(uint16_t i=0; i< 60; i++) {
            strip.setPixelColor(i, RED);
            strip.show();
            Serial.print(i);
            Serial.print(' ');
        }
        Serial.print("]");
    }
*/

    mds.init();

#ifdef MUTE
    mds.mute(true);
#endif

    /*
    while (1) {
        mds.vibrate(1000);
        delay(3500);
    }
    */


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

    //mds.ledsOff();
    //mds.ledsColor(WHITE);
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
}

ISR(TIMER1_OVF_vect)
{
    //TCNT1 = 0; // reset timer ct to 130 out of 255
    //TCNT1 = 34286; // preload timer
    TCNT1 = 0; // preload timer
    //TIFR1 = 0x100; // timer2 int flag reg: clear timer overflow flag

    tick = true;
}

/************************** Pixel routine */
// this code is from http://www.bliptronics.com Ben Moyes's example code for LEDs, check them out!
/*
// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}


//Input a value 0 to 127 to get a color value.
//The colours are a transition r - g -b - back to r
unsigned int Wheel(byte WheelPos)
{
  byte r,g,b;
  switch(WheelPos >> 5)
  {
    case 0:
      r=31- WheelPos % 32;   //Red down
      g=WheelPos % 32;      // Green up
      b=0;                  //blue off
      break;
    case 1:
      g=31- WheelPos % 32;  //green down
      b=WheelPos % 32;      //blue up
      r=0;                  //red off
      break;
    case 2:
      b=31- WheelPos % 32;  //blue down
      r=WheelPos % 32;      //red up
      g=0;                  //green off
      break;
  }
  return(Color(r,g,b));
}
*/

double data[10];
uint8_t data_index = 0;
double result = 0;
bool lastChargingStatus = false;

void loop() {

    //static bool chargingHandled = false;

    //mds.accel.printAllRegister();
    //while(1);

    //mds.ledsColor(WHITE);

    while (!tick) {
        sCmd.readSerial();
        delay(1);
    }

    // Stop timer 1 overflow
    TIMSK1 ^= (1 << TOIE1);

/*
    data[data_index] = mds.position.largest;

/*
Serial.print("[");
Serial.print(mds.position.largest);
Serial.print(", ");
Serial.print(data[data_index]);
Serial.print("]");
*-/

    data_index++;
    if (data_index >= sizeof(data)) {
        data_index = 0;
    }

    result = 0;
    for (int i = 0; i < 10; i++) {
        result += data[i];
//        Serial.print(data[i]);
//        Serial.print(", ");
    }

    //result /= 10;

    Serial.print(result);
    Serial.println();
*/
    // Red if charge in progress
    //if (READ(CHG)) {
    //    mds.ledsColor(RED);
    //}

    // Led on ?
    if (ledStatus && ledDelayOn++ > 400) {
        fadeOut();
    }

    // Test battery
    switch (mds.getBatteryState()) {
        case BATTERY_STATE_FULL:
        case BATTERY_STATE_LOW:
            break;
        case BATTERY_STATE_CRITICAL:
            ledsOff();
            PLN("Charge me !");
            delay(500);
            break;
        case BATTERY_STATE_STOP:
            fadeOut();
            PLN("Power down in 2 seconds !");
            delay(2000);
            while(1) {
                P("Power down");
                delay(1000);
            }
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            cli();
            sleep_enable();
            //sleep_bod_disable();
            return;
    }

    if (mds.isOnCharger()) {

        if (!lastChargingStatus) {
            if (mds.isCharging() && mds.getBatteryState() <= BATTERY_STATE_LOW) {
                fadeIn(RED);
            } else {
                fadeIn(WHITE);
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

                Serial.print("Record message #");
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

void cmd_led() {
    char *arg;
    unsigned int index, color = 0;

    arg = sCmd.next();
    if (!strcmp(arg, "set")) {
        arg = sCmd.next();
        index = atoi(arg);
        if (index < 0 || index >= sizeof(Leds)) {
            PLN("Index range : 0...7");
        } else {
            P("Set led ");
            P(index);

            arg = sCmd.next();
            color = atoi(arg);
            if (index < 0 || index >= sizeof(Colors)) {
                PLN("Color range : 0...5");
            return;
            }

            P(" to color ");
            P(color);

            setLed(Leds[index], Colors[color]);

            PLN("");
        }
    } else if (!strcmp(arg, "all")) {
        arg = sCmd.next();
        color = atoi(arg);
        if (index < 0 || index >= sizeof(Colors)) {
            PLN("Color range : 0...5");
            return;
        }

        P("Set all led to color ");
        P(color);

        //mds.ledsColor(Colors[color]);
        setLeds(Colors[color]);

        PLN("");
    } else if (!strcmp(arg, "off")) {
        
        fadeOut();
        PLN("Leds off !");

    } else if (!strcmp(arg, "fade")) {
       
        P("Fade leds ");
        P(index);

        arg = sCmd.next();
        color = atoi(arg);
        if (index < 0 || index >= sizeof(Colors)) {
            PLN("Color range : 0...5");
            return;
        }

        P(" to color ");
        PLN(color);

        fadeOut();
        fadeIn(Colors[color]);

    } else if (!strcmp(arg, "bright")) {
        arg = sCmd.next();
        color = atoi(arg);

        P("Set leds brightness to ");
        PLN(color);

        mds.ledsBrightness(color);

    } else if (!strcmp(arg, "map")) {

        arg = sCmd.next();
        if (!strcmp(arg, "get")) {
            arg = sCmd.next();
        
            P("Mapping : ");

            for (int i = 0; i < sizeof(Leds); i++) {
                P(Leds[i]);
            }

            PLN("");
        
        } else if (!strcmp(arg, "set")) {
            arg = sCmd.next();
            //index = atoi(arg);

            for (int i = 0; i < strlen(arg); i++) {
                mds.config.ledMapping[i] = arg[i] - ((int)'0');
                Leds[i] = mds.config.ledMapping[i];
            }

            mds.saveConfig();
            P("New mapping set to ");
            PLN(arg);
        }
    }
}

void cmd_color() {
    char *arg;
    unsigned int index, color = 0;

    arg = sCmd.next();
    color = atoi(arg);
    if (index < 0 || index >= sizeof(Colors)) {
        PLN("Color range : 0...5");
    }

    P("Set color ");// (RED, YELLOW, GREEN, TEAL, BLUE, VIOLET,  WHITE, BLACK) ");
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
    int i = 0;
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
        P("ledMapping  : ");
        for (i = 0; i < sizeof(mds.config.ledMapping); i++) {
            P((char)(mds.config.ledMapping[i] + ((int)'0')));
        }
        PLN("");
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
        } else {
            PLN("Unknow var !");
        }
    } else {
        PLN("Arg not found !\n\r - config [read|default|set]");
    }
}

void cmd_info() {
    double data[3];
    int error = 0;
    float headingDegrees, heading;

    error = mds.compass.SetScale(1.3);
    if (error != 0) {
        //Serial.println(mds.compass.GetErrorText(error));
    }

    error = mds.compass.SetMeasurementMode(Measurement_Continuous);
    if (error != 0) {
        //Serial.println(mds.compass.GetErrorText(error));
    }


    while (true) {

        while (Serial.available() > 0) {
            if (Serial.read()) {
                return;
            }
        }

        PLN("[Power]");
        P(" Battery voltage : ");
        PLN(mds.getBatteryVoltage());

        P(" Battery status : ");
        switch (mds.getBatteryState()) {
            case BATTERY_STATE_FULL:
                PLN("Full");
                break;
            case BATTERY_STATE_LOW:
                PLN("Low");
                break;
            case BATTERY_STATE_CRITICAL:
                PLN("Critical");
                break;
            case BATTERY_STATE_STOP:
                PLN("Stop");
                break;
        }

        P(" Charge : ");
        if (mds.isOnCharger()) {
            if (!mds.isCharging()) {
                PLN("Standby");
            } else {
                PLN("In progress");
            }
        } else {
            PLN("None");
        }

        P(" On charger : ");
        if (mds.isOnCharger()) {
            PLN("True");
        } else {
            PLN("False");
        }

        P(" Charger voltage : ");
        PLN(mds.getChargerVoltage());

        MagnetometerRaw raw = mds.compass.ReadRawAxis();
        heading = atan2(raw.YAxis, raw.XAxis);

        // Correct for when signs are reversed.
        if (heading < 0) {
            heading += 2 * PI;
        }

        PLN("\n[Sensor]");
        P(" Magnetometer : ");
        P(heading * 180 / M_PI);
        PLN(" degree");

/*
        P("Magnetometer x:");
        P(raw.XAxis);
        P(", y:");
        P(raw.YAxis);
        P(", z:");
        PLN(raw.ZAxis);
*/

        accel.get_Gxyz(data);
        P(" Accelerometer x:");
        P(data[0]);
        P(", y:");
        P(data[1]);
        P(", z:");
        PLN(data[2]);

        P(" Temperature : ");
        PLN(mds.getTemperature());

        PLN("--");

        delay(1000);
    }

    PLN("End");
}

void unrecognized(const char *command) {
    PLN("Command not found !");
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
        //PLN("RainbowParty demo");
        while (true) {
            mds.rainbowParty(500);

            while (Serial.available() > 0) {
                if (Serial.read()) {
                    return;
                }
            }
        }
    } else if (index == 1) {
        //PLN("Brightness demo");
        mds.ledsColor(WHITE);

        while (true) {
            mds.ledsBrightness(c);
            delay(5);

            while (Serial.available() > 0) {
                if (Serial.read()) {
                    return;
                }
            }

            if ((!upper && c == 1) || c == 255) {
                upper = !upper;
            }
            c = (upper) ? c + 1 : c - 1;
        }
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

    PLN("Status (0:empty, 1:full, 2:read)");

    if (index < 0 || index > 5) {
        PLN("Index range : 0...6");
        return;
    }

    arg = sCmd.next();
    status = atoi(arg);
    if (index < 0 || index > 2) {
        PLN("Index range : 0...6");
        return;
    }

    mds.status[index] = (Status)status;

    P("Face ");
    P(mds.status[index]);
    P(" set to ");
    PLN(status);
}

void cmd_mute() {
    bool mute = 0;
    char *arg;

    arg = sCmd.next();

    mute = atoi(arg);
    mds.mute((bool)mute);

    P("Mute : ");
    PLN(mute);
}

void cmd_help() {
    PLN("Command list :");
    PLN("- led");
    PLN("- play");
    PLN("- record");
    PLN("- stop");
    PLN("- config");
    PLN("- info");
    PLN("- demo");
    PLN("- fstatus");
    PLN("- mute");
}

