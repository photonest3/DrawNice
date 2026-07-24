// ============================================================
// cutout.cpp : 抠图工具主程序入口
// 功能：启动 Windows 窗口子系统，调用 photol.dll 提供的 paint_show 接口展示抠图界面
// ============================================================

#include <tchar.h>
#include <windows.h>

typedef unsigned int       uint32_t;   // 32位无符号整数类型别名
typedef unsigned long long uint64_t;   // 64位无符号整数类型别名

#include "../../photol/photol/photol.h" // 引入 photol.dll 的导出函数声明

// 使用 Windows 子系统入口（避免控制台窗口弹出），入口函数为 wmainCRTStartup
#pragma comment(linker, "/subsystem:windows /ENTRY:wmainCRTStartup")

// 链接 photol 导入库，以便调用 paint_show 等导出函数
#pragma comment( lib, "../../../PhotoNest/tests/cefclient/Release/bin/photol.lib" )

// ------------------------------------------------------------
// 主函数：解析命令行参数并启动抠图界面
// argc / argv : 命令行参数（支持传入初始文件路径）
// 返回值 : 固定返回 0
// ------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t buf[1024] = { 0 };

	// 如果命令行传入了文件路径，则拷贝到缓冲区
	if (argc > 1)
	{
		wcscpy_s(buf, argv[1]);
	}

	// 获取屏幕纵向 DPI，用于后续界面缩放
	HDC hDC = CreateCompatibleDC(0);
	int pixelsy = GetDeviceCaps(hDC, LOGPIXELSY);
	DeleteDC(hDC);

	// 调用 photol.dll 的 paint_show 函数打开抠图窗口
	// 参数说明：父窗口句柄(0), DPI, 窗口宽度(1152), 窗口高度(768), 初始文件路径, 其余为空
	int ret = paint_show(0, 0, pixelsy, 1152, 768, buf, L"", L"", L"", 0);
	return 0;
}

