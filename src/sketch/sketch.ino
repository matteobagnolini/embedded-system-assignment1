#include <avr/sleep.h>
#include <time.h>
#include "hardware.h"
#include "enumPhase.h"

//21/10/24 15:04

#define SLEEP_TIME 10000
#define MIN_VISIBLE 1500
#define RED_LED_GAME_OVER_TIME 1000
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

enum PHASE level;

void setup() {
    setupHardware();
    reset();
    srand(time(NULL));  // Initialize the seed
    Serial.begin(9600);
}

void loop() {
    currentMillis = millis();
    switch (level) {
        case PREPARATION:
            fadingLed(currentMillis);
            if (welcome == false && currentMillis - start_time < SLEEP_TIME) {
                welcome = true;
                displayMessage("Welcome to GMB! Press B1 to Start");
            } else if (currentMillis - start_time > SLEEP_TIME) {
                level = SLEEP;
                welcome = false;
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                sleep_enable();
                sleep_mode();
                sleep_disable();
                currentMillis = millis();
                start_time = currentMillis;
                level = PREPARATION;
            }
            if (canStart()) {
                setCanStart(false);
                starter();
            }

            break;
        case GAME:
            if (!started) {
                snprintf(buffer, sizeof(buffer), "%d", game_num);
                displayMessage(buffer);
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
            if (currentMillis - start_time > 2 * MIN_VISIBLE) {
                scored = false;
                displayMessage("");
                start_time = currentMillis;
                started = false;
                level = GAME;
            } else if (!scored && currentMillis - start_time > MIN_VISIBLE) {
                scored = true;
                snprintf(buffer, sizeof(buffer), "score: %d", score);
                displayMessage(buffer);
            }
            break;

        case GAME_OVER:
            if (currentMillis - start_time > SLEEP_TIME) {
                reset();
            } else if (!scored && currentMillis - start_time > RED_LED_GAME_OVER_TIME) {
                scored = true;
                setCanStart(false);
                redLedOff();
                snprintf(buffer, sizeof(buffer), "Game Over - Final Score %d",
                         score);
                displayMessage(buffer);
            }
            break;

        case STARTING:
            if (!started && currentMillis - start_time < MIN_VISIBLE) {
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
    allLedOff();
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
