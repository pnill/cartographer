#pragma once
#define ALT_MAIN_LOOP_HOOK 1

extern std::chrono::steady_clock::duration desiredRenderTime;

void initGSRunLoop();
void deinitGSRunLoop();
