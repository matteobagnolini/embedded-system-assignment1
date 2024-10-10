#include <math.h>
#include "hardware.h"

int ledStates[4] = {LOW, LOW, LOW, LOW};

static void changeLedState();   // This method should be called when button's interrupt is raised
static String getBinaryNumber();

void setupHardware() {
  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);

  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUTTON_PIN3, INPUT);
  pinMode(BUTTON_PIN4, INPUT);

  Serial.begin(9600);   // for logging purpose

  // Interrupt should be setupped here

}

void displayMessage(String msg) {
  // msg should be printed to the LCD display
  // For now we just print the msg on the serial line
  Serial.println(msg);
}

int getNumberFromBoard() {
  char bin[] = getBinaryNumber().c_str();
  int num = 0;
  for (int i = 0; i < GAME_PINS; i++) {
    num += bin[i] * pow(2, i);
  }
}

static String getBinaryNumber() {
  String bin = "";
  for (int i = 0; i < GAME_PINS; i++) {
    bin += ledStates[i] == HIGH ? 1 : 0;
  }
  Serial.println(bin);
}

static void changeLedState() {
  
}
