


#ifndef LEDANDIR_LED_H
#define LEDANDIR_LED_H

#include <Arduino.h>
#include <FastLED.h>
#define MAX_HUE 255
#define MAX_C_VALUE 255
#define MIN_C_VALUE 0

#include <colorpalettes.h>

enum effects {
    DISABLED = 0,
    LIGHT_WHITE,
    RAINBOW_FADE,
    RAINBOW_LOOP,
    CYLON_V1,
    CYLON_V2,
    POLICE_LIGHTS_SINGLE,
    POLICE_LIGHTS_SOLID,
    FULL_WHITE,
    FULL_RED,
    FULL_GREEN,
    FULL_BLUE,

};

class LedDelay {
public:
    LedDelay() { lastExecution = millis(); };

    unsigned int get() {
        return delayTime;
    }

    bool isWait() {
        return millis() - lastExecution <= delayTime;
    }

    bool wait() {
        resetCounter();
        while (isWait()) {
            return true;
        }
        return false;
    }

    void set(int delay) {
        this->delayTime = delay;
    }

private:

    unsigned int delayTime = 20;
    uint32_t lastExecution;

    void resetCounter() {
        lastExecution = millis();
    }

};

class Saturation {
public:
    Saturation() {};

    byte get() {
        return value;
    }

    void set(byte saturation) {
        value = saturation;
    }

private:
    byte value = 255;
};

class Hue {
public:
    Hue() {};

    byte get() {
        return this->hueValue;
    }

    void increaseHue() {
        hueValue++;
        if (hueValue > MAX_HUE) {
            hueValue = 0;
        }
    }

    void addHue(byte hue) {
        hueValue += hue;
        if (hueValue > MAX_HUE) {
            hueValue = 0;
        }
    }

    void setHue(byte hue) {
        hueValue = hue;
    }

private:
    int hueValue = 0;
};

class LEDEffects {
public:
    LEDEffects(CRGB *ledStorage,
               int ledCount,
               byte maxBrightness,
               effects initMode = LIGHT_WHITE,
               byte brightnessStep = 10);

    void init();

    void tick(bool (*interrupted)(void));

    void powerSwitch();

    void nextMode();

    void prevMode();

    void increaseBrightness();

    void decreaseBrightness();

private:
    struct CRGB *ledStorage;
    int ledCount;
    byte maxBrightness;
    int currentBrightness;
    byte brightnessStep;
    bool inited = false;
    bool powerOn = false;
    uint8_t mode = DISABLED;
    bool modeChanged = true;
    bool redraw = true;
    Hue hue = Hue();
    Saturation saturation = Saturation();
    LedDelay delay = LedDelay();
    int curIndex = 0;
    bool bounceDirectionForward = true;

    void markModeChanged() {
        modeChanged = true;
        markRedraw();
    }

    void markRedraw() {
        redraw = true;
    }

    void enable();

    void disable();

    void all_leds_one_color(int cred, int cgrn, int cblu);

    void rainbow_fade();

    void rainbow_loop();

    void ems_lightsONE();

    int antipodal_index(int i);

    void ems_lightsALL();

    void color_bounce();

    void color_bounceFADE();

    int adjacent_cw(int i);

    int adjacent_ccw(int i);

    void changeColorValue(int index, byte value);
};


#endif //LEDANDIR_LED_H