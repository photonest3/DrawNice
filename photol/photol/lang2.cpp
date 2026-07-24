#include "StdAfx.h"
#include "lang2.h"
#include "unitil2.h"
#include "resource.h"
#include "../../locales/locales/resource.h"
#include "../../locales/locales/locales.h"
#pragma comment( lib, "../../../PhotoNest/tests/cefclient/Release/bin/locales.lib" ) 
#include"tinyxml2.h"

CLang::CLang(void)
{
}

CLang::~CLang(void)
{
}

long CLang::Unitialize()
{
	_langMap.clear();
	return 0;
}

bool CLang::proc_lang(wstring lang)
{
	//wstring	path = L"locales/paint/0_";
	//path += lang + L".xml";

	UINT id_0 = IDR_0_EN_XML;

	if (lang == L"de")
	{
		id_0 = IDR_0_DE_XML;
	}
	else if (lang == L"en")
	{
		id_0 = IDR_0_EN_XML;
	}
	else if (lang == L"es")
	{
		id_0 = IDR_0_ES_XML;
	}
	else if (lang == L"fr")
	{
		id_0 = IDR_0_FR_XML;
	}
	else if (lang == L"it")
	{
		id_0 = IDR_0_IT_XML;
	}
	else if (lang == L"ja")
	{
		id_0 = IDR_0_JA_XML;
	}
	else if (lang == L"ru")
	{
		id_0 = IDR_0_RU_XML;
	}
	else if (lang == L"zh")
	{
		id_0 = IDR_0_ZH_XML;
	}





	string parData = "";
	//Read_file(path, parData);

	wstring fname = L"locales/album/0_" + lang + L".xml";
	char* szResponse = 0;
	int ret = Read_xml(id_0, fname.c_str(), &szResponse);
	if (ret == 0)
	{
		parData = szResponse;
		LocalFree(szResponse);
	}

	ParseLangXML(parData);
	return true;
}

bool CLang::id2name(int id, wstring& lang)
{
	switch (id)
	{
	case 0:
		lang = L"de";
		break;
	case 1:
		lang = L"en";
		break;
	case 2:
		lang = L"es";
		break;
	case 3:
		lang = L"fr";
		break;
	case 4:
		lang = L"it";
		break;
	case 5:
		lang = L"ja";
		break;
	case 6:
		lang = L"ru";
		break;
	case 7:
		lang = L"zh";
		break;
	default:
		break;
	}
	return true;
}

bool CLang::name2id(string lang, int& id)
{
	if (lang == "de")
	{
		id = 0;
	}
	else if (lang == "en")
	{
		id = 1;
	}
	else if (lang == "es")
	{
		id = 2;
	}
	else if (lang == "fr")
	{
		id = 3;
	}
	else if (lang == "it")
	{
		id = 4;
	}
	else if (lang == "ja")
	{
		id = 5;
	}
	else if (lang == "ru")
	{
		id = 6;
	}
	else if (lang == "zh")
	{
		id = 7;
	}
	return true;
}

string CLang::trans(string id)
{
	string val = id;
	map<string, string>::iterator iter;
	for (iter = _langMap.begin(); iter != _langMap.end(); iter++)
	{
		if (iter->first == id)
		{
			if (!iter->second.empty())
			{
				val = iter->second;
			}
			break;
		}
	}
	return val;
}

long CLang::Read_file(wstring filename, string& szhtml)
{
	szhtml = "";
	wstring szPath = get_module_path0(NULL);
	wstring	s1 = szPath + L"/";
	s1 += filename;

	HANDLE hFile = CreateFile(s1.c_str(), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		// Retrive file size
		int lengthActual = GetFileSize(hFile, NULL);
		DWORD dwNumBytesRead = 0;

		char* pBuf = (char*)malloc(lengthActual + 1);
		if (pBuf != NULL)
		{
			memset(pBuf, 0, lengthActual + 1);

			BOOL bResult = ReadFile(hFile, pBuf, lengthActual, &dwNumBytesRead, 0);
			if (bResult)
			{
				szhtml = string(pBuf, lengthActual);
			}

			free(pBuf);
		}

		CloseHandle(hFile);
	}
	return 0;
}

//void CLang::LogToFile(string text)
//{
//	SYSTEMTIME st;
//	GetLocalTime(&st);
//
//	int len = text.size() + 1024;
//
//	char* ptr = (char*)malloc(len);
//	if (ptr == NULL)
//	{
//		return;
//	}
//
//	memset(ptr, 0, len);
//	sprintf_s(ptr, len, "%u/%u/%04u %02u:%02u:%02u %s\r\n", st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond, text.c_str());
//
//	string temp = ptr;
//	free(ptr);
//
//	HANDLE hFile = CreateFile(L"e:\\paint.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
//		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//	if (hFile != INVALID_HANDLE_VALUE)
//	{
//		int lengthActual = GetFileSize(hFile, NULL);
//		if (lengthActual > 1024 * 1000 * 20)
//		{
//			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
//			SetEndOfFile(hFile);
//		}
//		else
//		{
//			SetFilePointer(hFile, 0, NULL, FILE_END);
//		}
//
//		DWORD dwWritten = 0;
//		BOOL bResult = WriteFile(hFile, temp.c_str(), temp.length(), &dwWritten, NULL);
//
//		CloseHandle(hFile);
//
//	}
//}

int CLang::ParseLangXML(string szhtml)
{
	_langMap.clear();

	XMLDocument pDoc;
	pDoc.LoadFile("szhtml");
	//tiny 1 pDoc->Parse(szhtml.c_str(), NULL, TIXML_ENCODING_UTF8);

	if (pDoc->Error())
	{
		delete pDoc;
		return 1;
	}

	TiXmlNode* root = pDoc;

	list<TiXmlNode*> n1;
	GetElementsByTagName(root, "item", n1);

	if (!n1.empty())
	{
		int index = 0;
		list<TiXmlNode*>::iterator it;
		for (it = n1.begin(); it != n1.end(); it++)
		{
			TiXmlNode* ev = (TiXmlNode*)*it;

			TEMPL_DTO dto;
			int ret = Parse(ev, dto);
			if (ret == 0)
			{
				_langMap.insert(map<string, string>::value_type(dto.id, dto.val));
			}
		}
	}

	delete pDoc;

	return 0;
}

long CLang::Parse(TiXmlNode* parent, TEMPL_DTO& dto)
{
	if (!parent) return 1;

	TiXmlNode* pChild;

	for (pChild = parent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		string val = pChild->ValueStr();

		if (val == "id")
		{
			dto.id = _GetNodeValue(pChild);
		}
		else if (val == "transl")
		{
			dto.val = _GetNodeValue(pChild);
		}
	}

	return 0;
}

list<TiXmlNode*> CLang::GetElementsByTagName(TiXmlNode* parent, string name, list<TiXmlNode*>& list)
{
	if (!parent)
	{
		return list;
	}

	string val = parent->ValueStr();
	if (val == name)
	{
		list.push_back(parent);
	}

	TiXmlNode* pChild;
	for (pChild = parent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		GetElementsByTagName(pChild, name, list);
	}

	return list;
}

string CLang::_GetNodeValue(TiXmlNode* node)
{
	string txt = node->FirstChild() != NULL ? node->FirstChild()->Value() : "";
	//if (txt.empty())
	//{
	//	txt = node->Value();
	//}

	return txt;
}
