#pragma once

#define ALT_MAIN_LOOP_HOOK 1

extern std::chrono::duration<long long, std::micro> gMaxFrameTime;

#define SET_DESIRED_RENDER_TIME() \
	gMaxFrameTime = std::chrono::duration<long long, std::micro>(long long(1000000.0 / (double)H2Config_fps_limit));

void InitRunLoop();
void DeinitRunLoop();
