///////////////////////////////////////////////////////////////////////
/// @file dllmain.cpp
/// @brief PhotoL DLL 模块入口点，paint要用。
/// @details 实现 DLL 的入口点函数,处理 DLL 加载/卸载事件,
///          保存模块实例句柄到全局变量 _his
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoL DLL 的入口,必须在 DLL 加载时正确初始化
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
HINSTANCE _his = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		_his = hModule;
		break;
	}
	return TRUE;
}

