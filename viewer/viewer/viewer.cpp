///////////////////////////////////////////////////////////////////////
/// @file viewer.cpp
/// @brief PhotoNest 图像查看器主程序
/// @details 实现图像查看器入口,启动 nest1.exe 并传递 URL 参数,
///          支持命令行参数指定初始文件或 URL
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 图像查看器的入口程序
///////////////////////////////////////////////////////////////////////

// viewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <string>

#pragma comment(linker, "/subsystem:windows /ENTRY:wmainCRTStartup")

using namespace std;

wstring Replace(wstring &szBody, wstring szOld, wstring szNew)
{
	if (szOld == szNew)
	{
		return szBody;
	}

	size_t nLen = szNew.length();

	wstring::size_type nFound = wstring::npos;
	do
	{
		nFound = szBody.find(szOld.c_str(), nFound != wstring::npos ? nFound + nLen : 0);
		if (nFound != wstring::npos)
		{
			szBody.replace(nFound, szOld.length(), szNew);
		}
	} while (nFound != wstring::npos);

	return szBody;
}


int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t buf2[MAX_PATH] = { 0 };
	long lSize = ::GetModuleFileName(NULL, buf2, MAX_PATH);

	wchar_t* p = wcsrchr(buf2, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}
	wcscat_s(buf2, L"\\nest1.exe");

	wchar_t buf[1024] = { 0 };
	wcscpy_s(buf, L"--url=http://nest1.pc/view.html");

	if (argc > 1)
	{
		wstring szBody = argv[1];
		szBody = Replace(szBody, L" ", L"%2B");

		wcscat_s(buf, L"?path=");
		wcscat_s(buf, szBody.c_str());
	}

	ShellExecute(NULL, NULL, buf2, buf, NULL, SW_SHOWNORMAL);
	return 0;
}

