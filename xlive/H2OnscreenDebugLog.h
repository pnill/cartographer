#pragma once

int getDebugTextArrayMaxLen();
void addDebugText(char* text);
void addDebugText(const char* text);
void addDebugText(wchar_t* wtext);
char* getDebugText(int ordered_index);
void initDebugText();

void setDebugTextDisplay(bool setOn);
bool getDebugTextDisplay();