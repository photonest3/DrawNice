///////////////////////////////////////////////////////////////////////
/// @file MySoft2.cpp
/// @brief 软件许可证验证功能实现(增强版)
/// @details 实现软件许可证的验证、注册码校验、授权模式判断等功能,
///          是 MySoft.h 中定义的 CMySoft 类的实现文件
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理软件授权验证,确保合法使用
///////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "MySoft2.h"
#include <time.h>
#include "unitil2.h"
#include "Blowfish.h"

CMySoft::CMySoft(void)
{
}

CMySoft::~CMySoft(void)
{
}

//return ERROR_LIC || MODE_STD || ret == MODE_PRO || ERROR_SIGN
int CMySoft::Verify(string s2, lic_header &lic)
{
	string serial = s2;

	if (serial.empty())
	{
		return ERROR_LIC;
	}

	int ret = Reg(serial, lic);
	char name[30];
	for (int i = 0; i < 200; i++)
	{
		ScrubBlob(name, 30);
	}
	//good
	if (ret == MODE_EXE)
	{
		return 0;
	}
	else
	{
		return ERROR_LIC;
	}
	return ret;
}

int CMySoft::Reg(string s2, lic_header &lic)
{
	s2 = my_tolower(s2);
	unsigned int len = 0;
	unsigned int i, j;

	char sn[256] = { 0 };
	strncpy_s(sn, 255, s2.c_str(), 255);
	len = (unsigned int)strlen(sn);

	for (i = 0, j = 0; i < len; i++)
	{
		if (isxdigit(sn[i]) == 0)
		{
			j = 1;
			break;
		}
	}

	if (len == 32 && j != 1 && len != 0)
	{
		CBlowfish  m_BF;

		char buf[10] = { 0 };
		sprintf_s(buf, 10, "%d%d", 2022, 3567);

		m_BF.SetPassword(buf);

		unsigned char hexsn[1024] = { 0 };
		hexchange(hexsn, sn);

		m_BF.Decrypt(hexsn, sizeof(lic_header));

		unsigned short sum = checksum((USHORT*)hexsn, sizeof(lic_header));
		if (sum == 0)
		{
			lic_header *p = (lic_header*)hexsn;
			if (p->version >= 20 && p->num > 0 && p->num < 99999)
			{
				lic.version = p->version;
				lic.mode = p->mode;
				lic.num = p->num;
				lic.chksum = p->chksum;
				lic.ov = p->ov;
				lic.year = p->year;
				lic.date = p->date;
				//u = p->users;
				//if (MODE_PRO == p->mode)
				//{
				//	u = 10000;
				//}
				return p->mode;
			}
		}
	}

	return ERROR_LIC;
}

void CMySoft::ScrubBlob(void *b, DWORD cb)
{
	for (int i = 0; i < 7; i++)
	{
		memset(b, 0xFF, cb); // all 1's
		memset(b, 0x00, cb); // all 0's
		memset(b, 0xAA, cb); // 10101010
		memset(b, 0x55, cb); // 01010101
	}
	ZeroMemory(b, cb);
}

void CMySoft::get_first(string &first)
{
	char *szSubKey = "Software\\Wow6432Node\\PhotoNest\\Cutout";
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		char buf[1024] = { 0 };
		DWORD dwSize = 1024;
		DWORD dwType = REG_SZ;

		int ret = RegQueryValueExA(hKey, "i3", NULL, &dwType, (LPBYTE)buf, &dwSize);
		first = buf;

		RegCloseKey(hKey);
	}
}

void CMySoft::get_lic(string & v0, string & v3)
{
	char *szSubKey = "Software\\OpenCV\\HighGUI\\Windows\\ZK1";
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 1024;
		char buf[1024] = { 0 };
		DWORD dwType = REG_SZ;
		long ret = RegQueryValueExA(hKey, "wo3", NULL, &dwType, (LPBYTE)buf, &dwSize);
		v3 = buf;

		dwSize = 1024;
		memset(buf, 0, 1024);
		ret = RegQueryValueExA(hKey, "thick", NULL, &dwType, (LPBYTE)buf, &dwSize);
		v0 = buf;

		RegCloseKey(hKey);
	}
}

long CMySoft::add_lic(string key, string val)
{
	char *szSubKey = "Software\\OpenCV\\HighGUI\\Windows\\ZK1";
	HKEY hKey;

	//DWORD dwDisposition = REG_OPENED_EXISTING_KEY;
	LONG lRet = RegCreateKeyExA(
		HKEY_CURRENT_USER,
		szSubKey,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL
	);

	if (lRet == ERROR_SUCCESS)
	{
		RegSetValueExA(hKey, key.c_str(), 0, REG_SZ, (BYTE*)val.c_str(), (DWORD)val.size());

		RegCloseKey(hKey);
	}

	if (key == "wo3")
	{
		lic_header lic;
		Reg(val, lic);
		WriteLic(lic);
	}
	return 0;
}

int CMySoft::get_verify(string& code, lic_header &lic, int &ov)
{
	time_t current_time;
	time(&current_time);

	string v0 = "";
	string v3 = "";
	get_lic(v0, v3);

	if (v0 == "")
	{
		lic_header olic;
		ReadLic(olic);
		unsigned short sum = checksum((USHORT*)(&olic), sizeof(lic_header));
		if (sum == 0)
		{
			olic.chksum = 0;
			Grn(olic, v3);
		}
	}

	int ret = Verify(v3, lic);
	if (ret == 0)
	{
		code = v3;
		ov = 2;

		time_t current_time;
		time(&current_time);

		if (lic.ov == 1)
		{
			ov = 1;
		}
		else
		{
			int day = abs(static_cast<int>(current_time - lic.date));

			if (lic.year == 0)
			{
				if (day > 30 * EXP_SPAN)
				{
					ov = 1;
				}
				else
				{

				}
			}
			else if (day > 365 * EXP_SPAN * lic.year)
			{
				ov = 1;
			}
		}

		lic.chksum = 0;
		if (ov == 1)
		{
			lic.ov = 1;
		}

		string sLic = "";
		Grn(lic, sLic);
		add_lic("wo3", sLic);
	}
	else
	{
		lic.version = 20;
		lic.mode = 30;
		lic.num = 1;
		lic.chksum = 0;
		lic.ov = 0;
		lic.year = 0;
		lic.date = current_time;

		string first1 = "";
		get_first(first1);
		time_t t1 = _atoi64(first1.c_str());
		if (t1 != 0 && t1 < current_time)
		{
			lic.date = t1;
			int day = abs(static_cast<int>(current_time - lic.date));
			if (day > 30 * EXP_SPAN)
			{
				ov = 1;
				lic.ov = 1;
			}
		}

		string sLic = "";
		Grn(lic, sLic);
		add_lic("wo3", sLic);
	}

	return ret;
}

BOOL CMySoft::Grn(lic_header lic, string& sLic)
{
	CBlowfish  m_BF;

	char buf0[10] = { 0 };
	sprintf_s(buf0, 10, "%d%d", 2022, 3567);

	m_BF.SetPassword(buf0);

	unsigned short sum = checksum((USHORT*)(&lic), sizeof(lic_header));
	lic.chksum = sum;

	unsigned char* ptr = (unsigned char*)&lic;

	m_BF.Encrypt(ptr, sizeof(lic_header));

	char buf[100] = { 0 }, tmp[20] = { 0 };

	for (int i = 0; i < sizeof(lic_header); i++)
	{
		memset(tmp, 0, 20);
		sprintf_s(tmp, "%02x", ptr[i]);
		strcat_s(buf, tmp);
	}

	sLic = buf;

	return TRUE;
}

int CMySoft::ReadLic(lic_header &lic)
{
	wstring szPath = get_module_path0(NULL);
	wstring	s1 = szPath + L"\\locales\\photol.dat";

	HANDLE hFile = CreateFile(s1.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwRead = 0;
		BOOL b = ReadFile(hFile, &lic, sizeof(lic_header), &dwRead, 0);
		CloseHandle(hFile);
	}

	return 0;
}

int CMySoft::WriteLic(lic_header lic)
{
	wstring szPath = get_module_path0(NULL);
	wstring	s1 = szPath + L"\\locales\\photol.dat";

	WIN32_FIND_DATAW ffd;
	HANDLE hFind;

	hFind = FindFirstFile(s1.c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind);
		HANDLE hWrite = CreateFile(s1.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (hWrite != INVALID_HANDLE_VALUE)
		{
			DWORD dwWritten = 0;
			WriteFile(hWrite, &lic, sizeof(lic_header), &dwWritten, NULL);

			::SetFileTime(hWrite, &ffd.ftCreationTime, &ffd.ftLastAccessTime, &ffd.ftLastWriteTime);
			CloseHandle(hWrite);
		}
	}

	return 0;
}
