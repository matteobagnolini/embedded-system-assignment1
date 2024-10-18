#include <EnableInterrupt.h>
#include <LiquidCrystal_I2C.h>
#include <avr/sleep.h>
#include <math.h>
#include <stdlib.h>  //TODO ?

const unsigned GAME_PINS = 4;

const unsigned LED_PIN1 = 13;
const unsigned LED_PIN2 = 3;
const unsigned LED_PIN3 = 4;
const unsigned LED_PIN4 = 5;

const unsigned REDLED_PIN = 6;

const unsigned BUTTON_PIN1 = 2;
const unsigned BUTTON_PIN2 = 12;
const unsigned BUTTON_PIN3 = 11;
const unsigned BUTTON_PIN4 = 10;

bool ledState1;
bool ledState2;
bool ledState3;
bool ledState4;

const unsigned SLEEP_TIME = 10000;
const unsigned MIN_VISIBLE = 3000;
const unsigned RED_LED_TIME = 1000;
const unsigned T1 = 10000;

int ledStates[4] = {LOW, LOW, LOW, LOW};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

static void changeLedState();  // This method should be called when button's
                               // interrupt is raised
static String getBinaryNumber();

enum difficulty { EASY, MEDIUM, HARD, ABSURD };

float F = 0;
int t;
int game_num;
int score;
bool correct;
bool started;
unsigned long start_time;
int scored;
char buffer[50];
unsigned long currentMillis;

enum PHASE { SLEEP, PREPARATION, GAME, POINT_CHANGE, GAME_OVER } level;

int currIntensity;
unsigned fadeAmount = 100;

void button1fun() {
    if (level != SLEEP) {
        changeLed1State();
    }
};

void setup() {
    setupHardware();
    reset();
    Serial.begin(9600);
    // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), button1fun, RISING);
}

void loop() {
    currentMillis = millis();
    switch (level) {
        case PREPARATION:
            Serial.println("Preparation");
            blinkingLed();
            if (started == false && currentMillis - start_time < SLEEP_TIME) {
                started = true;
                displayMessage("Welcome to GMB! Press B1 to Start");
            } else if (currentMillis - start_time > SLEEP_TIME) {
                level = SLEEP;
                started = false;
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_enable();
                sleep_mode();
                Serial.println("fuori Sleep");
                /* First thing to do is disable sleep. */
                // sleep_disable();
                level = PREPARATION;
            }
            break;
        case GAME:
            Serial.println("Game");
            correct = getNumberFromBoard() == game_num;
            if (correct) {
                nextTurn();
            } else if (currentMillis - start_time > t) {
                start_time = currentMillis;
                level = GAME_OVER;
                redLedOn();
            }
            break;

        case POINT_CHANGE:
            Serial.println("POINT_CHANGE");

            if (currentMillis - start_time > 2 * MIN_VISIBLE) {
                scored = false;
                displayMessage("");
                start_time = currentMillis;
                level = GAME;
            } else if (!scored && currentMillis - start_time > MIN_VISIBLE) {
                scored = true;
                snprintf(buffer, sizeof(buffer), "%d", score);
                displayMessage(buffer);
            }
            break;

        case GAME_OVER:
            Serial.println("GameJover");
            if (currentMillis - start_time > SLEEP_TIME) {
                reset();
            } else if (!scored && currentMillis - start_time > RED_LED_TIME) {
                scored = true;
                redLedOff();
                snprintf(buffer, sizeof(buffer), "Game Over - Final Score %d",
                         score);
                displayMessage(buffer);
            }
            break;
    }
    delay(500);
}

void nextTurn() {
    displayMessage("GOOD!!");
    correct = false;
    t = t * (1 - F);
    game_num = rand() % 16;
    score++;
    allLedOff();
    level = POINT_CHANGE;
}

void reset() {
    Serial.println("reset fun");
    level = PREPARATION;
    game_num = rand() % 16;
    scored = false;
    correct = false;
    score = 0;
    displayMessage("");
    t = T1;
    started = false;
    start_time = millis();
}

void starter() {
    enum difficulty diff;
    start_time = millis();
    diff = getDifficulty();
    F = adjustDifficulty(diff);
    level = GAME;
}

float adjustDifficulty(enum difficulty diff) {
    switch (diff) {
        case EASY:
            return 0.1;

        case MEDIUM:
            return 0.2;

        case HARD:
            return 0.5;

        case ABSURD:
            return 0.7;
    }
}

// ogni interrupt controlla che non sia su SLEEP,
//  l'interrupt su b1 controlla se siamo in Preparation Chiama starter()

void setupHardware() {
    Serial.println("Hardware setup");
    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    pinMode(LED_PIN3, OUTPUT);
    pinMode(LED_PIN4, OUTPUT);
    pinMode(REDLED_PIN, OUTPUT);

    pinMode(BUTTON_PIN1, INPUT);
    pinMode(BUTTON_PIN2, INPUT);
    pinMode(BUTTON_PIN3, INPUT);
    pinMode(BUTTON_PIN4, INPUT);

    Serial.begin(9600);  // for logging purpose

    // lcd initialisation
    lcd.init();
    lcd.backlight();
    currIntensity = 0;

    // Interrupt setupped here
    enableInterrupt(BUTTON_PIN1, b1Pressed, RISING);
    enableInterrupt(BUTTON_PIN2, changeLed2State, RISING);
    enableInterrupt(BUTTON_PIN3, changeLed3State, RISING);
    enableInterrupt(BUTTON_PIN4, changeLed4State, RISING);
}

void blinkingLed() {
    Serial.println("blinkingLed");
    analogWrite(REDLED_PIN, currIntensity);
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255) {
        fadeAmount = -fadeAmount;
    }
    delay(15);
}

void displayMessage(String msg) {
    Serial.println(msg);
    lcd.clear();
    lcd.print(msg);
}

int getNumberFromBoard() {
    int num = 0;
    for (int i = 0; i < GAME_PINS; i++) {
        num += ledStates[4 - i - 1] * (1 << i);
    }
    return num;
}

static void getBinaryNumber(char* bin) {
    for (int i = 0; i < GAME_PINS; i++) {
        bin[i] =
            ledStates[i] == HIGH
                ? '1'
                : '0';  // Impostare '1' o '0' a seconda dello stato del LED
    }
    bin[GAME_PINS] = '\0';  // Aggiungere il carattere di terminazione null
    Serial.println(bin);    // Stampa della stringa binaria per il logging
}

// This solution reuse too much code. We should think about
// a better implementation (Note: we cant use args in this methods)
static void changeLed1State() {
    ledState1 = ledState1 == HIGH ? LOW : HIGH;
    digitalWrite(LED_PIN1, ledState1);
}

static void changeLed2State() {
    if (level == GAME) {
        ledState2 = ledState2 == HIGH ? LOW : HIGH;
        digitalWrite(LED_PIN2, ledState2);
    }
}

static void changeLed3State() {
    if (level == GAME) {
        ledState3 = ledState3 == HIGH ? LOW : HIGH;
        digitalWrite(LED_PIN3, ledState3);
    }
}

static void changeLed4State() {
    if (level == GAME) {
        ledState4 = ledState4 == HIGH ? LOW : HIGH;
        digitalWrite(LED_PIN4, ledState4);
    }
}

static void redLedOn() { digitalWrite(REDLED_PIN, HIGH); }

static void redLedOff() { digitalWrite(REDLED_PIN, LOW); }

static void allLedOff() {
    digitalWrite(LED_PIN1, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
    digitalWrite(LED_PIN4, LOW);
    digitalWrite(REDLED_PIN, LOW);
}

static enum difficulty getDifficulty() {
    int sensorValue = analogRead(A0);
    if (sensorValue <= 66)
        return EASY;
    else if (sensorValue <= 132)
        return MEDIUM;
    else if (sensorValue <= 198)
        return HARD;
    else
        return ABSURD;
}

static void b1Pressed() {
    if (level == GAME) {
        changeLed1State();
    } else if (level == PREPARATION) {
        starter();
    } else if (level == SLEEP) {
        sleep_disable();
    }
}