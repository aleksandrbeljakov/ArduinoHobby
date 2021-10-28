// #define DECODE_PANASONIC    // the same as DECODE_KASEIKYO
// #define DECODE_NEC          // Includes Apple and Onkyo
// #define SEND_PWM_BY_TIMER
#define LED_COUNT   299
#define LED_DT   13

#include <Arduino.h>


#define IR_RECEIVE_PIN       2// To be compatible with interrupt example, pin 2 is chosen here.

#define BTN_CH_MINUS  0x4E5BA3AD
#define BTN_PWR  0xE51CA6AD
#define BTN_CH_PLUS  0xE207E1AD
#define BTN_BR_MINUS  0x5484B6AD
#define BTN_BR_PLUS  0xD22353AD

#include <FastLED.h>
#include <IRLremote.h>
#include <LED.h>
//#include <microLED.h>   // подключаем библу

CHashIR IRLremote;
uint32_t IRdata;
struct CRGB ledStorage[LED_COUNT];


LEDEffects leds(ledStorage, LED_COUNT, 255);

bool processInterrupted();

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<WS2812B, LED_DT, GRB>(ledStorage, LED_COUNT).setCorrection(TypicalLEDStrip);

    leds.init();

    IRLremote.begin(IR_RECEIVE_PIN);

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
}


void loop() {
    bool read_flag = false;
    if (IRLremote.available()) {
        auto data = IRLremote.read();
        IRdata = data.command;
        /*Serial.print(F("Buffer length: "));
        Serial.println(data.address, DEC);
        Serial.print(F("Command: 0x"));
        Serial.println(data.command, HEX);
        Serial.println();*/
        read_flag = true;
    }
    if (read_flag) {
        switch (IRdata) {
            case BTN_CH_MINUS:
                Serial.println(F("CH-"));
                leds.prevMode();
                break;
            case BTN_PWR:
                Serial.println(F("PW"));
                leds.powerSwitch();
                break;
            case BTN_CH_PLUS:
                Serial.println(F("CH+"));
                leds.nextMode();
                break;
            case BTN_BR_MINUS:
                Serial.println(F("BR-"));
                leds.decreaseBrightness();
                break;
            case BTN_BR_PLUS:
                Serial.println(F("BR+"));
                leds.increaseBrightness();
                break;
            default:;

        }

    }

    leds.tick(processInterrupted);
}

bool processInterrupted() {
    return IRLremote.receiving();
}
