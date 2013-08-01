
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

unsigned char Messages[] = {
    M1, M2, M3, M4, M5, M6, M7
};

inline void msgSelect(unsigned char index) {
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

bool record(unsigned char index) {

}

bool play(unsigned char index) {

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
}

ISR(TIMER1_OVF_vect)
{
    //TCNT1 = 0;     // reset timer ct to 130 out of 255
    //TCNT1 = 34286;            // preload timer
    TCNT1 = 0;            // preload timer
    //TIFR1 = 0x100;    // timer2 int flag reg: clear timer overflow flag
}

void loop() {

}

