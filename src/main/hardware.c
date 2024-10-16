#include <math.h>
#include <EnableInterrupt.h>
#include <LiquidCrystal_I2C.h>
#include "hardware.h"

int ledStates[4] = {LOW, LOW, LOW, LOW};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

static void changeLedState();   // This method should be called when button's interrupt is raised
static String getBinaryNumber();

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

  Serial.begin(9600);   // for logging purpose
  
  // lcd initialisation
  lcd.init();
  lcd.backlight();

  // Interrupt setupped here
  enableInterrupt(BUTTON_PIN1, changeLed1State, RISING);
  enableInterrupt(BUTTON_PIN2, changeLed2State, RISING);
  enableInterrupt(BUTTON_PIN3, changeLed3State, RISING);
  enableInterrupt(BUTTON_PIN4, changeLed4State, RISING);
}

void blinkingLed() {
  currIntensity = 0;
  fadeAmount = 5;
  analogWrite(LED_PIN, currIntensity);
  currIntensity = currIntensity + fadeAmount;
  if (currIntensity == 0 || currIntensity == 255) {
    fadeAmount = -fadeAmount ;
  }
  delay(15);
}

void displayMessage(String msg) {
  lcd.clear();
  lcd.print(msg);
}

int getNumberFromBoard() {
  char bin[] = getBinaryNumber().c_str();
  int num = 0;
  for (int i = 0; i < GAME_PINS; i++) {
    num += bin[i] * pow(2, i);
  }
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
    digitalWrite(PIN_LED, ledState1);
}

static void changeLed2State() {
  ledState2 = ledState2 == HIGH ? LOW : HIGH;
  digitalWrite(PIN_LED, ledState2);
}

static void changeLed3State() {
  ledState3 = ledState3 == HIGH ? LOW : HIGH;
  digitalWrite(PIN_LED, ledState3);
}

static void changeLed4State() {
  ledState4 = ledState4 == HIGH ? LOW : HIGH;
  digitalWrite(PIN_LED, ledState4);
}

