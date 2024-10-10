#include <stdio.h>
#include <stdlib.h>  //TODO ?

#include "hardware.h"

#define millis() 3

const unsigned SLEEP_TIME = 10000;
const unsigned MIN_VISIBLE = 3000;
const unsigned RED_LED_TIME = 1000;
const unsigned T1 = 10000;

float F = 0;
int t;
int game_num;
int score;
bool correct;
bool started;
int start_time;
int scored;
char buffer[50];

enum FASE { SLEEP, PREPARATION, GAME, POINT_CHANGE, GAME_OVER } level;

void setup() { reset(); }

void loop() {
    switch (level) {
        case PREPARATION:
            if (started = false && millis() - start_time < SLEEP_TIME) {
                started = true;
                displayMessage("Welcome to GMB! Press B1 to Start");
            } else {
                level = SLEEP;
                started = false;
            }
        case GAME:
            correct = getNumberFromBoard() == game_num;
            if (correct) {
                nextTurn();
            } else if (millis() - start_time > t) {
                start_time = millis();
                level = GAME_OVER;
                red_led_on();
            }
            break;

        case POINT_CHANGE:

            if (millis() - start_time > 2 * MIN_VISIBLE) {
                scored = false;
                displayMessage("");
                start_time = millis();
                level = GAME;
            } else if (!scored && millis() - start_time > MIN_VISIBLE) {
                scored = true;
                snprintf(buffer, sizeof(buffer), "%d", score);
                displayMessage(buffer);
            }
            break;

        case GAME_OVER:
            if (millis() - start_time > SLEEP_TIME) {
                reset();
            } else if (!scored && millis() - start_time > RED_LED_TIME) {
                scored = true;
                red_led_off();
                snprintf(buffer, sizeof(buffer), "Game Over - Final Score %d",
                         score);
                displayMessage(buffer);
            }
    }
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