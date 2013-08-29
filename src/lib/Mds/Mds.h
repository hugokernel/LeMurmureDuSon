
#ifndef Mds_h
#define Mds_h

#include "Wire.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include "HL1606strip.h"
#include <EEPROM.h>

#define UP(x)   digitalWrite(x, HIGH)
#define DOWN(x) digitalWrite(x, LOW)
#define READ(x) digitalRead(x)

#define VIBRATOR A1     // PC1

#define CHG 3

#define M1 A2
#define M2 A3
#define M3 20 // PB6
#define M4 21 // PB7
#define M5 4
#define M6 7
#define M7 6
#define M8 5

#define REC_PLAY 8
#define LED 2

#define STRIP_D 11
#define STRIP_C 13
#define STRIP_L 10

#define DEBUG

typedef enum Sides {
    SIDE_UNKNOW = -1,
    SIDE_0 = 0,
    SIDE_1 = 1,
    SIDE_2 = 2,
    SIDE_3 = 3,
    SIDE_4 = 4,
    SIDE_5 = 5,
};

typedef struct position_t {
    double xyz[3];  // Current values from sensor

    uint8_t index;  // Index of absolute upper sensor value
    double largest; // Most absolute upper sensor value
    bool sign;      // Positive value or not ?
    uint8_t side;   // Calculated upwardly side
    uint8_t axe;
};

#define THRESHOLD_INC_MIN   0.6     // Seuil de déclenchement mini
#define THRESHOLD_INC_MAX   1.1     // Seuil de déclenchement max
#define THRESHOLD_DEC_MIN   0.5     // Seuil de déclenchement pour reseter

#define COUNTER_VALID_VALUE 10      // Nombre de position mini
#define COUNTER_INC_VALUE   1
#define COUNTER_DEC_VALUE   3

#define THRESHOLD_SHAKED_INC_MIN     1.6

#define COUNTER_SHAKED_VALID_VALUE  4      // Nombre de position mini
#define COUNTER_SHAKED_INC_VALUE    1
#define COUNTER_SHAKED_DEC_VALUE    2

#define CONFIG_START    0
#define MDS_VERSION     "0.1"
#define MSG_DURATION    1000
#define DEBUG_MODE      false

typedef struct config_t {
    char version[4];
    unsigned int msgDuration;
    bool debugMode;
};

/*
MdsConfig = {
    MDS_VERSION,
    MSG_DURATION
};
*/

typedef struct event_t {
    uint8_t current_side;
    uint8_t current_axe;

    uint8_t counter;
    bool validated;

    uint8_t shaked_counter;
    bool shaked;

    uint8_t last_validated_side;
    uint8_t last_validated_axe;

    bool processed;

    event_t() :
        current_side(SIDE_UNKNOW),
        current_axe(0),

        counter(0),
        validated(false),
        shaked_counter(0),
        shaked(0),
        processed(false),
        last_validated_side(-1),
        last_validated_axe(0)
        { }
};

class Mds {
private:

    //int getHigher(double [], int &, bool &);
    int handlePosition(position_t &);

    void msgDown(uint8_t);
    void msgUp(uint8_t);
    //void (*_validated)();
    //void (*_shaked());

public:
    Mds(HL1606strip, ADXL345);

    void init();

    char currentMsg;
    char lastReadMsg;

    volatile static bool _isPlaying;
    volatile static bool _isRecording;
    volatile static bool _isBusy;
    //volatile static bool _isCharging;

    HL1606strip strip;

    ADXL345 accel;
    HMC5883L compass;

    position_t position;
    event_t event;

    // Configuration
    config_t config;
    void loadConfig();
    void saveConfig();

    // ISD
    inline void msgSelect(uint8_t);
    inline void msgUnselect();

    bool isPlaying();
    bool isRecording();
    bool isBusy();

    bool isCharging();

    bool record(uint8_t);
    bool play(uint8_t);
    bool stop();

    // IHM
    void vibrate(int);

    void ledOn(uint8_t, uint8_t);
    void ledsOff();

    void ledsColor(uint8_t);

    void rainbowParty(uint8_t);

    // Sensor
    //void readAccel(double &);
    //uint8_t getUpWardlySide(void);
    void processAccel();
    //void processAccel(position_t &);

    void resetEvent();
    void detectEvent();

    //void setCallbacks(void *(), void *());

//    void interruptRising();
//    void interruptFalling();
};

#endif

