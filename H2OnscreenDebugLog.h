#pragma once

int getDebugTextArrayMaxLen();
void addDebugText(char* text);
char* getDebugText(int ordered_index);
void initDebugText();

void setDebugTextDisplay(bool setOn);
bool getDebugTextDisplay();