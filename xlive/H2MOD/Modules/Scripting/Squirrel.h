#pragma once
#include <sqrat/include/sqrat.h>

void BindSquirrel(HSQUIRRELVM vm);
void sqSessionEnd();
void sqSessionStart();
void sqLoadScript(std::string);