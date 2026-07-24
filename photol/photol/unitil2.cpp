///////////////////////////////////////////////////////////////////////
/// @file unitil2.cpp
/// @brief PhotoNest 字符串与路径处理工具实现文件
/// @details 实现字符串编码转换、替换、分词、路径获取等实用函数:
///           - 宽字符/UTF-8 互转 (_w2u, _u2w)
///           - 字符串替换和修剪 (Replace, trim, ltrim, rtrim)
///           - 字符串分词 (tokenize)
///           - 十六进制转换和校验和 (hexchange, checksum)
///           - 模块路径和应用数据路径获取 (get_module_path, get_appdata_path)
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 的自定义工具模块
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "unitil2.h"
#include <codecvt>
#include <shlobj.h>


string _w2u(wstring w)
{
	wstring_convert<codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(w);
}

wstring _u2w(string u)
{
	wstring_convert<codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(u);
}

wstring Replace(wstring& szBody, wstring szOld, wstring szNew)
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

string ReplaceOne(string& szBody, string szOld, string szNew)
{
	if (szOld == szNew)
	{
		return szBody;
	}

	string::size_type nFound = string::npos;

	nFound = szBody.find(szOld, 0);
	if (nFound != string::npos)
	{
		szBody.replace(nFound, szOld.length(), szNew);
	}

	return szBody;
}
//
string trim(string& s, const string drop)
{
	// trim right
	s.erase(s.find_last_not_of(drop) + 1);
	// trim left
	return s.erase(0, s.find_first_not_of(drop));
}

string ltrim(string& s, const string drop)
{
	// trim left
	return s.erase(0, s.find_first_not_of(drop));
}

string rtrim(string& s, const string drop)
{
	// trim right
	return s.erase(s.find_last_not_of(drop) + 1);
}

string my_tolower(string& s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

//
vector<string> tokenize(const string& src, string tok, bool btrim, string null_subst)
{
	vector<string> v;
	if (src.empty() || tok.empty())
	{
		return v;
		//throw "tokenize: empty string\0";
	}

	string::size_type pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != string::npos)
	{
		if ((len = index - pre_index) != 0)
		{
			v.push_back(src.substr(pre_index, len));
		}
		else if (!btrim)
		{
			v.push_back(null_subst);
		}
		pre_index = index + 1;
	}

	string endstr = src.substr(pre_index);
	if (!btrim)
	{
		v.push_back(endstr.empty() ? null_subst : endstr);
	}
	else if (!endstr.empty())
	{
		v.push_back(endstr);
	}

	return v;
}

vector<wstring> tokenize(const wstring& src, wstring tok, bool btrim, wstring null_subst)
{
	vector<wstring> v;
	if (src.empty() || tok.empty())
	{
		return v;
		//throw "tokenize: empty string\0";
	}

	wstring::size_type pre_index = 0, index = 0, len = 0;
	while ((index = src.find_first_of(tok, pre_index)) != wstring::npos)
	{
		if ((len = index - pre_index) != 0)
		{
			v.push_back(src.substr(pre_index, len));
		}
		else if (!btrim)
		{
			v.push_back(null_subst);
		}
		pre_index = index + 1;
	}

	wstring endstr = src.substr(pre_index);
	if (!btrim)
	{
		v.push_back(endstr.empty() ? null_subst : endstr);
	}
	else if (!endstr.empty())
	{
		v.push_back(endstr);
	}

	return v;
}

int hexchange(unsigned char* dest, const char* src)
{
	int i;
	char buf[256 * 2 + 1];

	unsigned char* table = (unsigned char*)malloc(65536);
	if (table != NULL)
	{
		memset(table, 0, 65536);
		unsigned short* shortSrc = (unsigned short*)buf;

		for (i = 0; i <= 255; i++)
		{
			sprintf_s(buf + i * 2, 3, "%02x", i);
		}

		for (i = 0; i <= 255; i++)
		{
			table[shortSrc[i]] = i;
		}

		while (*src)
		{
			*dest = table[*((unsigned short*)src)];
			src += 2;
			dest++;
		}

		free(table);
	}
	return 0;
}

unsigned short checksum(USHORT* buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(USHORT);
	}
	if (size != 0)
	{
		cksum += *(UCHAR*)buffer;
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);

}

wstring get_module_path0(HMODULE hModule)
{
	wchar_t buf[MAX_PATH] = { 0 };
	long lSize = ::GetModuleFileNameW(hModule, buf, MAX_PATH);

	wchar_t* p = wcsrchr(buf, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}
	wstring s = buf;
	transform(s.begin(), s.end(), s.begin(), ::tolower);

	return s;

}
wstring get_module_path(HMODULE hModule)
{
	wchar_t buf[MAX_PATH] = { 0 };
	long lSize = ::GetModuleFileNameW(hModule, buf, MAX_PATH);

	wchar_t* p = wcsrchr(buf, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}

	p = wcsrchr(buf, L'\\');
	if (p != NULL)
	{
		p[0] = 0;
	}

	wstring s = buf;
	transform(s.begin(), s.end(), s.begin(), ::tolower);

	return s;

}


wstring get_appdata_path(string alb)
{
	std::wstring dst = L"";

	wchar_t* szDocument;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &szDocument)))
	{
		dst = szDocument;

		wstring p = get_module_path(NULL);
		wstring::size_type pos = p.find(L"\\windowsapps\\photonest");
		if (pos != wstring::npos)
		{
			dst += L"\\PhotoNest_App";
			dst += L"\\Nest1";
		}
		else
		{
			dst += L"\\PhotoNest";
			if (alb == "private")
			{
				dst += L"\\Nest2";
			}
			else
			{
				dst += L"\\Nest1";
			}
		}

		CoTaskMemFree(szDocument);
	}

	transform(dst.begin(), dst.end(), dst.begin(), ::tolower);
	return dst;
}

string Int2Str(int num)
{
	char buf[100] = { 0 };
	sprintf_s(buf, 100, "%d", num);

	return buf;
}
