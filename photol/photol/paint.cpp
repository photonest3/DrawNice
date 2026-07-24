// ============================================================
// paint.cpp : PhotoNest 绘图工具主程序入口
// 功能：支持将文件拖放到 exe 上直接打开，并使用互斥量防止多开
// ============================================================

#include <tchar.h>
#include <windows.h>

typedef unsigned int       uint32_t;   // 32位无符号整数类型别名
typedef unsigned long long uint64_t;   // 64位无符号整数类型别名

#include "../../photol/photol/photol.h" // 引入 photol.dll 导出函数

// 使用 Windows 子系统入口，避免弹出控制台窗口
#pragma comment(linker, "/subsystem:windows /ENTRY:wmainCRTStartup")
// 链接 photol 导入库
#pragma comment( lib, "../../../PhotosVO/tests/cefclient/Release/bin/photol.lib" )

// ------------------------------------------------------------
// 主函数：解析命令行参数并启动 paint 窗口
// 支持将文件拖放到 exe 上，argv[1] 为文件路径
// ------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t fname[MAX_PATH] = { 0 };

	if (argc > 1)
	{
		wcscpy_s(fname, argv[1]);
	}

	// 获取当前程序所在目录，用于构建唯一互斥量名称
	HMODULE hModule = ::GetModuleHandle(NULL);
	wchar_t buf[MAX_PATH] = { 0 };
	GetModuleFileNameW(hModule, buf, MAX_PATH);
	wchar_t* p = wcsrchr(buf, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}

	// 创建基于路径的唯一互斥量名称，防止多开
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
		// 互斥量已存在，说明已有实例在运行
	}
	else
	{
		// 获取屏幕 DPI
		HDC hDC = CreateCompatibleDC(0);
		int pixelsy = GetDeviceCaps(hDC, LOGPIXELSY);
		DeleteDC(hDC);

		// 调用 photol.dll 的 paint_show 打开绘图窗口
		int ret = paint_show(0, 0, pixelsy, 1152, 768, fname, L"", L"", L"", 0);
	}

	if (paint_mgr != NULL)
	{
		CloseHandle(paint_mgr);
	}

	return 0;
}

