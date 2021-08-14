// dllmain.cpp : Defines the entry point for the DLL application.

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



#include "pch.h"
#include "resource.h"
#include <Windows.h>
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#include <windowsx.h>
#include <d3d9.h>
#include <vector>
#include <algorithm>
#include "rw.h"
#include "MemoryMgr.h"
#include "mhk2.h"

using namespace Memory::VP;

int& m_overwriteVideoMode = *(int*)0x5C51B4;
std::vector<RwVideoMode> resolutions;



bool mode_compare(RwVideoMode first, RwVideoMode in)
{
	return first.height == in.height && first.width == in.width;
}


void GetVideoModes()
{
	IDirect3D9* dev = Direct3DCreate9(D3D_SDK_VERSION);

	if (dev)
	{
		for (unsigned int i = 0; i < dev->GetAdapterModeCount(0, D3DFMT_X8R8G8B8); i++)
		{
			D3DDISPLAYMODE mode;

			if (SUCCEEDED(dev->EnumAdapterModes(0, D3DFMT_X8R8G8B8, i, &mode)))
			{
				RwVideoMode rw_mode;
				rw_mode.width = mode.Width;
				rw_mode.height = mode.Height;
				rw_mode.format = mode.Format;
				rw_mode.refRate = mode.RefreshRate;
				rw_mode.flags = 0;
				resolutions.push_back(rw_mode);
			}
		
		}
		auto pos = std::unique(resolutions.begin(), resolutions.end(), mode_compare);
		resolutions.erase(pos, resolutions.end());

		dev->Release();
		dev = nullptr;
	}
	else
	{
		MessageBox(0, L"Failed to create Direct3D device!", L"Error", MB_ICONERROR);
	}
}

void WriteVideoMode(int mode)
{
	char value[64] = {};
	sprintf_s(value, "%d", mode);
	WritePrivateProfileStringA("Settings", "VideoMode", value, ".\\mhk2.ini");
}


INT_PTR CALLBACK Launcher(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		DWORD res;
		HWND list;

		res = GetPrivateProfileIntA("Settings", "VideoMode", 0, ".\\mhk2.ini");

		if (res > resolutions.size())
			res = 0;

		list = GetDlgItem(hDlg, IDC_COMBO1);
		SendMessage(list, CB_RESETCONTENT, 0, 0);

		for (unsigned int i = 0; i < resolutions.size(); i++)
		{
			wchar_t tmp[128] = {};
			wsprintf(tmp, L"%d x %d", resolutions[i].width, resolutions[i].height);
			ComboBox_AddString(list, tmp);
		}
		ComboBox_SetCurSel(list, res);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			DWORD mode;
			mode = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COMBO1));
			m_overwriteVideoMode = mode + 1;
			// change last resolution to custom one
			Patch<int>(0x4200E6 + 4, resolutions[mode].width);
			Patch<int>(0x4200EE + 4, resolutions[mode].height);
			Patch<int>(0x46F87C + 4, resolutions[mode].width);
			Patch<int>(0x46F884 + 4, resolutions[mode].height);

			WriteVideoMode(mode);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			WriteVideoMode(ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_COMBO1)));
			exit(0);
		}
		break;
	}
	return (INT_PTR)FALSE;
}



void Init()
{
	wchar_t	 moduleName[MAX_PATH] = {};
	GetModuleFileName((HINSTANCE)&__ImageBase, moduleName, MAX_PATH);
	GetVideoModes();
	DialogBox(GetModuleHandle(moduleName), MAKEINTRESOURCE(IDD_DIALOG1), 0, Launcher);

	// "modern" loading bar
	Patch<float>(0x5965D4, 325.0f);
	Patch<double>(0x5965C8, 325.0f);

	// in game
	Patch<void(__thiscall Camera::*)(float, float)>(0x597A6C,&Camera::Setup);
	// menu							 
	Patch<void(__thiscall Camera::*)(float, float)>(0x597D14,&Camera::Setup);

	InjectHook(0x41F8A4, Hook_ChangeVideoMode, PATCH_CALL);
	InjectHook(0x46FA98, Hook_StartupScale, PATCH_CALL);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		Init();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

