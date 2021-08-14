#pragma once
#include "rw.h"

void Set2DScale();

class Camera {
private:
	int field0;
	int field4;
	int field8;
	int field12;
	int camera;
	float field20;
	float field24;
public:
	void Setup(float a1, float a2);
};

void Hook_RwCameraSetViewWindow(int camera, RwV2d* view);
void Hook_ChangeVideoMode(int mode);
void Hook_StartupScale();