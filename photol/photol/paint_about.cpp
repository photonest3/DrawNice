///////////////////////////////////////////////////////////////////////
/// @file paint_about.cpp
/// @brief 绘图模块 - 关于对话框和注册界面实现
/// @details 实现关于对话框、软件注册界面、许可证验证等功能的
///          窗口过程处理和用户交互逻辑
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件包含 Windows 对话框处理和注册码验证逻辑
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
//#include "MySoft2.h"
#include <shlobj.h>
#include "SimpleIni.h"
#include <process.h>
#include "unitil3.h"

INT_PTR CALLBACK CPaint::Register(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	int wmId, wmEvent;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CPaint* self = (CPaint*)lParam;
		SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		//self->on_init_register_wnd(hDlg);
		SendMessage(GetDlgItem(hDlg, IDC_EDIT_CODE), EM_LIMITTEXT, 48, 0L);

	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_BUTTON_REGISTER:
		{
			CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
			int ret = 0;// self->on_register(hDlg);
			if (ret == 0)
			{
				EndDialog(hDlg, LOWORD(wParam));
			}
		}
		return (INT_PTR)TRUE;
		case IDC_BUTTON_BUY:
		{
			wstring buyURL = L"";

			if (buyURL.empty())
			{
				buyURL = L"https://www.photonest.io/index.html#subscribe";
			}

			ShellExecuteW(NULL, L"open", buyURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		return (INT_PTR)TRUE;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK CPaint::Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);

	int wmId, wmEvent;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CPaint* self = (CPaint*)lParam;
		SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		//self->on_init_settings_wnd(hDlg);
	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_COMBO_LANG:
		{
			CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
			if (wmEvent == CBN_SELCHANGE)
			{

			}
		}
		return (INT_PTR)TRUE;
		case IDOK:
		{
			CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));

			int nSel = (int)SendMessage(GetDlgItem(hDlg, IDC_COMBO_LANG), CB_GETCURSEL, 0, 0);
			wstring lang = L"en";
			self->_lang.id2name(nSel, lang);

			self->WritePrivate("language", _w2u(lang).c_str());
			int pixelsy = GetDeviceCaps(self->_hDC, LOGPIXELSY);
			self->init2(self->_app_type, pixelsy, lang.c_str(), self->_usr_lic.c_str(), self->_ov, false);


			////permissions need admin
			//HKEY hkey;
			//long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PhotoNest Cutout_is1", 0,
			//	KEY_SET_VALUE | KEY_WOW64_64KEY, &hkey);
			//if (ret == ERROR_SUCCESS)
			//{
			//	wchar_t buf[100] = { 0 };
			//	wcscpy_s(buf, 100, lang.c_str());
			//	RegSetValueEx(hkey, L"Inno Setup: Language", 0, REG_SZ, (BYTE*)buf, wcslen(buf) * sizeof(wchar_t));
			//	RegCloseKey(hkey);
			//}

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK CPaint::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
	{
		CPaint* self = (CPaint*)lParam;
		SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		//self->on_init_about_wnd(hDlg);
	}
	return (INT_PTR)TRUE;

	case WM_CTLCOLORSTATIC:
	{
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_STATIC_THIRD))
		{
			SetTextColor((HDC)wParam, RGB(0, 0, 255));
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (INT_PTR)GetStockObject((NULL_BRUSH));
		}
	}
	return (INT_PTR)0;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_STATIC_THIRD)
		{
			wstring szFileName = get_module_path0(NULL);
			szFileName += L"\\help\\cutout.html";

			ShellExecuteW(NULL, L"open", szFileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void CPaint::transl2(HWND hDlg, UINT id, int isBtn)
{
	wchar_t buf[2048] = { 0 };
	GetDlgItemText(hDlg, id, buf, 2048);

	string t = _lang.trans(_w2u(buf));
	wstring sz = _u2w(t);

	::SetDlgItemText(hDlg, id, sz.c_str());

	if (isBtn == 1)
	{
		HFONT control_font = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
		HFONT old = (HFONT)SelectObject(_hDC, control_font);
		SIZE size;
		GetTextExtentPoint32(_hDC, sz.c_str(), (int)sz.size(), &size);
		SelectObject(_hDC, old);

		HWND hwnd = ::GetDlgItem(hDlg, id);
		RECT rcWin;
		::GetWindowRect(hwnd, &rcWin);

		int newcx = size.cx;
		if (newcx > rcWin.right - rcWin.left - 50)
		{
			POINT pt;
			pt.x = rcWin.left;
			pt.y = rcWin.top;
			if (id == IDOK)
			{
				pt.x = rcWin.right - newcx - 50;
			}
			ScreenToClient(hDlg, &pt);
			MoveWindow(hwnd, pt.x, pt.y, newcx + 50, rcWin.bottom - rcWin.top, TRUE);
		}
	}
}
/*
void CPaint::on_init_register_wnd(HWND hDlg)
{
	string tit = "";
	string exp = "";
	get_license(tit, exp);
	SetDlgItemText(hDlg, IDC_STATIC_TIP3, _u2w(exp).c_str());
	SetDlgItemText(hDlg, IDC_STATIC_30, _u2w(tit).c_str());

	transl2(hDlg, IDC_STATIC_30);
	transl2(hDlg, IDC_BUTTON_BUY, 1);
	transl2(hDlg, IDC_BUTTON_REGISTER, 1);
	transl2(hDlg, IDOK);

	if (exp == "")
	{
		//ShowWindow(GetDlgItem(hDlg, IDC_STATIC_30), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_EDIT_CODE), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_BUTTON_REGISTER), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_BUTTON_BUY), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_TIP3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC0), SW_HIDE);

		ShowWindow(GetDlgItem(hDlg, IDC_STATIC_TIP), SW_HIDE);
		SetDlgItemText(hDlg, IDC_STATIC_TIP, L"Activated.");
	}
	transl2(hDlg, IDC_STATIC_TIP);
	SetWindowText(hDlg, _u2w(_lang.trans("Registration")).c_str());
}

void CPaint::on_init_settings_wnd(HWND hDlg)
{
	//transl2(hDlg, IDC_STATIC_LANG);
	transl2(hDlg, IDOK);
	transl2(hDlg, IDCANCEL);
	::SetWindowText(hDlg, _u2w(_lang.trans("Language")).c_str());

	const wchar_t* tem[] = { L"Deutsch",L"English",L"Español",L"Français",L"Italiano",L"日本語",L"Русский",L"简体中文" };
	HWND hCombo1 = GetDlgItem(hDlg, IDC_COMBO_LANG);

	for (int i = 0; i < 8; i++)
	{
		SendMessage(hCombo1, CB_ADDSTRING, i, (LPARAM)tem[i]);
	}

	int id = 0;
	_lang.name2id(_w2u(_language), id);

	SendMessage(hCombo1, CB_SETCURSEL, id, 0);

}

void CPaint::on_init_about_wnd(HWND hDlg)
{
	if (_app_type == FROM_EXE)
	{
		string tit = "";
		string exp = "";
		get_license(tit, exp);
		SetDlgItemText(hDlg, IDC_EDIT_LIC, _u2w(tit + "\r\n" + exp).c_str());
	}

	transl2(hDlg, IDC_STATIC_NAME);
	transl2(hDlg, IDC_STATIC_COPY);
	transl2(hDlg, IDC_STATIC_LIC);
	transl2(hDlg, IDC_STATIC_WARN);
	transl2(hDlg, IDC_STATIC_THIRD);
	transl2(hDlg, IDOK);
	::SetWindowText(hDlg, _u2w(_lang.trans("About")).c_str());

	//if (PRODUCT_NAME == L"PhotoNest Cutout")
	{
		//IDC_STATIC_LIC
		//IDC_EDIT_LIC
	}
}

int CPaint::on_register(HWND hDlg)
{
	char code_buf[MAX_PATH] = { 0 };
	UINT num = GetDlgItemTextA(hDlg, IDC_EDIT_CODE, code_buf, MAX_PATH);

	if (strlen(code_buf) > 0)
	{
		string title = _lang.trans("Registration");

		lic_header lic;
		CMySoft soft;
		int ret = soft.Verify(code_buf, lic);
		if (ret == 0)
		{
			bool b = true;
			string v0 = "";
			string v3 = "";
			soft.get_lic(v0, v3);

			if (v0 == "")
			{
				lic_header olic;
				soft.ReadLic(olic);
				unsigned short sum = checksum((USHORT*)(&olic), sizeof(lic_header));
				if (sum == 0)
				{
					olic.chksum = 0;
					soft.Grn(olic, v3);
				}
			}

			if (v3 != "")
			{
				lic_header oldLic;
				soft.Reg(v3, oldLic);
				if (oldLic.year != 0)
				{
					if (oldLic.num == lic.num)
					{
						b = false;
					}
				}

				string t0 = Int2Str(lic.num);
				vector<string> v1 = tokenize(v0, ",", true, "");
				int len = (int)v1.size();
				for (int i = 0; i < len; i++)
				{
					if (v1[i] == t0)
					{
						b = false;
						break;
					}
				}

				if (!b)
				{
					wstring t = _u2w(_lang.trans("This license is expired."));
					MessageBox(hDlg, t.c_str(), _u2w(title).c_str(), MB_OK | MB_ICONINFORMATION);
					return 1;
				}
			}

			if (b)
			{
				time_t current_time;
				time(&current_time);
				lic.date = current_time;
				lic.chksum = 0;
				lic.ov = 0;

				string sLic = "";
				soft.Grn(lic, sLic);
				soft.add_lic("wo3", sLic);

				if (v0 != "")
				{
					v0 += ",";
				}
				v0 += Int2Str(lic.num);
				soft.add_lic("thick", v0);


				_ov = 0;
				expiration_idle = Mat::zeros(Size(0, 0), CV_8UC3);

				wstring t = _u2w(_lang.trans("Activation completed!"));
				MessageBox(hDlg, t.c_str(), _u2w(title).c_str(), MB_OK | MB_ICONINFORMATION);

				return 0;
			}
		}

		wstring t = _u2w(_lang.trans("Please check your license key, and then try again."));
		MessageBox(hDlg, t.c_str(), _u2w(title).c_str(), MB_OK | MB_ICONINFORMATION);
	}
	return 1;
}

int CPaint::get_license(string& tit, string& exp)
{
	CMySoft soft;
	string code = "";
	lic_header lic2;
	int ov = 0;
	int ret = soft.get_verify(code, lic2, ov);
	if (ret == 0)
	{
		if (lic2.year >= 10)
		{
			tit = _lang.trans("Perpetual License");
			exp = "";
		}
		else
		{
			time_t t0 = lic2.date;

			if (lic2.year == 0)
			{
				t0 += 30 * EXP_SPAN;
				tit = _lang.trans("30 days trial period");
			}
			else
			{
				if (lic2.year == 1)
				{
					tit = _lang.trans("1-year license");
				}
				else
				{
					tit = _lang.trans("3-year license");
					ReplaceOne(tit, "3", Int2Str(lic2.year));
				}
			}

			struct tm t;
			localtime_s(&t, &t0);
			if (lic2.year != 0)
			{
				t.tm_year += lic2.year;
			}

			char stamped[MAX_PATH] = { 0 };
			strftime(stamped, MAX_PATH, "%Y/%m/%d", &t);

			exp = _lang.trans("Expiration Date:");
			exp += " ";
			exp += stamped;
		}
	}
	else
	{
		time_t current_time;
		time(&current_time);
		time_t t0 = current_time;

		t0 += 30 * EXP_SPAN;
		struct tm t;
		localtime_s(&t, &t0);

		char stamped[MAX_PATH] = { 0 };
		strftime(stamped, MAX_PATH, "%Y/%m/%d", &t);

		tit = _lang.trans("30 days trial period");

		exp += _lang.trans("Expiration Date:");
		exp += " ";
		exp += stamped;
	}
	return ret;
}
*/