#include "pch.h"
#include "mhk2.h"
#include "MemoryMgr.h"

using namespace Memory::VP;

void Set2DScale()
{
	int x = *(int*)0x883700;
	int y = *(int*)0x883704;
	float aspectRatio = (float)x / (float)y;

	float newScale = 1.0f / (float)x * ((float)y / 600.0f);
	Patch<float>(0x5965E4, newScale);
}

void Hook_ChangeVideoMode(int mode)
{
	((void(__cdecl*)(int))0x5338C0)(mode);
	Set2DScale();
}

void Hook_StartupScale()
{
	((void(__cdecl*)())0x45D1D0)();
	Set2DScale();
}

int GetShadowSize(eShadowsSettings shadowType)
{
	switch (shadowType)
	{
	case SHADOWS_NORMAL:
		return 128;
	case SHADOWS_HIGH:
		return 512;
	case SHADOWS_VERY_HIGH:
		return 2048;
	default:
		break;
	}
	return 0;
}

void Camera::Setup(float a1, float a2)
{
	field20 = a1;
	field24 = a2;
	CalculateNewViewWindow();
}

void Camera::Setup_SplitScreen(float a1, float a2)
{
	field20 = a1;
	field24 = a2;
	CalculateNewViewWindow();
}

void Camera::CalculateNewViewWindow()
{
	float x = (float)*(int*)0x883700;
	float y = (float)*(int*)0x883704;
	float aspectRatio = (float)x / (float)y;

	float _4_3 = 4.0f / 3.0f;
	float scaleRatio = aspectRatio / _4_3;
	RwV2d newView;

	float unk1 = ((double(__thiscall*)(Camera*)) * (int*)(*(int*)this + 36))(this);
	float unk2 = ((double(__thiscall*)(Camera*)) * (int*)(*(int*)this + 40))(this);

	float v5 = 1.0f;
	if (0.0 != unk1 && 0.0 != unk2)
		v5 = unk1 / unk2;

	newView.x = field20 * scaleRatio;
	newView.y = (field24 / v5) * scaleRatio;
	((void(__cdecl*)(int, RwV2d*))0x53BCD0)(camera, &newView);
}
