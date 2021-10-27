#include "LED.h"
#include <FastLED.h>

LEDEffects::LEDEffects(CRGB *ledStorage, int ledCount, byte maxBrightness, effects initMode, byte brightnessStep) {
    this->mode = initMode;
    this->maxBrightness = maxBrightness;
    this->currentBrightness = maxBrightness;
    this->brightnessStep = brightnessStep;
    this->ledStorage = ledStorage;
    this->ledCount = ledCount;
}

void LEDEffects::init() {
    if (inited) {
        Serial.println(F("LED effects already inited"));
        return;
    }
    FastLED.setBrightness(maxBrightness);
    FastLED.clear(true);
    inited = true;
    Serial.println(F("LED effects inited: "));
    Serial.print(F("LEDS count: "));
    Serial.println(ledCount);

}

void LEDEffects::powerSwitch() {
    if (!inited) {
        return;
    }
    if (powerOn) {
        disable();
    } else {
        enable();
    }
}

void LEDEffects::enable() {
    Serial.println(F("Led enabled"));
    this->powerOn = true;
    markModeChanged();
}

void LEDEffects::disable() {
    this->powerOn = false;
    FastLED.clear(true);
    Serial.println(F("Led disabled"));
}

void LEDEffects::tick(bool (*interrupted)(void)) {
    if (!inited) {
        return;
    }
    if (!powerOn) {
        //  Serial.println(F("Led is disabled"));
        return;
    }
    if (delay.isWait()) {
        return;
    }
    int _currentBrightness = currentBrightness;
    switch (mode) {

        case LIGHT_WHITE:
            all_leds_one_color(255, 255, 255);
            _currentBrightness = (int) (_currentBrightness * 0.2);
            break;
        case FULL_WHITE:
            all_leds_one_color(255, 255, 255);
            break;
        case FULL_RED:
            all_leds_one_color(255, 0, 0);
            break;
        case FULL_GREEN:
            all_leds_one_color(0, 255, 0);
            break;
        case FULL_BLUE:
            all_leds_one_color(0, 0, 255);
            break;
        case RAINBOW_FADE:
            rainbow_fade();
            break;
        case RAINBOW_LOOP:
            rainbow_loop();
            break;
        case POLICE_LIGHTS_SINGLE:
            ems_lightsONE();
            break;
        case POLICE_LIGHTS_SOLID:
            ems_lightsALL();
            break;
        case DISABLED:
        default:
            FastLED.clear(false);
            mode = DISABLED;
            break;
    }
    if (!interrupted() && (modeChanged || redraw)) {
        FastLED.show(_currentBrightness);
        modeChanged = false;
        redraw = false;
    }
}

void LEDEffects::nextMode() {
    if (!powerOn) {
        return;
    }
    mode++;
    markModeChanged();
    Serial.print(F("Next mode: "));
    Serial.println(mode);
}

void LEDEffects::prevMode() {
    if (!powerOn) {
        return;
    }
    mode--;
    if (mode < 0) {
        mode = DISABLED;
    }
    markModeChanged();
    Serial.print(F("Prev mode: "));
    Serial.println(mode);
}

void LEDEffects::increaseBrightness() {
    if (!powerOn) {
        return;
    }
    this->currentBrightness += brightnessStep;
    if (this->currentBrightness > maxBrightness) {
        currentBrightness = maxBrightness;
    }
    markRedraw();
    Serial.print(F("Increase brightness: "));
    Serial.println(currentBrightness);
}

void LEDEffects::decreaseBrightness() {
    if (!powerOn) {
        return;
    }
    this->currentBrightness -= brightnessStep;
    if (this->currentBrightness < 0) {
        currentBrightness = 0;
    }
    markRedraw();
    Serial.print(F("Decrease brightness: "));
    Serial.println(currentBrightness);
}

void LEDEffects::all_leds_one_color(int cred, int cgrn, int cblu) {
    if (!modeChanged) {
        return;
    }
    for (int i = 0; i < ledCount; i++) {
        ledStorage[i].setRGB(cred, cgrn, cblu);
    }

}


void LEDEffects::rainbow_fade() {
    hue.increaseHue();
    for (int i = 0; i < ledCount; i++) {
        ledStorage[i] = CHSV(hue.get(), saturation.get(), MAX_C_VALUE);
    }
    delay.set(60);
    markRedraw();
}

void LEDEffects::rainbow_loop() {
    hue.addHue(10);
    curIndex++;
    if (curIndex >= ledCount) {
        curIndex = 0;
    }
    ledStorage[curIndex] = CHSV(hue.get(), saturation.get(), MAX_C_VALUE);
    delay.set(20);
    markRedraw();
}

void LEDEffects::ems_lightsONE() {                    //-m7-EMERGENCY LIGHTS (TWO COLOR SINGLE LED)
    if (modeChanged) {
        hue.setHue(0);
    }
    curIndex++;
    if (curIndex >= ledCount) {
        curIndex = 0;
    }
    int idexR = curIndex;
    int idexB = antipodal_index(idexR);
    int thathue = (hue.get() + 160) % 255;
    for (int i = 0; i < ledCount; i++) {
        if (i == idexR) {
            ledStorage[i] = CHSV(hue.get(), saturation.get(), MAX_C_VALUE);
        } else if (i == idexB) {
            ledStorage[i] = CHSV(thathue, saturation.get(), MAX_C_VALUE);
        } else {
            ledStorage[i] = CHSV(0, 0, 0);
        }
    }
    delay.set(40);
    markRedraw();
}

void LEDEffects::ems_lightsALL() {                  //-EMERGENCY LIGHTS (TWO COLOR SOLID)
    if (modeChanged) {
        hue.setHue(0);
    }
    curIndex++;
    if (curIndex >= ledCount) {
        curIndex = 0;
    }
    int idexR = curIndex;
    int idexB = antipodal_index(idexR);
    int thathue = (hue.get() + 160) % 255;
    ledStorage[idexR] = CHSV(hue.get(), saturation.get(), MAX_C_VALUE);
    ledStorage[idexB] = CHSV(thathue, saturation.get(), MAX_C_VALUE);
    delay.set(40);
    markRedraw();
}


int LEDEffects::antipodal_index(int i) {
    int topIndex = ledCount / 2;
    int iN = i + topIndex;
    if (i >= topIndex) {
        iN = (i + topIndex) % ledCount;
    }
    return iN;
}
