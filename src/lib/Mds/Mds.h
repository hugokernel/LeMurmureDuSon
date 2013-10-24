
#ifndef Mds_h
#define Mds_h

#include "Wire.h"
#include "ADXL345.h"
#include "HMC5883L.h"
#include <EEPROM.h>

//#define HL1606

#ifdef HL1606
#include "HL1606strip.h"
#define SET_LED_COLOR(index, color) strip.setLEDcolor(index, color)
#define UPDATE_LED()                strip.writeStrip()
#define GET_LED_COUNT()             strip.numLEDs()
#define GET_LED_COLOR(index)        strip.getLEDcolor(index)

#define STRIP_D 11
#define STRIP_C 13
#define STRIP_L 10
#else
#include <Adafruit_NeoPixel.h>
#define SET_LED_COLOR(index, color) strip.setPixelColor(index, color)
#define UPDATE_LED()                strip.show()
#define GET_LED_COUNT()             strip.numPixels()
#define GET_LED_COLOR(index)        strip.getPixelColor(index)

#define BLACK   Adafruit_NeoPixel::Color(0, 0, 0)
#define WHITE   Adafruit_NeoPixel::Color(255, 255, 255)
#define RED     Adafruit_NeoPixel::Color(255, 0, 0)
#define YELLOW  Adafruit_NeoPixel::Color(255, 255, 0)
#define GREEN   Adafruit_NeoPixel::Color(0, 255, 0)
#define TEAL    Adafruit_NeoPixel::Color(2, 132, 130)
#define BLUE    Adafruit_NeoPixel::Color(0, 0, 255)
#define VIOLET  Adafruit_NeoPixel::Color(102, 51, 153)

#define STRIP   13
#endif

//#define VERSION_06
#define VERSION_08

#define UP(x)   digitalWrite(x, HIGH)
#define DOWN(x) digitalWrite(x, LOW)
#define READ(x) digitalRead(x)

#ifdef VERSION_06
#define VIBRATOR A1     // PC1
#endif

#ifdef VERSION_06
#define M1 A2
#define M2 A3
#define M3 20 // PB6
#define M4 21 // PB7
#define M5 4
#define M6 7
#define M7 6
#define M8 5
#else
#define M1 A2
#define M2 A3
#define M3 A0
#define M4 A1
#define M5 4
#define M6 7
#define M7 6
#define M8 5
#endif

#define CHG 3

#define REC_PLAY 8
#define LED 2

#define DEBUG

// Analog input
#define AD0     6   // Battery
#define AD1     7   // Charger
#define TEMP    8   // Avr internal temperature sensor

/**
 *  Analog parts
 */
#define VIN_MAX     (double)1.1

// Battery voltage
#define R11         6800
#define R12         2200
#define VBAT_MAX    (double)4.5
#define VBAT_LSB    (double)(VIN_MAX / 1024)
//#define VBAT_RATIO  (double)(R12 / (R11 + R12))
#define VBAT_RATIO  (double)(VBAT_MAX / VIN_MAX)
#define VBAT(value) (double)value * (double)VBAT_LSB * (double)VBAT_RATIO 
//#define VBAT(value) (double)value * 0.0039950

#define BATTERY_LEVEL(x)    x / (VBAT_LSB * VBAT_RATIO)

#define BATTERY_LEVEL_FULL      BATTERY_LEVEL(3.5)
#define BATTERY_LEVEL_LOW       BATTERY_LEVEL(3.3)
#define BATTERY_LEVEL_CRITICAL  BATTERY_LEVEL(3.1)
#define BATTERY_LEVEL_STOP      BATTERY_LEVEL(3.0)

#define BATTERY_STATE_FULL      3
#define BATTERY_STATE_LOW       2
#define BATTERY_STATE_CRITICAL  1
#define BATTERY_STATE_STOP      0


// Charger voltage
#define R13         10000
#define R14         1500
#define VCHG_MAX    7
#define VCHG_LSB    (double)VIN_MAX / 1024
//#define VCHG_RATIO  (float)R14 / (R13 + R14)
#define VCHG_RATIO  (double)(VCHG_MAX / VIN_MAX)
#define VCHG(value) (double)value * (double)VCHG_LSB * (double)VCHG_RATIO 

typedef enum Sides {
    SIDE_UNKNOW = -1,
    SIDE_0 = 0,
    SIDE_1 = 1,
    SIDE_2 = 2,
    SIDE_3 = 3,
    SIDE_4 = 4,
    SIDE_5 = 5,
};

typedef enum Status {
    STATUS_EMPTY = 0,
    STATUS_FULL  = 1,
    STATUS_READ  = 2
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

// 1.6
#define THRESHOLD_SHAKED_INC_MIN    1.4

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
    char ledMapping[6];
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

/*
typedef struct path_t {
    uint8_t index;
    uint8_t faces[10];
};
*/

class Mds {
private:

    //int getHigher(double [], int &, bool &);
    int handlePosition(position_t &);

    void msgDown(uint8_t);
    void msgUp(uint8_t);
    //void (*_validated)();
    //void (*_shaked());

public:
#ifdef HL1606
    Mds(HL1606strip, ADXL345);
#else
    Mds(Adafruit_NeoPixel, ADXL345);
#endif

    void init();

    char currentMsg;
    char lastReadMsg;

    volatile static bool _isPlaying;
    volatile static bool _isRecording;
    volatile static bool _isBusy;
    //volatile static bool _isCharging;
    
    bool _mute;

#ifdef HL1606
    HL1606strip strip;
#else
    Adafruit_NeoPixel strip;
#endif

    ADXL345 accel;
    HMC5883L compass;

    position_t position;
    event_t event;

    // Message status
    Status status[6];

    void setStatus(Status);

    // Path related
    uint8_t path_index;
    uint8_t path[5];

    void addPath(uint8_t);
    bool testPath(uint8_t [], uint8_t);

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
    bool isOnCharger();

    uint8_t getBatteryState();
    double getBatteryVoltage();
    double getChargerVoltage();
    float getTemperature();

    void mute(bool);
    bool record(uint8_t);
    bool play(uint8_t);
    bool stop();

    // IHM
    void vibrate(int);

    void ledOn(uint8_t, uint32_t);
    void ledsOff();

    void ledsColor(uint32_t);

    void rainbowParty(uint8_t);

    // Sensor
    //void readAccel(double &);
    //uint8_t getUpWardlySide(void);
    void processAccel();
    //void processAccel(position_t &);

    void resetEvent();
    void detectEvent();

    void (*_callback_play)(uint8_t);
    void (*_callback_record)(uint8_t);
    void (*_callback_stop)();
    void setCallbacks(void (*)(uint8_t), void (*)(uint8_t), void (*)());

//    void interruptRising();
//    void interruptFalling();
};

#endif

