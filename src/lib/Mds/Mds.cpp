
#include "Mds.h"

uint8_t Messages[8] = {
    //M1, M2, M3, M4, M5, M6, M7, M8
    M1, M2, M5, M6, M7, M8
};

uint8_t Sides[6] = {
    0, 1, 2, 3, 4, 5
};

/*
uint8_t Colors[6] = {
    RED, YELLOW, GREEN, TEAL, BLUE, VIOLET
};
*/

uint8_t Leds[6] = {
    0, 1, 2, 3, 4, 5
};

volatile bool Mds::_isPlaying = false;
volatile bool Mds::_isRecording = false;
volatile bool Mds::_isBusy = false;

Mds::Mds(HL1606strip _strip, ADXL345 _accel):
    strip(_strip),
    accel(_accel),
    currentMsg(-1),
    lastReadMsg(-1)
{
}

void interruptChg() {
    //mds.ledsColor(RED);
}

void interruptLed() {
    Mds::_isRecording = false;
    Mds::_isPlaying = false;
    Mds::_isBusy = false;

    if (config.debugMode) {
        Serial.println("[End]");
    }
}

void Mds::init(void) {

    loadConfig();

    pinMode(VIBRATOR, OUTPUT);
    pinMode(CHG, INPUT);

    // Set output
    for (uint8_t i = 0; i < sizeof(Messages); i++) {
        pinMode(Messages[i], OUTPUT);
    }

    pinMode(REC_PLAY, OUTPUT);
    pinMode(LED, INPUT);

    DOWN(VIBRATOR);

    msgUnselect();

    UP(REC_PLAY);

    accel.powerOn();

    event = event_t();

    attachInterrupt(0, interruptLed, FALLING);
    attachInterrupt(1, interruptChg, FALLING);

 /*
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

    accel.setAxisOffset(2, 3, 4);
    accel.setActivityThreshold(48);
    accel.setTapDuration(0x30);

    accel.setActivityX(true);
    accel.setActivityY(true);
    accel.setActivityZ(true);
*/
}

/**
 *  From http://playground.arduino.cc/Code/EEPROMLoadAndSaveSettings
 */
void Mds::loadConfig() {
  if (EEPROM.read(CONFIG_START + 0) == MDS_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == MDS_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == MDS_VERSION[2]) {
        for (unsigned int t = 0; t < sizeof(config); t++) {
            *((char*)&config + t) = EEPROM.read(CONFIG_START + t);
        }
    }
}

void Mds::saveConfig() {
    for (unsigned int t = 0; t < sizeof(config); t++) {
        EEPROM.write(CONFIG_START + t, *((char*)&config + t));
    }
}

inline void Mds::msgSelect(uint8_t index) {
    DOWN(Messages[index]);
}

inline void Mds::msgUnselect() {
    for (char i = 0; i < sizeof(Messages); i++) {
        UP(Messages[i]);
    }
}

bool Mds::isPlaying() {
    //return !READ(LED);
    return Mds::_isPlaying;
}

bool Mds::isRecording() {
    return Mds::_isRecording;
}

bool Mds::isBusy() {
    return Mds::_isBusy;
}

bool Mds::record(uint8_t index) {
    DOWN(REC_PLAY);
    // Todo: Sleep for 30ns
    delay(1);
    DOWN(Messages[index]);

    currentMsg = index;

    Mds::_isRecording = true;
    Mds::_isBusy = true;
}

bool Mds::play(uint8_t index) {

    if (Mds::_isPlaying) {
        DOWN(Messages[currentMsg]);
        delay(20);
        UP(Messages[currentMsg]);
        delay(20);
    }

    UP(REC_PLAY);
    // Todo: Sleep for 30ns
    delay(1);
    DOWN(Messages[index]);
    
    // 225k / fs = ~18ms
    delay(20); 
    UP(Messages[index]);

    currentMsg = index;

    Mds::_isPlaying = true;
    Mds::_isBusy = true;
}

bool Mds::stop() {
    msgUnselect();

    currentMsg = -1;
}

void Mds::vibrate(int time) {
    UP(VIBRATOR);
    delay(time);
    DOWN(VIBRATOR);
}

void Mds::ledsOff() {
    uint8_t c = 0;
    for (c = 0; c < strip.numLEDs(); c++) {
        strip.setLEDcolor(c, BLACK);
    }

    strip.writeStrip();
}

void Mds::ledOn(uint8_t index, uint8_t color) {
    strip.setLEDcolor(index, color);
    strip.writeStrip();
}

void Mds::ledsColor(uint8_t color) {
    uint8_t c = 0;

    for (c = 0; c < strip.numLEDs(); c++) {
        strip.setLEDcolor(c, color);
    }

    strip.writeStrip();
}

void Mds::rainbowParty(uint8_t wait) {
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

/*
int Mds::getHigher(double data[], int &index, bool &sign) {
    double largest = abs(data[0]);
    sign = (data[0] > 0);
    for (int i = 0; i< 3; i++) {
        if (largest < abs(data[i])) {
            largest = abs(data[i]);
            sign = (data[i] > 0);
            index = i;
        }
    }

    return largest;
}
*/

int Mds::handlePosition(position_t &pos) {
    pos.largest = abs(pos.xyz[0]);
    pos.sign = (pos.xyz[0] > 0);
    pos.index = 0;
    for (int i = 0; i < 3; i++) {
        if (pos.largest < abs(pos.xyz[i])) {
            pos.largest = abs(pos.xyz[i]);
            pos.sign = (pos.xyz[i] > 0);
            pos.index = i;
        }
    }
}

/*
uint8_t Mds::getUpWardlySide(void) {
    bool sign = false;
    uint8_t side = 0;
    uint8_t absValue = 0;
    int index = 0;

//    readAccel();
//    absValue = getHigher(position.xyz, index, sign);
}
*/

void Mds::processAccel() {
    accel.get_Gxyz(position.xyz);
/*
Serial.print("XYZ COUNTS: ");
Serial.print(position.xyz[0], DEC);
Serial.print(" ");
Serial.print(position.xyz[1], DEC);
Serial.print(" ");
Serial.print(position.xyz[2], DEC);
Serial.println();
*/
    handlePosition(position);

    position.side = position.index;
/*
Serial.print("Position index : ");
Serial.print(position.index);

Serial.print(", Side : ");
Serial.print(position.side);
*/
    if (!position.sign) {
        position.side += 3;
    }

    position.side = Sides[position.side];
/*
Serial.print(", Sign : ");
Serial.print(position.sign);

Serial.print(", new side : ");
Serial.println(position.side);
*/
}

//void Mds::detectEvent(event_t &ev) {
void Mds::detectEvent() {

    /**
     *  Playing
     *  1. Si on dépasse en valeur absolue un minimum (0.7), on incrémente un compteur count
     *  2. Si on passe en dessous du seuil 0.7, on remet à 0 le compteur
     *  3. Au bout de 10 passages (toutes les 100ms = 1sec), on déclare que la position est validée
     *  4. On passe en mode lecture
     */
    processAccel();
/*
    Serial.print("Current side : ");
    Serial.print(event.current_side);
    Serial.print(", Position side : ");
    Serial.print(position.side);
    Serial.print(", Largest : ");
    Serial.println(position.largest);
*/


    if (event.current_side != SIDE_UNKNOW && event.current_side != position.side) {
        //pos_reset();
        event = event_t();

        //Serial.println("RESET 1");
        //event = cube_pos_t();
    }

    if (event.counter < COUNTER_VALID_VALUE && position.largest > THRESHOLD_INC_MIN) {
        event.current_side = position.side;
        event.counter += COUNTER_INC_VALUE;
        //Serial.println("INC");
    } else if (position.largest < THRESHOLD_DEC_MIN) {
        event.counter -= COUNTER_DEC_VALUE;
        //Serial.println("DEC");
    }

    if (event.counter <= 0) {
        //Serial.println("RESET 2");
        //Serial.print(event.processed);
        //pos_reset();

        event = event_t();
        //event = cube_pos_t();
    }

    if (event.counter == COUNTER_VALID_VALUE) {
        //Serial.println("HERE!!!!");
        event.validated = true;
    }

    /**
     *  Recording :
     *  1. Lock sur une valeur d'un axe supérieure à 0.7
     *  2. On doit rester sur cette position > 0.7 pendant minimum 1 seconde
     *  3. Accélération sur un axe > 1.6
     *  4. Si tjrs sur le meme axe, on lance le record
     */
    /*
    if (event.validated) {
        Serial.println(position.largest);
        if (position.largest > THRESHOLD_SHAKED_INC_MIN) {
            Serial.print("[SHAKE]");
            event.shaked_counter += COUNTER_SHAKED_INC_VALUE;
        } else if (position.largest < COUNTER_SHAKED_DEC_VALUE) {
            event.shaked_counter -= COUNTER_SHAKED_DEC_VALUE;
        }

        if (event.shaked_counter <= 0) {
            event.shaked_counter = 0;
            event.shaked = false;
        }

        if (event.shaked_counter == COUNTER_SHAKED_VALID_VALUE) {
            event.shaked = true;
        }
    }
    */

    //memcpy(&ev, &event, sizeof(event_t));

/*
    Serial.print("event: ");
    Serial.print(event.validated);
    Serial.print(", ev: ");
    Serial.print(ev.validated);
    //ev.processed = event.processed;
*/
}

