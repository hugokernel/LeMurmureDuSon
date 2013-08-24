
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

void cmd_play();
void cmd_record();
void unrecognized(const char *);

SerialCommand sCmd;

HL1606strip strip = HL1606strip(STRIP_D, STRIP_L, STRIP_C, 6);
ADXL345 accel = ADXL345();

Mds mds(strip, accel);

volatile bool tick = false;

void setup()
{
    Serial.begin(9600);

    delay(1500);
    sCmd.setDefaultHandler(unrecognized);
    sCmd.addCommand("play",     cmd_play);
    sCmd.addCommand("record",   cmd_record);
    sCmd.addCommand("stop",     cmd_stop);
    sCmd.addCommand("config",   cmd_config);
    sCmd.addCommand("sensor",   cmd_sensor);

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

double data[10];
uint8_t data_index = 0;
double result = 0;

void loop() {

    //static bool chargingHandled = false;

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

    if (mds.isCharging()) {
        mds.rainbowParty(500);
        return;
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
            //Serial.print(mds.position.largest);
            Serial.print("Play message #");
            Serial.println(mds.event.current_side);

            mds.ledsOff();

            // Side led on, play !
            mds.ledOn(Leds[mds.event.current_side], Colors[mds.event.current_side]);

            mds.play(mds.event.current_side);

            mds.event.processed = true;
        }
    } else {

        if (mds.event.shaked) {
            if (mds.isRecording()) {
                Serial.print("Stop recording !");

                mds.stop();
            } else {
                mds.vibrate(50);

                Serial.print("Record message #");
                Serial.print(mds.event.current_side);

                mds.ledOn(Leds[mds.event.current_side], RED);

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

void cmd_sensor() {
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

        /*
        paf();
        delay(1000);
        continue;
        */

        P("Charger ");
        PLN(READ(CHG));

        MagnetometerRaw raw = mds.compass.ReadRawAxis();
        heading = atan2(raw.YAxis, raw.XAxis);

        // Correct for when signs are reversed.
        if (heading < 0) {
            heading += 2 * PI;
        }

        P("Magnetometer : ");
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
        P("Accelerometer x:");
        P(data[0]);
        P(", y:");
        P(data[1]);
        P(", z:");
        PLN(data[2]);

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

