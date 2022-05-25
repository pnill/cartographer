#pragma once
#define ALT_MAIN_LOOP_HOOK 1

extern std::chrono::duration<double, std::nano> desiredRenderTime;
#define SET_DESIRED_RENDER_TIME() desiredRenderTime = std::chrono::duration<double, std::micro>((1000000.0 / (double)H2Config_fps_limit));

void InitRunLoop();
void DeinitRunLoop();
