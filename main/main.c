/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// LED Definitions
#define RED_LED 14
#define BLU_LED 15
#define YEL_LED 16
#define GRN_LED 17

// Pushbutton Definitions
#define RED_BTN 4
#define BLU_BTN 5
#define YEL_BTN 6
#define GRN_BTN 7

#define BUZZ_PIN 15

#define MAX_LEVEL 20

typedef enum {IDLE_S, PLAYSEQ_S, RIGHTSEQ_S, WRONGSEQ_S} state_e;
typedef enum {GRN_BTN_NOT_PRESSED, GRN_BTN_PRESSED, RIGHTSEQ, WRONGSEQ} event_e;

void gpio_led_init();
void gpio_button_init();
void idle_sequence();
event_e check_start();
void generate_sequence();
void play_sequence();
void play_wrong_sequence();
void get_sequence();
void play_right_sequence()
{
        gpio_put(GRN_LED, 0);
        gpio_put(RED_LED, 0);
        gpio_put(YEL_LED, 0);
        gpio_put(BLU_LED, 0);
        sleep_ms(500);
        gpio_put(GRN_LED, 1);
        gpio_put(RED_LED, 1);
        gpio_put(YEL_LED, 1);
        gpio_put(BLU_LED, 1);
        sleep_ms(500);
        gpio_put(GRN_LED, 0);
        gpio_put(RED_LED, 0);
        gpio_put(YEL_LED, 0);
        gpio_put(BLU_LED, 0);
        sleep_ms(500);
}

uint diff = 1000;
uint level = 1;
uint seq[MAX_LEVEL], playerseq[MAX_LEVEL];

state_e state = IDLE_S;
event_e event = GRN_BTN_NOT_PRESSED;

int main() {

    stdio_init_all(); 
    gpio_led_init();
    gpio_button_init();

    while (true) {
        switch(state){
            case IDLE_S:
                idle_sequence();
                if (GRN_BTN_PRESSED == check_start())
                {
                    state = PLAYSEQ_S;
                    generate_sequence();
                }
                break;

            case PLAYSEQ_S:
                    play_sequence();
                    get_sequence();
                break;

            case RIGHTSEQ_S:
                play_right_sequence();
                if (level < MAX_LEVEL)
                    level++;
                diff -= 100;
                state = PLAYSEQ_S;
                break;
            
            case WRONGSEQ_S:
                play_wrong_sequence();
                level = 1;
                diff = 1000;
                state = IDLE_S;
                break;

            default:
                break;
        }

    }
}

void gpio_led_init(){
    gpio_init(GRN_LED);
    gpio_set_dir(GRN_LED, GPIO_OUT);
    gpio_init(RED_LED);
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_init(YEL_LED);
    gpio_set_dir(YEL_LED, GPIO_OUT);
    gpio_init(BLU_LED);
    gpio_set_dir(BLU_LED, GPIO_OUT);
}

void gpio_button_init(){
    gpio_init(GRN_BTN);
    gpio_set_dir(GRN_BTN, GPIO_IN);
    gpio_pull_up(GRN_BTN);
    gpio_init(RED_BTN);
    gpio_set_dir(RED_BTN, GPIO_IN);
    gpio_pull_up(RED_BTN);
    gpio_init(YEL_BTN);
    gpio_set_dir(YEL_BTN, GPIO_IN);
    gpio_pull_up(YEL_BTN);
    gpio_init(BLU_BTN);
    gpio_set_dir(BLU_BTN, GPIO_IN);
    gpio_pull_up(BLU_BTN);
}

void idle_sequence(){
    // Flash the LEDs in sequence
    for (int i = GRN_LED; i < BLU_LED+1; i++) {
      gpio_put(i, 1);
      sleep_ms(100);
      gpio_put(i, 0);
    }
}

event_e check_start(){
    return (!gpio_get(GRN_BTN))?GRN_BTN_PRESSED:GRN_BTN_NOT_PRESSED;
}

void generate_sequence(){
    uint i;
    srand(time(NULL));
    for(i=0; i<MAX_LEVEL; i++ )
    {
        seq[i] = (rand() % (BLU_LED - GRN_LED)) + GRN_LED;
    }
}

void play_sequence()
{
    uint i;

    gpio_put(GRN_LED, 0);
    gpio_put(RED_LED, 0);
    gpio_put(YEL_LED, 0);
    gpio_put(BLU_LED, 0);

    for(i=0; i<level; i++ )
    {
        gpio_put(seq[i], 1);
        sleep_ms(diff);
        gpio_put(seq[i], 0);
        sleep_ms(200);
    }   
}

void play_wrong_sequence(){
    uint i;
    for(i=0; i<3; i++ )
    {
        gpio_put(GRN_LED, 1);
        gpio_put(RED_LED, 1);
        gpio_put(YEL_LED, 1);
        gpio_put(BLU_LED, 1);
        sleep_ms(500);
        gpio_put(GRN_LED, 0);
        gpio_put(RED_LED, 0);
        gpio_put(YEL_LED, 0);
        gpio_put(BLU_LED, 0);
        sleep_ms(500);
    }
}

void get_sequence(){
    uint i, flag=0;

    for(i=0; i<level; i++ )
    {
        flag = 0;
        while(!flag)
        {
            if (!gpio_get(GRN_BTN))
            {
                gpio_put(GRN_LED, 1);
                sleep_ms(500);
                gpio_put(GRN_LED, 0);
                sleep_ms(200);
                flag = 1;
                playerseq[i] = GRN_LED;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    state = WRONGSEQ_S;
                    return;
                }
            }
            else if (!gpio_get(RED_BTN))
            {
                gpio_put(RED_LED, 1);
                sleep_ms(500);
                gpio_put(RED_LED, 0);
                sleep_ms(200);
                flag = 1;
                playerseq[i] = RED_LED;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    state = WRONGSEQ_S;
                    return;
                }
            }
            else if (!gpio_get(YEL_BTN))
            {
                gpio_put(YEL_LED, 1);
                sleep_ms(500);
                gpio_put(YEL_LED, 0);
                sleep_ms(200);
                flag = 1;
                playerseq[i] = YEL_LED;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    state = WRONGSEQ_S;
                    return;
                }
            }
            else if (!gpio_get(BLU_BTN))
            {
                gpio_put(BLU_LED, 1);
                sleep_ms(500);
                gpio_put(BLU_LED, 0);
                sleep_ms(200);
                flag = 1;
                playerseq[i] = BLU_LED;
                if (playerseq[i] != seq[i])
                {
                    event = WRONGSEQ;
                    state = WRONGSEQ_S;
                    return;
                }
            }
        }
    }
    event = RIGHTSEQ;
    state = RIGHTSEQ_S;
}