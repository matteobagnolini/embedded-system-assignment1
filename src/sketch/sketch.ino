#include <avr/sleep.h>
#include "hardware.h"
#include "enumPhase.h"

#define SLEEP_TIME 10000
#define MIN_VISIBLE 1500
#define RED_LED_TIME 100
#define T1 10000

float F = 0;
int t;
int game_num;
int score;
bool correct;
bool started;
bool welcome;
unsigned long start_time;
int scored;
char buffer[50];
unsigned long currentMillis;
unsigned long lastFadeUpdate = 0;
unsigned fadeInterval = 3;

enum PHASE level;

void setup() {
    setupHardware();
    reset();
    Serial.begin(9600);
}

void loop() {
    currentMillis = millis();
    switch (level) {
        case PREPARATION:
            Serial.println("Preparation");
            fadingLed();
            if (welcome == false && currentMillis - start_time < SLEEP_TIME) {
                welcome = true;
                displayMessage("Welcome to GMB! Press B1 to Start");
            } else if (currentMillis - start_time > SLEEP_TIME) {
                level = SLEEP;
                welcome = false;
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
            if (!started) {
                snprintf(buffer, sizeof(buffer), "%d", game_num);
                displayMessage(buffer);
                Serial.println("2 qui");
                started = true;
            }
            correct = getNumberFromBoard() == game_num;
            if (correct) {
                nextTurn();
            } else if (currentMillis - start_time > t) {
                start_time = currentMillis;
                level = GAME_OVER;
                redLedOn();
                displayMessage("Time is Over");
            }
            break;

        case POINT_CHANGE:
            Serial.println("POINT_CHANGE");

            if (currentMillis - start_time > 2 * MIN_VISIBLE) {
                scored = false;
                displayMessage("");
                start_time = currentMillis;
                started = false;
                level = GAME;
            } else if (!scored && currentMillis - start_time > MIN_VISIBLE) {
                Serial.println("ci entra?");
                scored = true;
                snprintf(buffer, sizeof(buffer), "score: %d", score);
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

        case STARTING:
            Serial.println(currentMillis - start_time);
            Serial.println(started);
            if (!started && currentMillis - start_time < MIN_VISIBLE) {
                Serial.println("sono dentro");
                started = true;
                displayMessage("!!! Go! !!!");
            }
            if (currentMillis - start_time > MIN_VISIBLE) {
                start_time = currentMillis;
                started = false;
                level = GAME;
            }
            break;
    }
}

void nextTurn() {
    start_time = currentMillis;
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
    welcome = false;
    score = 0;
    displayMessage("");
    t = T1;
    started = false;
    start_time = millis();
}

void starter() {
    allLedOff();
    enum difficulty diff;
    diff = getDifficulty();
    F = adjustDifficulty(diff);
    started = false;
    start_time = currentMillis;
    level = STARTING;
}

static float adjustDifficulty(enum difficulty diff) {
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
