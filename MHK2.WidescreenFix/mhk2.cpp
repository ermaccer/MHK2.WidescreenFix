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

void Hook_RwCameraSetViewWindow(int camera, RwV2d * view)
{
	float x = (float)*(int*)0x883700;
	float y = (float)*(int*)0x883704;
	float aspectRatio = (float)x / (float)y;

	float hud_y = 0.7f * (aspectRatio / (4.0f / 3.0f));


	float widthScale = 0.546875f;
	RwV2d newView;

	newView.x = hud_y + 0.039999999f;
	newView.y = widthScale + -0.059999999f;

	((void(__cdecl*)(int, RwV2d*))0x53BCD0)(camera, &newView);
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

void Camera::Setup(float a1, float a2)
{
	field20 = a1;
	field24 = a2;
	Hook_RwCameraSetViewWindow(camera, 0);
}
