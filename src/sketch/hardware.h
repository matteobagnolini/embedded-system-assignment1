#define GAME_LEDS 4

const unsigned LED_PIN1 = 13;
const unsigned LED_PIN2 = 3;
const unsigned LED_PIN3 = 4;
const unsigned LED_PIN4 = 5;

const unsigned REDLED_PIN = 6;

const unsigned BUTTON_PIN1 = 2;
const unsigned BUTTON_PIN2 = 12;
const unsigned BUTTON_PIN3 = 11;
const unsigned BUTTON_PIN4 = 10;

const unsigned REBOUND_TIME = 150;

enum difficulty { EASY, MEDIUM, HARD, ABSURD };

void setupHardware();

void fadingLed(unsigned long currentMillis);
void redLedOn();
void redLedOff();
void allLedOff();

void displayMessage(String message);

int getNumberFromBoard();

enum difficulty getDifficulty();

bool canStart();

void setCanStart(bool value);
