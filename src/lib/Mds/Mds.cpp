
#include "Mds.h"

uint8_t Messages[8] = {
    M1, M2, M3, M4, M5, M6, M7, M8
    //M1, M2, M5, M6, M7, M8
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
#ifdef DEBUG
    Serial.print("[Charge!]");
#endif

    //Mds::_isCharging = true;
}

void interruptLed() {
    // Led is low during recording
    //Mds::_isRecording = false;

#ifdef DEBUG
    if (Mds::_isPlaying) {
        Serial.println("[End playing !]");
    }
#endif

    // Led is low when stop playing
    Mds::_isPlaying = false;
    Mds::_isBusy = false;
}

void Mds::init(void) {

    loadConfig();

    memset(path, 0x00, sizeof(path));

    // Led mapping
    for (int i = 0; i < sizeof(config.ledMapping); i++) {
        Leds[i] = config.ledMapping[i];
    }

    pinMode(CHG, INPUT);

    // Set output
    for (uint8_t i = 0; i < sizeof(Messages); i++) {
        pinMode(Messages[i], OUTPUT);
    }

    pinMode(REC_PLAY, OUTPUT);
    pinMode(LED, INPUT);

    msgUnselect();

    UP(REC_PLAY);

    accel.powerOn();
    compass = HMC5883L();

#ifdef VERSION_08
    DDRB |= (1 << DDB6);    // OUTPUT
    DDRB &= ~(1 << DDB7);   // INPUT
    PORTB &= ~(1 << PORTB6);
#else
    pinMode(VIBRATOR, OUTPUT);
    DOWN(VIBRATOR);
#endif

    event = event_t();

    attachInterrupt(0, interruptLed, FALLING);
    //attachInterrupt(1, interruptChg, FALLING);

    analogReference(INTERNAL);

    _mute = false;

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
    msgDown(Messages[index]);
}

inline void Mds::msgUnselect() {
    for (char i = 0; i < sizeof(Messages); i++) {
        msgUp(Messages[i]);
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

double Mds::getBatteryVoltage() {
    int ad0 = analogRead(AD0);
    return VBAT(ad0); //((float)ad0 * VBAT_LSB) * VCHG_RATIO;
}

uint8_t Mds::getBatteryState() {
    int vbat = analogRead(AD0);

    if (vbat >= BATTERY_LEVEL_FULL) {
        return BATTERY_STATE_FULL;
    }

    if (vbat >= BATTERY_LEVEL_LOW) {
        return BATTERY_STATE_LOW;
    }

    if (vbat >= BATTERY_LEVEL_CRITICAL) {
        return BATTERY_STATE_CRITICAL;
    }

    if (vbat >= BATTERY_LEVEL_STOP) {
        return BATTERY_STATE_STOP;
    }
}

double Mds::getChargerVoltage() {
    int ad1 = analogRead(AD1);
    return VCHG(ad1); //ad1 * VCHG_LSB / VCHG_RATIO;
}

float Mds::getTemperature() {
    //int temp = analogRead(TEMP);
    // From http://playground.arduino.cc/Main/InternalTemperatureSensor

    unsigned int wADC;
    double t;
    // Set the internal reference and mux.
    ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
    ADCSRA |= _BV(ADEN);  // enable the ADC

    delay(20);            // wait for voltages to become stable.

    ADCSRA |= _BV(ADSC);  // Start the ADC

    // Detect end-of-conversion
    while (bit_is_set(ADCSRA,ADSC));

    // Reading register "ADCW" takes care of how to read ADCL and ADCH.
    wADC = ADCW;
    return (wADC - 324.31 ) / 1.22;
}

bool Mds::isCharging() {
    return !READ(CHG);
}

bool Mds::isOnCharger() {
    return analogRead(AD1) > 0;
}

bool Mds::record(uint8_t index) {

    _callback_record(index);

    DOWN(REC_PLAY);

    // Todo: Sleep for 30ns
    delay(1);
    msgDown(Messages[index]);

    currentMsg = index;

    Mds::_isRecording = true;
    Mds::_isBusy = true;

    status[index] = STATUS_FULL;
}

void Mds::msgDown(uint8_t value) {
#ifdef VERSION_06
    if (value == M3) {
        PORTB &= ~(1 << PORTB6);
    } else if (value == M4) {
        PORTB &= ~(1 << PORTB7);
    } else {
        DOWN(value);
    }
#else
    DOWN(value);
#endif
}

void Mds::msgUp(uint8_t value) {
#ifdef VERSION_06
    if (value == M3) {
        PORTB |= (1 << PORTB6);
    } else if (value == M4) {
        PORTB |= (1 << PORTB7);
    } else {
        UP(value);
    }
#else
    UP(value);
#endif
}

void Mds::mute(bool mstatus) {
    _mute = mstatus;
}

bool Mds::play(uint8_t index) {

    _callback_play(index);
    if (!_mute) {
        if (Mds::_isPlaying) {
            msgDown(Messages[currentMsg]);
            delay(20);
            msgUp(Messages[currentMsg]);
            delay(20);
        }

        UP(REC_PLAY);

        // Todo: Sleep for 30ns
        delay(1);
        msgDown(Messages[index]);
        
        // 225k / fs = ~18ms
        delay(20); 
        msgUp(Messages[index]);
    }

    currentMsg = index;

    Mds::_isPlaying = true;
    Mds::_isBusy = true;

    status[index] = STATUS_READ;
}

bool Mds::stop() {
    if (Mds::_isRecording) {
        Mds::_isRecording = false;
    } else if (Mds::_isPlaying) {
        msgDown(Messages[currentMsg]);
        Mds::_isPlaying = false;
    }

    msgUnselect();
    Mds::_isBusy = false;
    currentMsg = -1;

    _callback_stop();
}

void Mds::vibrate(int time) {
#ifdef VERSION_08
    PORTB |= (1 << PORTB6);
    delay(time);
    PORTB &= ~(1 << PORTB6);
#else
    UP(VIBRATOR);
    delay(time);
    DOWN(VIBRATOR);
#endif
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

void Mds::setStatus(Status newstatus) {
    for (int i = 0; i < sizeof(status); i++) {
        status[i] = newstatus;
    }
}

void Mds::setCallbacks(void (*play)(uint8_t), void (*record)(uint8_t), void (*stop)()) {
    _callback_play = play;
    _callback_record = record;
    _callback_stop = stop;
}

void Mds::addPath(uint8_t face) {

    uint8_t i, o = 0;

    if (path_index < sizeof(path)) {

        // Skip if same face
        if (face == path[max(0, path_index - 1)]) {
            return;
        }

        path[path_index++] = face;

        //if (path_index >= sizeof(path)) {
        //    path_index = sizeof(path);
        //}
    } else {
        /*
        Serial.print("[");
        //Serial.print(path[sizeof(path) - 1]);
        Serial.print(path_index);
        Serial.print(" - ");
        Serial.print(path[path_index]);
        Serial.print("]");
        */

        // Skip if same face
        if (face == path[sizeof(path) - 1]) {
        //if (face == path[path_index]) {
            return;
        }

        //for (int i = 0; i < sizeof(path) - 1; i++) {
        for (i = 0, o = 1; i < path_index - 1; i++, o++) {
            path[i] = path[o];
        }

        //path[sizeof(path) - 1] = face;
        path[path_index - 1] = face;
    }

    Serial.print("AddPath: ");
    for (int i = 0; i < sizeof(path); i++) {
        Serial.print(path[i]);
        Serial.print(", ");
    }

    /*
     */
}

bool Mds::testPath(uint8_t pattern[], uint8_t count) {
    bool found = false;
    uint8_t offset, i = 0;

    //[ 2, 0, 2, 4, 0, 1 ]
    //[ 2, 4, 0, 1 ]

    if (path_index >= sizeof(path)) {
        offset = sizeof(path) - 1 - sizeof(pattern);
    }

    //for (i = 0; i < min(count, sizeof(path)); i++) {
    for (i = 0; i < sizeof(path); i++) {
        if (pattern[i] != path[i++]) {
            return false;
        }

        if (i >= sizeof(path)) {
            i = 0;
        }
    }

    return true;
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
    uint8_t axes[] = { 'x', 'y', 'z' };
    pos.largest = abs(pos.xyz[0]);
    pos.sign = (pos.xyz[0] > 0);
    pos.index = 0;
    pos.axe = axes[0];
    for (int i = 0; i < 3; i++) {
        if (pos.largest < abs(pos.xyz[i])) {
            pos.largest = abs(pos.xyz[i]);
            pos.sign = (pos.xyz[i] > 0);
            pos.index = i;
            pos.axe = axes[i];
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

void Mds::resetEvent() {
    event = event_t();
}

//void Mds::detectEvent(event_t &ev) {
void Mds::detectEvent() {

    static uint8_t pafCounter = 0;

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


    //if (event.current_side != SIDE_UNKNOW && event.current_side != position.side) {
    if (event.current_axe != position.axe) {
        pafCounter++;

        if (pafCounter > 2) {
/*
Serial.print("[Axe Reset : ");
Serial.print((char)event.current_axe);
Serial.print(", ");
Serial.print((char)position.axe);
Serial.println("]");
*/
            resetEvent();
            //event = cube_pos_t();   
        }
    } else {
        pafCounter = 0;
    }

    if (event.counter < COUNTER_VALID_VALUE
        &&
        position.largest > THRESHOLD_INC_MIN
        &&
        position.largest < THRESHOLD_INC_MAX
        ) {
        event.current_side = position.side;
        event.current_axe = position.axe;

        /*
        Serial.print("[");
        Serial.print((char)event.current_axe);
        Serial.print("]");
        */

        event.counter += COUNTER_INC_VALUE;
        //Serial.println("INC");
    }
    /*else if (position.largest < THRESHOLD_DEC_MIN) {
        event.counter -= COUNTER_DEC_VALUE;
        //Serial.println("DEC");
    }
    */

    if (event.counter <= 0) {
        //Serial.println("RESET 2");
        //Serial.print(event.processed);
        //pos_reset();

        //resetEvent();
        //event = cube_pos_t();

        event.current_side = SIDE_UNKNOW;
        event.counter = 0;
        event.validated = false;

        event.processed  = false;
        event.last_validated_side = -1;
        //event.last_validated_axe = 0;

        //Serial.print("[Reset!]");
    }

    if (event.counter == COUNTER_VALID_VALUE) {
        //Serial.println("HERE!!!!");
        event.validated = true;
        event.last_validated_axe = position.axe;

        addPath(event.current_side);
    }
//return;
    /**
     *  Recording :
     *  1. Lock sur une valeur d'un axe supérieure à 0.7
     *  2. On doit rester sur cette position > 0.7 pendant minimum 1 seconde
     *  3. Accélération sur un axe > 1.6
     *  4. Si tjrs sur le meme axe, on lance le record
     */
    //if (event.validated) {
    if (event.last_validated_axe == position.axe) {

        if (position.largest > THRESHOLD_SHAKED_INC_MIN) {
            /*
            if (event.shaked_counter == 0) {
                event.last_validated_side = event.current_side;
            }
            */

            event.shaked_counter += COUNTER_SHAKED_INC_VALUE;
            Serial.print("+");
        } else if (position.largest < COUNTER_SHAKED_DEC_VALUE) {
            if (event.shaked_counter > COUNTER_SHAKED_DEC_VALUE) {
                Serial.print("-");
                event.shaked_counter -= COUNTER_SHAKED_DEC_VALUE;
            }
        }

/*
Serial.print("[");
Serial.print(event.shaked_counter);
Serial.print("]");
*/
        if (event.shaked_counter <= 0) {
            event.shaked_counter = 0;
            event.shaked = false;
            //event.last_validated_side = -1;
            //Serial.print("[Reset Shaked!]");
        }

        if (event.shaked_counter >= COUNTER_SHAKED_VALID_VALUE) {
            //Serial.print("Here : ");
            //Serial.print(event.last_validated_side);
            //Serial.print(", ");
            //Serial.println(event.current_side);
            //if (event.last_validated_side == event.current_side) {
                //Serial.print("Shaked !");
                event.shaked = true;
            //}
        }
    }

/*
    Serial.print("event: ");
    Serial.print(event.validated);
    Serial.print(", ev: ");
    Serial.print(ev.validated);
    //ev.processed = event.processed;
*/
}

