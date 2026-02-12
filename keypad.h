#ifndef _KEYPAD_H_
#define _KEYPAD_H_
#include <Arduino.h>

char waitkey();
char scanKeys(); // Non-blocking check
void keypad_init();

#endif