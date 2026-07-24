///////////////////////////////////////////////////////////////////////
/// @file unitil2.h
/// @brief PhotoNest 字符串与路径处理工具头文件,SchemeBackend vuejs
/// @details 提供字符串编码转换、替换、分词、路径获取等实用函数:
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

#pragma once

#include <string>
#include <cctype>
#include <algorithm>
#include <vector>


using namespace std;

string _w2u(wstring w);
wstring _u2w(string u);

wstring Replace(wstring& szBody, wstring szOld, wstring szNew);
string ReplaceOne(string& szBody, string szOld, string szNew);

string trim(string& s, const string drop);
string ltrim(string& s, const string drop);
string rtrim(string& s, const string drop);

string my_tolower(string& s);
vector<string> tokenize(const string& src, string tok, bool btrim, string null_subst);
vector<wstring> tokenize(const wstring& src, wstring tok, bool btrim, wstring null_subst);

int hexchange(unsigned char* dest, const char* src);
unsigned short checksum(USHORT* buffer, int size);

wstring get_module_path0(HMODULE hModule);
wstring get_module_path(HMODULE hModule);
wstring get_appdata_path(string alb);

string Int2Str(int num);
