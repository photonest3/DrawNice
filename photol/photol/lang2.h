///////////////////////////////////////////////////////////////////////
/// @file lang2.h
/// @brief PhotoNest 多语言支持头文件，目前使用vue.js
/// @details 实现多语言翻译功能:
///           - 语言初始化 (Unitialize)
///           - 语言处理 (proc_lang)
///           - 字符串翻译 (trans)
///           - 基于 XML 的翻译文件 (tinyxml)
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件使用 tinyxml 库解析翻译文件
///////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>


#include "tinyxml2.h"
using namespace std;


typedef struct _TEMPL_DTO
{
	string id;
	string val;
} TEMPL_DTO;


class CLang 
{
public:
	CLang(void);
	~CLang(void);

public:
	long Unitialize();

	bool proc_lang(wstring lang);
	string trans(string id);
	bool id2name(int id, wstring &lang);
	bool name2id(string lang, int &id);
	int ParseLangXML(string szhtml);

private:

	map<string, string> _langMap;

	//string procLangData(string common);

	//void StatusChange(string &fout, int &pattern, char &c, char &last);
	//long Read_file(HINSTANCE hrcInst, UINT cr_id, string &szhtml);
	long Read_file(wstring filename, string& szhtml);

	//void LogToFile(string text);
	long Parse(TiXmlNode* parent, TEMPL_DTO& dto);
	list<TiXmlNode*> GetElementsByTagName(TiXmlNode* parent, string name, list<TiXmlNode*>& list);
	string _GetNodeValue(TiXmlNode* node);

};
