#pragma once
#include "rw.h"

void Set2DScale();

enum eShadowsSettings {
	SHADOWS_NORMAL,
	SHADOWS_HIGH,
	SHADOWS_VERY_HIGH,
	SHADOWS_TOTAL_SETTINGS
};

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
	void Setup_SplitScreen(float a1, float a2);

	void CalculateNewViewWindow();
};

void Hook_ChangeVideoMode(int mode);
void Hook_StartupScale();

int GetShadowSize(eShadowsSettings shadowType);