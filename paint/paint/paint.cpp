///////////////////////////////////////////////////////////////////////
/// @file paint.cpp
/// @brief PhotoNest 绘图应用入口程序
/// @details 实现绘图应用的 main 函数,支持将文件拖放到 exe 上直接打开
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是绘图应用的入口点,使用 photol 库进行图像处理
///////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <windows.h>

typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#include "../../photol/photol/photol.h"

#pragma comment(linker, "/subsystem:windows /ENTRY:wmainCRTStartup")
#pragma comment( lib, "../../../PhotoNest/tests/cefclient/Release/bin/photol.lib" )

int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t fname[MAX_PATH] = { 0 };

	if (argc > 1)
	{
		wcscpy_s(fname, argv[1]);
	}

	HMODULE hModule = ::GetModuleHandle(NULL);
	wchar_t buf[MAX_PATH] = { 0 };
	GetModuleFileNameW(hModule, buf, MAX_PATH);
	wchar_t* p = wcsrchr(buf, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}

	wchar_t buf2[MAX_PATH] = { 0 };
	swprintf_s(buf2, MAX_PATH, L"mypaint_%d", (int)wcslen(buf));

	HANDLE paint_mgr = NULL;
	paint_mgr = CreateMutex(
		(LPSECURITY_ATTRIBUTES)NULL,
		(BOOL)TRUE,
		buf2
	);

	DWORD dwLastError = GetLastError();

	if (paint_mgr == NULL || dwLastError == ERROR_ALREADY_EXISTS)
	{
	}
	else
	{
		HDC hDC = CreateCompatibleDC(0);
		int pixelsy = GetDeviceCaps(hDC, LOGPIXELSY);
		DeleteDC(hDC);

		int ret = paint_show(0, 0, pixelsy, 1152, 768, fname, L"", L"", L"", 0);
	}

	if (paint_mgr != NULL)
	{
		CloseHandle(paint_mgr);
	}

	return 0;
}

