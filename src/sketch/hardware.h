#define GAME_PINS 4

#define LED_PIN1 1
#define LED_PIN2 2
#define LED_PIN3 3
#define LED_PIN4 4

#define REDLED_PIN 6

#define BUTTON_PIN1 10
#define BUTTON_PIN2 11
#define BUTTON_PIN3 12
#define BUTTON_PIN4 13

enum difficulty { EASY, MEDIUM, HARD, ABSURD };

void setupHardware();

void fadingLed();

void gameStart();   // TODO: This method should change interrupt of B1 from awake() to changeLed1State()

void displayMessage(char* message);

int getNumberFromBoard();

enum difficulty getDifficulty();
