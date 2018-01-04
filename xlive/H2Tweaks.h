#pragma once

#define CONTROLLER 1
#define MOUSE 0


void InitH2Tweaks();
void DeinitH2Tweaks();
void setSens(short input_type, int sens);
void setFOV(int field_of_view);
void setCrosshairPos(float crosshair_offset);