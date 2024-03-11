/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


// Constants
#define LED1 6
#define LED2 11
#define LED3 10
#define LED4 17
#define BUTTON1 12
#define BUTTON2 8
#define BUTTON3 24
#define BUTTON4 18
#define N 2
#define MICRO_TIME 100 // Microseconds
#define HALF_TIME 500  // Milliseconds

const int BTN_PIN_RED = 2;
const int BTN_PIN_BLUE = 3;
const int BTN_PIN_YELLOW = 4;
const int BTN_PIN_GREEN = 5;

const int LED_PIN_RED = 16;
const int LED_PIN_BLUE = 17;
const int LED_PIN_YELLOW = 18;
const int LED_PIN_GREEN = 19;

const int BUZZER_PIN = 15;

// Function Prototypes
int getColor(void);
void playNote(int t);
void lightLED(int led, int t);
void playColor(int led, int t);
int getRandColor(void);
void playSequence(void);
void addColor(void);
void playEndGame(void);
void getColorSequence(void);
void beepToStart(void);
void setup(void);

// Variables
int colors[100]; // Increase size as needed
int colorsCount = 0;


void setup() {
    stdio_init_all();
    gpio_init(BUTTON1);
    gpio_set_dir(BUTTON1, GPIO_IN);
    gpio_pull_up(BUTTON1);
    // Repeat for other buttons and LEDs...
}

int getColor() {
    while (true) {
        if (!gpio_get(BUTTON1)) return LED1;
        // Repeat for other buttons...
    }
}

void playNote(int t) {
    gpio_put(N, 1);
    sleep_ms(t);
    gpio_put(N, 0);
}

void lightLED(int led, int t) {
    gpio_put(led, 1);
    sleep_ms(t);
    gpio_put(led, 0);
}

void playColor(int led, int t) {
    gpio_put(led, 1);
    gpio_put(N, 1);
    sleep_ms(t);
    gpio_put(N, 0);
    gpio_put(led, 0);
}

int getRandColor() {
    int randomInt = rand() % 4 + 1; // Generates a random number between 1 and 4
    switch(randomInt) {
        case 1: return LED1;
        case 2: return LED2;
        case 3: return LED3;
        case 4: return LED4;
        default: return LED1; // Default case should never happen
    }
}

void playSequence() {
    for(int i = 0; i < colorsCount; ++i) {
        playColor(colors[i], 300);
        sleep_ms(300);
    }
}

void addColor() {
    int randomColor = getRandColor();
    colors[colorsCount++] = randomColor;
}

void playEndGame() {
    printf("\n\n*** THE USER HAS LOST! ***\n\n");
    sleep_ms(100);

    // Assuming LED_LIST as a function to light all LEDs, this needs to be implemented
    // For simplicity, lighting up each LED individually
    gpio_put(LED1, 1);
    gpio_put(LED2, 1);
    gpio_put(LED3, 1);
    gpio_put(LED4, 1);
    for(int i = 0; i < 7; ++i) {
        playNote(150);
        sleep_ms(150);
    }
    gpio_put(LED1, 0);
    gpio_put(LED2, 0);
    gpio_put(LED3, 0);
    gpio_put(LED4, 0);
    
    // Clean up and exit
    // Note: Proper clean-up in C on a microcontroller often involves resetting peripherals to a known state.
    // For the Pico, there isn't a direct 'cleanup' function like in RPi.GPIO, so you'd manually reset the GPIOs if needed.
    // The 'exit' call is not typically used on microcontrollers; you might reset the MCU or enter a low-power state instead.
}

void getColorSequence() {
    int selectedColor = 0;
    for(int i = 0; i < colorsCount; ++i) {
        selectedColor = getColor();
        playColor(selectedColor, 300);
        if(selectedColor != colors[i]) {
            playEndGame();
            return; // Exit or reset the game
        }
    }
}

void beepToStart() {
    for(int i = 0; i < 5; ++i) {
        playNote(150);
        sleep_ms(150);
    }
    sleep_ms(1000);
}

int main() {
    setup();
    beepToStart();
    sleep_ms(1000);

    while (true) {
        addColor();
        playSequence();
        getColorSequence();
        sleep_ms(500);
    }

    return 0;
}
