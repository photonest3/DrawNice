///////////////////////////////////////////////////////////////////////
/// @file paint_text.cpp
/// @brief PhotoNest 绘图模块 - 文本输入功能实现文件
/// @details 实现文本输入和对话框功能:
///           - 加载输入对话框 (load_input_wnd)
///           - 文本叠加显示
///           - 输入框事件处理
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的核心交互文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
#include <Commdlg.h>
#include<fstream>
#include <process.h>
extern HINSTANCE _his;

bool CPaint::load_input_wnd(POINT pt, int w, int h)
{
	bool b = false;
	if (!IsWindow(_hInputWnd))
	{
		_hInputWnd = CreateDialog(_his, MAKEINTRESOURCE(IDD_DIALOG_INPUT), _win_handle, (DLGPROC)input_wnd_proc);
		::SetWindowLongPtr(_hInputWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		if (_input_bg_trans != 0)
		{
			SetWindowLong(_hInputWnd,
				GWL_EXSTYLE,
				GetWindowLong(_hInputWnd, GWL_EXSTYLE) | WS_EX_LAYERED);// | ~WS_EX_APPWINDOW); //| LWA_COLORKEY LWA_ALPHA
			int r = 0;
			int g = 0;
			int b = 0;
			get_rgb(r, g, b);

			SetLayeredWindowAttributes(_hInputWnd, RGB(r, g, b), 0, LWA_COLORKEY);
		}

		HDC desktopDc = ::CreateCompatibleDC(0);

		HFONT hfont = CreateFontIndirect(&_input_lf);
		::SelectObject(desktopDc, hfont);
		::GetTextMetrics(desktopDc, &_cw);

		//float hDeskDpi = GetDeviceCaps(desktopDc, DESKTOPVERTRES);
		//float hDpi = GetDeviceCaps(desktopDc, VERTRES);
		//float lg = GetDeviceCaps(desktopDc, LOGPIXELSY);
		//float ddpi = (float)hDeskDpi / (float)hDpi;
		//if (ddpi < 1)
		//{
		//	ddpi = 1;
		//}

		int pixels = _cw.tmHeight * _dst_zoom_w / _lena_w + 20;;

		DeleteDC(desktopDc);

		int xoff = 4;
		int yoff = 4;

		if (h == 0 && _dst_zoom_w > _lena_w)
		{
			w = w * _dst_zoom_w / _lena_w;
		}

		int hh = max(pixels, h);
		::MoveWindow(_hInputWnd, pt.x + xoff, pt.y + yoff, w, hh, FALSE);

		RECT rw;
		RECT rc;
		::GetWindowRect(_win_handle, &rw);
		::GetWindowRect(_hInputWnd, &rc);

		int x = rc.left - rw.left;
		int y = rc.top - rw.top;
		_input_wnd_offset = Point(x, y);

		int x0 = rc.left - rw.left - _dst_in_frame.x;
		int y0 = rc.top - rw.top - _dst_in_frame.y;
		int w0 = min(w, _dst.cols - x0);
		int h0 = min(hh, _dst.rows - y0);

		if (x0 + w > _dst.cols)
		{
			x0 = _dst.cols - w;
			w0 = w;
		}

		if (y0 + hh > _dst.rows)
		{
			y0 = _dst.rows - hh;
			h0 = hh;
		}

		int height = _cw.tmHeight * _dst_zoom_w / _lena_w;
		_hBmpTxt = (HBITMAP)LoadImage(_his, MAKEINTRESOURCE(IDB_TEXT), IMAGE_BITMAP, 4, height, 0);
		HWND hEdit = ::GetDlgItem(_hInputWnd, IDC_EDIT1);

		SetWindowLongPtr(hEdit, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		edit_wndproc_old_ = SetWndProcPtr(hEdit, EditWndProc);

		if (w0 > _dst.cols || h0 > _dst.rows)
		{
			::DestroyWindow(_hInputWnd);
			_hInputWnd = NULL;
			_input_win_in_dst = Rect(0, 0, 0, 0);
			_input_win_in_dst_last = Rect(0, 0, 0, 0);
			DeleteObject(hfont);
		}
		else
		{
			_input_win_in_dst_last = Rect(0, 0, 0, 0);
			_input_win_in_dst = Rect(x0, y0, w0, h0);
			::MoveWindow(_hInputWnd, pt.x + xoff, pt.y + yoff, w0, h0, FALSE);

			b = true;
		}
	}
	return b;
}

void CPaint::reload_input_wnd()
{
	memset(_input_buf, 0, 2048 * sizeof(wchar_t));
	GetDlgItemText(_hInputWnd, IDC_EDIT1, _input_buf, 2048);

	RECT rc;
	::GetWindowRect(_hInputWnd, &rc);
	::DestroyWindow(_hInputWnd);
	_hInputWnd = NULL;
	_input_win_in_dst_last = Rect(0, 0, 0, 0);

	POINT pt;
	pt.x = rc.left - 8;
	pt.y = rc.top - 8;

	load_input_wnd(pt, _input_win_in_dst.width, _input_win_in_dst.height);
}

WNDPROC CPaint::SetWndProcPtr(HWND hWnd, WNDPROC wndProc)
{
	WNDPROC old = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
	LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProc));
	return old;
}

void CPaint::move_input_wnd()
{
	if (::IsWindowVisible(_hInputWnd))
	{
		RECT rcWin;
		::GetWindowRect(_win_handle, &rcWin);

		if (_win_handle_rw.left != rcWin.left || _win_handle_rw.top != rcWin.top ||
			_win_handle_rw.right != rcWin.right || _win_handle_rw.bottom != rcWin.bottom)
		{
			_win_handle_rw.left = rcWin.left;
			_win_handle_rw.top = rcWin.top;
			_win_handle_rw.right = rcWin.right;
			_win_handle_rw.bottom = rcWin.bottom;

			int left = _input_wnd_offset.x + rcWin.left;
			int top = _input_wnd_offset.y + rcWin.top;

			RECT rc;
			::GetWindowRect(_hInputWnd, &rc);

			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;

			::MoveWindow(_hInputWnd, left, top, w, h, TRUE);
		}
	}
}

void CPaint::set_input_font()
{
	CHOOSEFONT cf;
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = _win_handle;
	cf.lpLogFont = &_input_lf;
	cf.rgbColors = _draw_rgb;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;

	if (ChooseFont(&cf) == TRUE)
	{
		_draw_rgb = cf.rgbColors;

		_input_lf.lfHeight = cf.lpLogFont->lfHeight;
		_input_lf.lfWidth = cf.lpLogFont->lfWidth;
		_input_lf.lfEscapement = cf.lpLogFont->lfEscapement;
		_input_lf.lfOrientation = cf.lpLogFont->lfOrientation;
		_input_lf.lfWeight = cf.lpLogFont->lfWeight;
		_input_lf.lfItalic = cf.lpLogFont->lfItalic;
		_input_lf.lfUnderline = cf.lpLogFont->lfUnderline;
		_input_lf.lfStrikeOut = cf.lpLogFont->lfStrikeOut;
		_input_lf.lfCharSet = cf.lpLogFont->lfCharSet;
		_input_lf.lfOutPrecision = cf.lpLogFont->lfOutPrecision;
		_input_lf.lfClipPrecision = cf.lpLogFont->lfClipPrecision;
		_input_lf.lfQuality = cf.lpLogFont->lfQuality;
		_input_lf.lfPitchAndFamily = cf.lpLogFont->lfPitchAndFamily;
		wcscpy_s(_input_lf.lfFaceName, cf.lpLogFont->lfFaceName);

		WritePrivate("lfHeight", _input_lf.lfHeight);
		WritePrivate("lfFaceName", _w2u(_input_lf.lfFaceName).c_str());

		WritePrivate("draw_rgb", _draw_rgb);

		int r = GetRValue(_draw_rgb);
		int g = GetGValue(_draw_rgb);
		int b = GetBValue(_draw_rgb);

		_picker_val = Scalar(b, g, r);



		if (_hInputWnd != NULL)
		{
			reload_input_wnd();
		}
	}
	else
	{
		if (_hInputWnd != NULL)
		{
			SetForegroundWindow(_hInputWnd);
			SetFocus(GetDlgItem(_hInputWnd, IDC_EDIT1));
		}
	}

	_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
}

void CPaint::set_draw_color()
{
	CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = _win_handle;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = _draw_rgb;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		_draw_rgb = cc.rgbResult;
		WritePrivate("draw_rgb", _draw_rgb);

		int r = GetRValue(_draw_rgb);
		int g = GetGValue(_draw_rgb);
		int b = GetBValue(_draw_rgb);

		_picker_val = Scalar(b, g, r);
	}

	_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
}

void CPaint::get_rgb(int& r, int& g, int& b)
{
	if (_input_bg_trans != 0)
	{
		r = GetRValue(_draw_rgb);
		g = GetGValue(_draw_rgb);
		b = GetBValue(_draw_rgb);

		r = r > 127 ? r - 1 : r + 1;
		g = g > 127 ? g - 1 : g + 1;
		b = b > 127 ? b - 1 : b + 1;
	}
	else
	{
		r = GetRValue(_input_bg_rgb);
		g = GetGValue(_input_bg_rgb);
		b = GetBValue(_input_bg_rgb);
	}

}

void CPaint::bg_ok(int trans)
{
	_input_bg_trans = trans;
	WritePrivate("input_bg_trans", trans);

	_input_bg_rgb = _select_bg_rgb;
	WritePrivate("input_bg_rgb", _input_bg_rgb);

	if (_hInputWnd != NULL)
	{
		reload_input_wnd();
	}
}

void CPaint::on_init_bg_wnd(HWND hDlg)
{
	_select_bg_rgb = _input_bg_rgb;

	if (_input_bg_trans != 0)
	{
		SendDlgItemMessage(hDlg, IDC_RADIO_TRANS, BM_SETCHECK, BST_CHECKED, 0);
		EnableWindow(GetDlgItem(hDlg, IDC_BTN_COLOR), FALSE);
	}
	else
	{
		SendDlgItemMessage(hDlg, IDC_RADIO_OPAQUE, BM_SETCHECK, BST_CHECKED, 1);
		EnableWindow(GetDlgItem(hDlg, IDC_BTN_COLOR), TRUE);
	}

	transl2(hDlg, IDC_BTN_COLOR, 1);
	transl2(hDlg, IDC_RADIO_OPAQUE);
	transl2(hDlg, IDC_RADIO_TRANS);
	transl2(hDlg, IDOK);
	transl2(hDlg, IDCANCEL);
	::SetWindowText(hDlg, _u2w(_lang.trans("Background")).c_str());

	RECT rc;
	rc.left = 150;
	rc.top = 16;
	rc.right = 166;
	rc.bottom = 32;
	::InvalidateRect(hDlg, &rc, TRUE);
}

void CPaint::input_command(int type)
{
	if (type == 0 && _for_font)
	{
		return;
	}

	if (_hInputWnd != NULL)
	{
		wchar_t buf[2048];
		memset(buf, 0, 2048 * sizeof(wchar_t));
		GetDlgItemText(_hInputWnd, IDC_EDIT1, buf, 2048);

		HWND hEdit = GetDlgItem(_hInputWnd, IDC_EDIT1);
		LRESULT off = SendMessage(hEdit, EM_POSFROMCHAR, 0, 0);//EM_GETMARGINS
		int left = LOWORD(off);
		//WritePrivate("EM_POSFROMCHAR", LOWORD(off));

		if (wcslen(buf) > 0)
		{
			int xoff = 4;
			int yoff = 4;

			Rect sr = _input_win_in_dst;

			Rect rc;
			rc.x = _dst_in_lena.x + sr.x * _lena_w / _dst_zoom_w + xoff * _lena_w / _dst_zoom_w;
			rc.y = _dst_in_lena.y + sr.y * _lena_w / _dst_zoom_w + yoff * _lena_w / _dst_zoom_w;

			rc.width = (sr.width - 8) * _lena_w / _dst_zoom_w;
			rc.height = (sr.height - 8) * _lena_w / _dst_zoom_w;

			rc.width = min(rc.width, _weighted_befoe_input.cols - rc.x);
			rc.height = min(rc.height, _weighted_befoe_input.rows - rc.y);
			if (rc.width >= 8 && rc.height >= 8)
			{

				//import
				div_t di = div(rc.width, 4);
				rc.width = di.quot * 4;

				Mat box = _weighted_befoe_input(rc);
				Mat text = put_input_text(rc, left, buf);
				set_trans_era(text, rc, box);
				text.copyTo(_weightedImage(rc));

				opt_t op;
				op.opt_type = btn_draw_text;
				op.nav_type = 0;
				op.thickness = THICKNESS_VAL;
				op.image = _weightedImage.clone();
				op.mask = _eraser_mask.clone();
				add_opt(op);
				_lena_bak = _weightedImage.clone();

				_working = act_display;
			}
		}
		_input_win_in_dst = Rect(0, 0, 0, 0);
		_input_win_in_dst_last = Rect(0, 0, 0, 0);

		_beginthreadex(NULL, 0, handle_input, this, NULL, NULL);
	}
}

void CPaint::destory_input()
{
	Sleep(100);
	::ShowWindow(_hInputWnd, SW_HIDE);
	::DestroyWindow(_hInputWnd);
	_hInputWnd = NULL;
}

LRESULT CALLBACK CPaint::input_wnd_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		/*
		case WM_CTLCOLORDLG:
		{
			HDC hDc = (HDC)wParam;
			CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));

			::SetBkMode(hDc, OPAQUE);
			int r = 0;
			int g = 0;
			int b = 0;
			self->get_rgb(r, g, b);

			COLORREF color = RGB(r, g, b);
			::SetBkColor(hDc, color);
			return (INT_PTR)CreateSolidBrush(color);
		}
		*/
	case WM_CTLCOLOREDIT:
	{
		HWND hEdit = ::GetDlgItem(hDlg, IDC_EDIT1);
		HDC hDc = (HDC)wParam;

		if (hEdit == (HWND)lParam)
		{
			CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
			::SetTextColor(hDc, self->_draw_rgb);

			::SetBkMode(hDc, OPAQUE);
			int r = 0;
			int g = 0;
			int b = 0;

			self->get_rgb(r, g, b);

			COLORREF color = RGB(r, g, b);
			::SetBkColor(hDc, color);

			return (INT_PTR)CreateSolidBrush(color);
		}
		ReleaseDC(hEdit, hDc);
		break;
	}
	/*
case WM_LBUTTONDOWN:
{
	CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
	::GetWindowRect(hDlg, &rc);
	//PostMessage(self->_win_handle, message, rc.left+ (int)(short)LOWORD(lParam), rc.top + (int)(short)HIWORD(lParam));
	PostMessage(self->_win_handle, message, wParam, MAKELPARAM(rc.left + (int)(short)LOWORD(lParam), rc.top + (int)(short)HIWORD(lParam)));

}
	break;
case WM_MOUSEMOVE:
{
	CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
	::GetWindowRect(hDlg, &rc);
	PostMessage(self->_win_handle, message, wParam, MAKELPARAM(rc.left + (int)(short)LOWORD(lParam), rc.top + (int)(short)HIWORD(lParam)));
//	PostMessage(self->_win_handle, message, wParam, lParam);

}
	break;

case WM_SIZING:
{
	LPRECT wp = (LPRECT)lParam;
	CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));

	RECT rw;
	GetWindowRect(self->_win_handle, &rw);

	int x0 = self->_dst_in_frame.x + rw.left;// +GetSystemMetrics(SM_CXFRAME);
	int y0 = self->_dst_in_frame.y + rw.top;// +GetSystemMetrics(SM_CYCAPTION);
	int w = self->_dst.cols - 10;
	int h = self->_dst.rows - 10;

	DWORD fw = (DWORD)wParam;
	switch (fw)
	{
	case WMSZ_TOP:
		wp->top = (wp->top < y0) ? y0 : wp->top;
		break;
	case WMSZ_LEFT:
		wp->left = (wp->left < x0) ? x0 : wp->left;
		break;
	case WMSZ_RIGHT:
		wp->right = (wp->right > x0 + w) ? x0 + w : wp->right;
		break;
	case WMSZ_BOTTOM:
		wp->bottom = (wp->bottom > y0 + h) ? y0 + h - wp->bottom : wp->bottom;
		break;
	case WMSZ_TOPLEFT:
		wp->top = (wp->top < y0) ? y0 : wp->top;
		wp->left = (wp->left < x0) ? x0 : wp->left;
		break;
	case WMSZ_TOPRIGHT:
		wp->top = (wp->top < y0) ? y0 : wp->top;
		wp->right = (wp->right > x0 + w) ? x0 + w : wp->right;
		break;
	case WMSZ_BOTTOMRIGHT:
		wp->right = (wp->right > x0 + w) ? x0 + w : wp->right;
		wp->bottom = (wp->bottom > y0 + h) ? y0 + h - wp->bottom : wp->bottom;
		break;
	case WMSZ_BOTTOMLEFT:
		wp->left = (wp->left < x0) ? x0 : wp->left;
		wp->bottom = (wp->bottom > y0 + h) ? y0 + h - wp->bottom : wp->bottom;
		break;
	default:
		break;
	}
}
break;
*/
	}
	return  (INT_PTR)FALSE;
}

LRESULT CALLBACK CPaint::bg_wnd_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		CPaint* self = (CPaint*)lParam;
		SetWindowLongPtr(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
		self->on_init_bg_wnd(hDlg);
		HWND hwnd1 = GetDlgItem(hDlg, IDC_STATIC_BG);
		ShowWindow(hwnd1, SW_HIDE);
	}
	return  (INT_PTR)TRUE;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hDlg, &ps);

		CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));

		RECT rc;
		self->get_trans_pos(hDlg, rc);

		HBRUSH hbrush = CreateSolidBrush(self->_select_bg_rgb);
		FillRect(hdc, &rc, hbrush);

		if (hbrush != NULL)
		{
			DeleteObject(hbrush);
		}

		EndPaint(hDlg, &ps);
		return (INT_PTR)TRUE;
	}
	case WM_COMMAND:
		CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hDlg, GWLP_USERDATA));
		if (LOWORD(wParam) == IDC_BTN_COLOR)
		{
			CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = self->_win_handle;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = self->_select_bg_rgb;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE)
			{
				self->_select_bg_rgb = cc.rgbResult;

				self->_input_bg_rgb = self->_select_bg_rgb;
				self->WritePrivate("input_bg_rgb", self->_input_bg_rgb);

				RECT rc;
				self->get_trans_pos(hDlg, rc);

				::InvalidateRect(hDlg, &rc, TRUE);
			}

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_RADIO_OPAQUE || LOWORD(wParam) == IDC_RADIO_TRANS)
		{
			int trans = (int)SendDlgItemMessage(hDlg, IDC_RADIO_TRANS, BM_GETCHECK, 0, 0);
			if (trans != 0)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_BTN_COLOR), FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg, IDC_BTN_COLOR), TRUE);
			}
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDOK)
		{
			int trans = (int)SendDlgItemMessage(hDlg, IDC_RADIO_TRANS, BM_GETCHECK, 0, 0);

			EndDialog(hDlg, LOWORD(wParam));
			self->bg_ok(trans);

			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));

			if (self->_hInputWnd != NULL)
			{
				SetForegroundWindow(self->_hInputWnd);
				SetFocus(GetDlgItem(self->_hInputWnd, IDC_EDIT1));
			}
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK CPaint::EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_SETFOCUS:
	{
		::CreateCaret(hWnd, self->_hBmpTxt, 0, 0);
		::ShowCaret(hWnd);
	}
	break;
	case WM_KILLFOCUS:
		::DestroyCaret();
		break;
		/*
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			wchar_t buf[2048];
			memset(buf, 0, 2048 * sizeof(wchar_t));
			GetWindowText(hWnd, buf, 2048);

			HDC hdc = ::GetDC(hWnd);
			HDC srcDC = CreateCompatibleDC(hdc);
			HFONT hfont = CreateFontIndirect(&self->_input_lf);
			HFONT hOldFont = (HFONT)SelectObject(srcDC, hfont);
			RECT rt;
			rt.left = 0;
			rt.right = 100;
			rt.top = 0;
			rt.bottom = 20;
			DrawText(srcDC, buf, wcslen(buf), &rt, DT_WORDBREAK | DT_CALCRECT | DT_LEFT | DT_TOP);


			SelectObject(srcDC, hOldFont);
			DeleteObject(hfont);

			::ReleaseDC(hWnd, hdc);

			::MoveWindow(hWnd, 0, 0, rt.right - rt.left, rt.bottom - rt.top, TRUE);

		}
	break;
	*/
	default:
		break;
	}

	return CallWindowProc(self->edit_wndproc_old_, hWnd, message, wParam, lParam);
}

Mat CPaint::put_input_text(Rect rc0, int left, const wchar_t* str)
{
	HDC hDC = CreateCompatibleDC(0);

	HFONT hf;
	//if(_dst_zoom_w > _lena_w)
	//{
	//	HWND hEdit = GetDlgItem(_hInputWnd, IDC_EDIT1);
	//	hf = (HFONT)::SendMessage(hEdit, WM_GETFONT, 0, 0);
	//}
	//else
	{
		LOGFONT lf = _input_lf;
		//lf.lfHeight = lf.lfHeight;
		//lf.lfWidth = lf.lfWidth;
		hf = CreateFontIndirect(&lf);
	}
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int r = 0;
	int g = 0;
	int b = 0;
	get_rgb(r, g, b);

	Mat dst(rc0.size(), CV_8UC3, Scalar(r, g, b));

	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = dst.cols;
	bih.biHeight = dst.rows;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = bih.biWidth * bih.biHeight;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	BITMAPINFO bmp = { 0 };
	bmp.bmiHeader = bih;

	void* pDibData = 0;
	HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);
	if (hBmp != 0)
	{
		//CV_Assert(pDibData != 0);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = bih.biWidth;
		rc.bottom = bih.biHeight;
		HBRUSH hbrush = CreateSolidBrush(RGB(r, g, b));
		FillRect(hDC, &rc, hbrush);

		if (hbrush != NULL)
		{
			DeleteObject(hbrush);
		}

		SetBkColor(hDC, RGB(r, g, b));
		SetTextColor(hDC, _draw_rgb);

		RECT rt;
		rt.left = left * _lena_w / _dst_zoom_w;

		rt.top = 0;
		rt.right = rc0.width;
		rt.bottom = rc0.height;
		//TextOut(hDC, 0, 0, str, wcslen(str));
		DrawText(hDC, str, (int)wcslen(str), &rt, DT_WORDBREAK);

		GetBitmapBits(hBmp, bih.biWidth * bih.biHeight * 3, dst.data);

		SelectObject(hDC, hOldBmp);
		DeleteObject(hBmp);
	}

	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteDC(hDC);

	return dst;
}

void CPaint::set_trans_era(Mat dst, Rect rc0, Mat box)
{
	if (_input_bg_trans != 0)
	{
		int r = 0;
		int g = 0;
		int b = 0;
		get_rgb(r, g, b);

		if (_eraser_mask.empty())
		{
			for (int i = 0; i < dst.rows; i++)
			{
				Vec3b* ptr = dst.ptr<Vec3b>(i);
				for (int j = 0; j < dst.cols; j++)
				{
					Vec3b& v = ptr[j];
					if (v[0] == b && v[1] == g && v[2] == r)
					{
						v = box.at<Vec3b>(i, j);
					}
				}
			}
		}
		else
		{
			Mat box2 = _eraser_mask(rc0);
			for (int i = 0; i < dst.rows; i++)
			{
				Vec3b* ptr = dst.ptr<Vec3b>(i);
				for (int j = 0; j < dst.cols; j++)
				{
					Vec3b& v = ptr[j];
					if (v[0] == b && v[1] == g && v[2] == r)
					{
						v = box.at<Vec3b>(i, j);
					}
					else
					{
						box2.at<uchar>(i, j) = 255;
					}
				}
			}
		}
	}
	else
	{
		if (!_eraser_mask.empty())
		{
			_eraser_mask(rc0).setTo(255);
		}
	}
}

void CPaint::get_trans_pos(HWND hDlg, RECT& rc)
{
	int cx = GetSystemMetrics(SM_CXICON);

	HWND hwnd1 = GetDlgItem(hDlg, IDC_STATIC_BG);
	GetWindowRect(hwnd1, &rc);

	POINT pt;
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(hDlg, &pt);

	rc.left = pt.x;
	rc.top = pt.y;
	rc.right = rc.left + 20 * cx / 32;
	rc.bottom = rc.top + 20 * cx / 32;

}