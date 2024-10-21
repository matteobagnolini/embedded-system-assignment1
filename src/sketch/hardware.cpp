#include <EnableInterrupt.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "hardware.h"
#include "enumPhase.h"

int ledStates[] = {LOW, LOW, LOW, LOW};
unsigned long timePassed[4] = {0, 0, 0, 0};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

int fadingLedCurrIntensity = 0;
int fadeAmount = 5;
unsigned long lastFadeUpdate = 0;
unsigned fadeInterval = 7;

extern enum PHASE level;

static void changeLed1State();
static void changeLed2State();
static void changeLed3State();
static void changeLed4State();
static void b1Pressed();

bool canStartGame = false;

void setupHardware() {
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(LED_PIN3, OUTPUT);
    pinMode(LED_PIN4, OUTPUT);
    pinMode(REDLED_PIN, OUTPUT);

    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);
    pinMode(BUTTON_PIN3, INPUT);
    pinMode(BUTTON_PIN4, INPUT);

    //Serial.begin(9600);  // for logging purpose

    // lcd initialisation
    lcd.init();
    lcd.backlight();
    fadingLedCurrIntensity = 0;

    // Interrupt setupped here
    enableInterrupt(BUTTON_PIN1, b1Pressed, RISING);
    enableInterrupt(BUTTON_PIN2, changeLed2State, RISING);
    enableInterrupt(BUTTON_PIN3, changeLed3State, RISING);
    enableInterrupt(BUTTON_PIN4, changeLed4State, RISING);
}

void fadingLed(unsigned long currentMillis) {
    if (currentMillis - lastFadeUpdate >= fadeInterval) {
        analogWrite(REDLED_PIN, fadingLedCurrIntensity);
        fadingLedCurrIntensity += fadeAmount;
        if (fadingLedCurrIntensity == 0 || fadingLedCurrIntensity == 255) {
            fadeAmount = -fadeAmount;
        }
        lastFadeUpdate = currentMillis;
    }
}

void displayMessage(String msg) {
    Serial.println(msg);
    lcd.clear();
    lcd.print(msg);
}

int getNumberFromBoard() {
    int num = 0;
    for (int i = 0; i < GAME_LEDS; i++) {
        num += ledStates[4 - i - 1] * (1 << i);
    }
    return num;
}

static void changeLedState(const unsigned PIN, int index) {
    if (level == GAME) {
        unsigned long currTime = millis();
        if (currTime - timePassed[index] >= REBOUND_TIME) {
            ledStates[index] = ledStates[index] == HIGH ? LOW : HIGH;
            digitalWrite(PIN, ledStates[index]);
            timePassed[index] = currTime;
        }
    }
}

static void changeLed1State() { changeLedState(LED_PIN1, 0); }

static void changeLed2State() { changeLedState(LED_PIN2, 1); }

static void changeLed3State() { changeLedState(LED_PIN3, 2); }

static void changeLed4State() { changeLedState(LED_PIN4, 3); }

void allLedOff() {
    digitalWrite(LED_PIN1, LOW);
    ledStates[0] = LOW;
    digitalWrite(LED_PIN2, LOW);
    ledStates[1] = LOW;
    digitalWrite(LED_PIN3, LOW);
    ledStates[2] = LOW;
    digitalWrite(LED_PIN4, LOW);
    ledStates[3] = LOW;
    redLedOff();
}

enum difficulty getDifficulty() {
    int sensorValue = map(analogRead(A0), 0, 1023, 0, 255);
    if (sensorValue <= 66)
      return EASY;

    else if (sensorValue <= 132) 
        return MEDIUM; 

    else if (sensorValue <= 198)
      return HARD;

    else
      return ABSURD;
}

void redLedOn() {
  digitalWrite(REDLED_PIN, HIGH);
}

void redLedOff() {
  digitalWrite(REDLED_PIN, LOW);
}

static void b1Pressed() {
    if (level == PREPARATION || level == STARTING) {
        canStartGame = true;
    } //else if (level == SLEEP) { } // Wakes up Arduino
    changeLedState(LED_PIN1, 0);
}

bool canStart() {
    return canStartGame;
}

void setCanStart(bool value) {
    canStartGame = value;
}