///////////////////////////////////////////////////////////////////////
/// @file paint_other.cpp
/// @brief 绘图模块 - 其他功能实现
/// @details 实现图像处理的其他功能,包括 BMP 格式处理、像素操作、
///          位图压缩、设备上下文(DC)管理等底层图像操作
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件包含 Windows GDI 位图操作和图像格式处理
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//Ö¸¶¨ÎÄ¼þ´óÐ¡&#xff0c;°üÀ¨Õâ14¸ö×Ö½Ú
/*32Î»µÄWindows²Ù×÷ÏµÍ³´¦Àí4¸ö×Ö½Ú(32Î»)µÄËÙ¶È±È½Ï¿ì&#xff0c;ËùÒÔBMPµÄÃ¿Ò»ÐÐÑÕÉ«Õ¼ÓÃµÄ×Ö½ÚÊý¹æ¶¨Îª4µÄÕûÊý±¶¡£MyBmp.bmpÖÐÒ»ÐÐÑÕÉ«ÓÐÁ½¸öÏñËØ&#xff0c;¹²Õ¼ÓÃ6×Ö½Ú&#xff0c;Èç¹ûÒª²¹Æë4*2=8×Ö½Ú&#xff0c;¾ÍÒªÔÙ¼ÓÁ½¸ö0×Ö½Ú¡£
ÐÐ²¹Î»µÄ¹«Ê½Îª&#xff1a;widthBytes = (width*biBitCount&#43;31)/32*4 */
//width = (width*bpp &#43; width) / 32 * 4;
// ¸ß¶È  .bMPÎÄ¼þµÄÊý¾Ý´ÓÏÂµ½ÉÏ&#xff0c;´Ó×óµ½ÓÒµÄ¡£Ò²¾ÍÊÇËµ&#xff0c;´ÓÎÄ¼þÖÐ×îÏÈ¶Áµ½µÄÊÇÍ¼Ïó×îÏÂÃæÒ»ÐÐµÄ×ó±ßµÚÒ»¸öÏóËØ&#xff0c;È»ºóÊÇ×ó±ßµÚ¶þ¸öÏóËØ¡­¡­½ÓÏÂÀ´ÊÇµ¹ÊýµÚ¶þÐÐ×ó±ßµÚÒ»¸öÏóËØ&#xff0c;×ó±ßµÚ¶þ¸öÏóËØ¡­¡­ÒÀ´ÎÀàÍÆ &#xff0c;×îºóµÃµ½µÄÊÇ×îÉÏÃæÒ»ÐÐµÄ×îÓÒÒ»¸öÏóËØ¡£
// Ã¿¸öÏñËØËùÐèµÄÎ»Êý&#xff0c;±ØÐëÊÇ1(Ë«É«), Ã¿¸öÏñËØµÄÎ»Êý
// 1 - ºÚ°×Í¼&#xff0c;4 - 16É«&#xff0c;8 - 256É«&#xff0c;24 - Õæ²ÊÉ« 4(16É«)&#xff0c;8(256É«)»ò24(Õæ²ÊÉ«)Ö®Ò»
/*
DWORD biSizeImage; // Î»Í¼µÄ´óÐ¡&#xff0c;ÒÔ×Ö½ÚÎªµ¥Î»
LONG biXPelsPerMeter; // Î»Í¼Ë®Æ½·Ö±æÂÊ&#xff0c;Ã¿Ã×ÏñËØÊý
LONG biYPelsPerMeter; // Î»Í¼´¹Ö±·Ö±æÂÊ&#xff0c;Ã¿Ã×ÏñËØÊý
DWORD biClrUsed;// Î»Í¼Êµ¼ÊÊ¹ÓÃµÄÑÕÉ«±íÖÐµÄÑÕÉ«Êý
DWORD biClrImportant;// Î»Í¼ÏÔÊ¾¹ý³ÌÖÐÖØÒªµÄÑÕÉ«Êý72*8
*/


//HDC hDC = ::GetDC(hWnd); // »ñÈ¡Éè±¸»·¾³¾ä±ú
//HDC hdcMem = CreateCompatibleDC(hDC); // ´´½¨ÓëÉè±¸Ïà¹ØµÄÄÚ´æ»·¾³
//SelectObject(hdcMem, hBitmap); // Ñ¡Ôñ¶ÔÏó
//SetMapMode(hdcMem, GetMapMode(hDC)); // ÉèÖÃÓ³ÉäÄ£Ê½
//BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY); //Î»Í¼¸´ÖÆ
//::ReleaseDC(hWnd, hDC); // ÊÍ·ÅÉè±¸»·¾³¾ä±ú
//DeleteDC(hdcMem); // É¾³ýÄÚ´æ»·¾³

// Ä¿±êÉè±¸µÄ¼¶±ð&#xff0c;±ØÐëÎª1
// Î»Í¼Ñ¹ËõÀàÐÍ&#xff0c;±ØÐëÊÇ 0(²»Ñ¹Ëõ), 1(BI_RLE8Ñ¹ËõÀàÐÍ)»ò2(BI_RLE4Ñ¹ËõÀàÐÍ)Ö®Ò»
//	bmih->biSizeImage = bmih->biHeight * 8 *height;
/*biSizeImage
Ö¸¶¨Êµ¼ÊµÄÎ»Í¼Êý¾ÝÕ¼ÓÃµÄ×Ö½ÚÊý&#xff0c;ÆäÊµÒ²¿ÉÒÔ´ÓÒÔÏÂµÄ¹«Ê½ÖÐ¼ÆËã³öÀ´&#xff1a;
biSizeImage=biWidth¡¯ ¡Á biHeight
Òª×¢ÒâµÄÊÇ&#xff1a;ÉÏÊö¹«Ê½ÖÐµÄbiWidth¡¯±ØÐëÊÇ4µÄÕû±¶Êý(ËùÒÔ²»ÊÇbiWidth&#xff0c;¶øÊÇbiWidth¡¯&#xff0c;
±íÊ¾´óÓÚ»òµÈÓÚbiWidthµÄ&#xff0c;×î½Ó½ü4µÄÕû±¶Êý¡£¾Ù¸öÀý×Ó&#xff0c;Èç¹ûbiWidth=240&#xff0c;ÔòbiWidth¡¯=240&#xff1b;
Èç¹ûbiWidth=241&#xff0c;biWidth¡¯=244)¡£*/

//static CPaint * pPaint;
//
//struct  myPoint {
//	int x;
//	int y;
//
//	bool operator<(const myPoint& tmp) const {
//		if (this->x < tmp.x || (this->x == tmp.x && this->y < tmp.y))
//			return true;
//		return false;
//	}
//};
//set<myPoint> _langMap;



//
//
//
//void CPaint::set_filter_bg(Mat temp)
//{
//	float alphaReserve = 0.1;
//	int	BChannel = 49;
//	int	GChannel = 52;
//	int	RChannel = 49;
//
//	int w = temp.cols;
//	int h = temp.rows;
//
//	for (int i = 0; i < h; i++)
//	{
//		for (int j = 0; j < w; j++)
//		{
//			Vec3b& v = temp.at<Vec3b>(i, j);
//
//			v[0] = v[0] * alphaReserve + BChannel * (1 - alphaReserve);
//			v[1] = v[1] * alphaReserve + GChannel * (1 - alphaReserve);
//			v[2] = v[2] * alphaReserve + RChannel * (1 - alphaReserve);
//		}
//	}
//}

//case btn_seg_fgd:
//{
//	txt = _lang.trans(L"Foreground");
//	draw_tip_txt(TOOLBAR_BTN_LEFT + 405 + off + 12, TOOLBAR_TOP_HEIGHT, txt.c_str());
//}
//break;
//case btn_seg_bgd:
//	txt = _lang.trans(L"Background");
//	draw_tip_txt(TOOLBAR_BTN_LEFT + 445 + off + 12, TOOLBAR_TOP_HEIGHT, txt.c_str());
//	break;
//case nav_inp:
//	draw_tip_txt(lf + off, TOOLBAR_TOP_HEIGHT, _lang.trans(L"Remove Unwanted Objects").c_str());
//	break;
//case menu_edit:
//	lf += TOOLBAR_BTN_STEP_X;
//	draw_tip_txt(lf + off, TOOLBAR_TOP_HEIGHT, _lang.trans(L"General").c_str());
//	break;
//case btn_seg_rect:
//if (_show_combo == 1 && _rect_segment.width == 0)
//{
//top += TOOLBAR_BTN_STEP_Y * 2;
//draw_tip_txt(70 + _combo_width_seg_2, top/* + off*/, seg_sample);
//int x = 50;
//int w = min(_win_w - x, seg_sample.cols);
//seg_sample(Rect(0, 0, w, seg_sample.rows)).copyTo(
//	_frame(Rect(x, TOOLBAR_TOP_HEIGHT, w, seg_sample.rows)));
//}
//break;

//case btn_delete:
//	draw_tip_txt(50, TOOLBAR_TOP_HEIGHT + 284 + off, _lang.trans(L"Transparent").c_str());
//	break;
//case toolbar_open:
//{
//	wchar_t *p = wcsrchr((wchar_t*)_cur_name.c_str(), L'\\');
//	if (p != NULL)
//	{
//		wstring tmp = p + 1;
//		tmp += L" (" + _file_time + L")";
//		Mat lbl = get_lbl(tmp.c_str(), Scalar::all(128));
//		lbl.copyTo(_frame(Rect(_win_w - lbl.cols - 10, TOOLBAR_TOP_HEIGHT + off, lbl.cols, lbl.rows)));
//	}
//}
//break;

/*
void CPaint::cut_clipboard()
{
	Rect rc;
	bool b = copy_clipboard(rc);
	if (!b)
	{
		return;
	}

	opt_t op;
	op.opt_type = btn_cut;
	op.thickness = _channels;

	Mat dst = _weightedImage(rc);
	int w = dst.cols;
	int h = dst.rows;

	//if (_channels == 4)
	{
		op.pts.push_back(Point(rc.x, rc.y));
		op.pts.push_back(Point(rc.x + rc.width, rc.y + rc.height));

		//_rect_cut = rc;
		for (int i = 0; i < h; i++)
		{
			for (int j = 0; j < w; j++)
			{
				dst.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
		}
		op.inpaintedImage = _weightedImage.clone();
	}

	//save_to_file(dst, L"d:\\test\\u.png");
	add_opt(op);
}
*/
//if (lfHeight >= 96)
//{
//	lfHeight = 96;//72
//}
//else if (lfHeight >= 64)
//{
//	lfHeight = 64;//48
//}
//else if (lfHeight >= 48)
//{
//	lfHeight = 48;//36
//}
//else if (lfHeight >= 37)
//{
//	lfHeight = 37;//28
//}
//else if (lfHeight >= 35)
//{
//	lfHeight = 35;//26
//}
//else if (lfHeight >= 32)
//{
//	lfHeight = 32;//24
//}
//else if (lfHeight >= 29)
//{
//	lfHeight = 29;//22
//}
//else if (lfHeight >= 27)
//{
//	lfHeight = 27;//20
//}
//else if (lfHeight >= 24)
//{
//	lfHeight = 24;//18
//}
//else if (lfHeight >= 21)
//{
//	lfHeight = 21;//16
//}
//else if (lfHeight >= 19)
//{
//	lfHeight = 19;//14
//}
//else if (lfHeight >= 16)
//{
//	lfHeight = 16;//12
//}
//else if (lfHeight >= 15)
//{
//	lfHeight = 15;//11
//}
//else if (lfHeight >= 13)
//{
//	lfHeight = 13;//10
//}
//else if (lfHeight >= 12)
//{
//	lfHeight = 12;//9
//}
//else
//{
//	lfHeight = 11;//8
//}



/*


//HWND hWnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP, wc.lpszClassName, nullptr, WS_OVERLAPPEDWINDOW, 183, 84, 1000, 600, nullptr, nullptr,
//	hInstance, nullptr);



//int style = GetWindowLong(hwndDlg, GWL_STYLE);
//style &= ~WS_CAPTION;
//SetWindowLong(hwndDlg, GWL_STYLE, style);
//::SetWindowPos(hwndDlg, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE);

//color = RGB(255, 255, 255);
//SetLayeredWindowAttributes(hwndDlg, color, 0, LWA_COLORKEY);// | LWA_ALPHA);
////SendDlgItemMessage(hwndDlg, BTN_FONT, BM_SETIMAGE, IMAGE_BITMAP,
////	(LPARAM)LoadBitmap(GetModuleHandle(L"cv.dll"),  MAKEINTRESOURCE(IDB_FONT_IDLE)));

//case WM_PAINT:
//{
//	PAINTSTRUCT ps;
//	HDC hdc;
//	hdc = BeginPaint(hwndDlg, &ps);

//	EndPaint(hwndDlg, &ps);
//	return true;
//}
//case WM_COMMAND:
//	if (LOWORD(wParam) == BTN_FONT)
//	{
//		CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));
//		self->set_input_font();
//		return (INT_PTR)TRUE;
//	}
//	break;
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

RECT rc;
::GetWindowRect(hwndDlg, &rc);

self->_input_win_in_dst = Rect(rc.left - rw.left - self->_dst_in_frame.x,
rc.top - rw.top - self->_dst_in_frame.y,
rc.right - rc.left, rc.bottom - rc.top);

HWND hEdit = GetDlgItem(hwndDlg, IDC_EDIT1);
w = rc.right - rc.left;
h = rc.bottom - rc.top;
::MoveWindow(hEdit, 0, 0, w, h, TRUE);

RECT rcWin;
::GetWindowRect(self->_win_handle, &rcWin);

int x = rc.left - rw.left;
int y = rc.top - rw.top;
self->_input_wnd_offset = Point(x, y);

return TRUE;
}

//case WM_NCCALCSIZE:
//	if (wParam)
//	{
//		if (IsZoomed(hwndDlg))//×î´ó»¯Ê±ÐÞÕý¿Í»§Çø
//		{
//			NCCALCSIZE_PARAMS* lp = (LPNCCALCSIZE_PARAMS)lParam;
//			lp->rgrc[0].left += 8;
//			lp->rgrc[0].top += 8;
//			lp->rgrc[0].right -= 8;
//			lp->rgrc[0].bottom -= 8;
//		}
//	}
//	return 0;//È¥³ý·Ç¿Í»§Çø
//case WM_NCHITTEST:
//	if (!IsZoomed(hwndDlg))//µ÷Õû´°¿Ú´óÐ¡
//	{
//		int x = 2;//±ß¿ò¿í¶È
//		RECT rc; GetClientRect(hwndDlg, &rc);//¿Í»§Çø¾ØÐÎ
//		POINT pt; GetCursorPos(&pt); ScreenToClient(hwndDlg, &pt);//Êó±êÎ»ÖÃ
//		if (pt.x < rc.left + x)
//		{
//			if (pt.y < rc.top + x)return HTTOPLEFT;
//			if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;
//			return HTLEFT;
//		}
//		if (pt.x >= rc.right - x)
//		{
//			if (pt.y < rc.top + x)return HTTOPRIGHT;
//			if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;
//			return HTRIGHT;
//		}
//		if (pt.y < rc.top + x)return HTTOP;
//		if (pt.y >= rc.bottom - x)return HTBOTTOM;
//	}
//	return HTCAPTION;//ÍÏ¶¯´°¿Ú
//case WM_CTLCOLORDLG:
//{
//	HDC hDc = (HDC)wParam;
//	//::SetBkMode(hDc, OPAQUE);
//	//COLORREF color = TRANSPARENT;// RGB(255, 255, 255);//
//	CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));

//	::SetBkMode(hDc, OPAQUE);
//	int r = 0;
//	int g = 0;
//	int b = 0;
//	self->get_rgb(r, g, b);

//	COLORREF color = RGB(r, g, b);


//	::SetBkColor(hDc, color);
//	return (INT_PTR)CreateSolidBrush(color);
//}

*/

/*
LRESULT CALLBACK CPaint::EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
CPaint* self = reinterpret_cast<CPaint*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

switch (message) {
case WM_INITDIALOG:
{
SetCursor((HCURSOR)LoadCursor(0, IDC_ARROW));
}
return TRUE;
case WM_SETFOCUS:
::CreateCaret(hWnd, self->_hbmp, 0, 0);
::ShowCaret(hWnd);
break;
case WM_KILLFOCUS:
::DestroyCaret();
break;
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

//TEXTMETRICW cw;
//::GetTextMetrics(hdc, &cw);

//SIZE si;
//::GetTextExtentPoint(hdc, buf, wcslen(buf), &si);

SelectObject(srcDC, hOldFont);
DeleteObject(hfont);

::ReleaseDC(hWnd, hdc);

//::MoveWindow(hWnd, 0, 0, si.cx, si.cy, TRUE);
::MoveWindow(hWnd, 0, 0, rt.right - rt.left, rt.bottom - rt.top, TRUE);

}
break;
case WM_CHAR:
{
//HWND hEdit = GetDlgItem(_hInputWnd, IDC_EDIT1);

}
break;
}

return CallWindowProc(self->edit_wndproc_old_, hWnd, message, wParam, lParam);
}
*/

//if (_show_cpicker == 1)
//{
//	int top = TOOLBAR_TOP_HEIGHT;
//	int left = 169;

//	Mat tt = color_el.clone();
//	make_draw_ico(tt, 220);

//	char buf[100] = { 0 };
//	sprintf_s(buf, "#%02x%02x%02x", (unsigned int)_cur_color[2], (unsigned int)_cur_color[1], (unsigned int)_cur_color[0]);

//	draw_ico_button(buf, tt, Scalar::all(220), Scalar::all(76));

//	tt.copyTo(color_picker(Rect(28, 70, tt.cols, tt.rows)));

//	color_picker.copyTo(_frame(Rect(left, top, color_picker.cols, color_picker.rows)));
//}

//else if (_show_cpicker == 1)
//{
//	int top = TOOLBAR_TOP_HEIGHT + 21;

//	int lf = 169 + 21;
//	int off = 20;
//	int off1 = 20;

//	if (x > lf && x < lf + off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(0);
//	}
//	else if (x > lf + off && x < lf + 2 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(38);
//	}
//	else if (x > lf + 2 * off && x < lf + 3 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(76);
//	}
//	else if (x > lf + 3 * off && x < lf + 4 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(113);
//	}
//	else if (x > lf + 4 * off && x < lf + 5 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(151);
//	}
//	else if (x > lf + 5 * off && x < lf + 6 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(189);
//	}
//	else if (x > lf + 6 * off && x < lf + 7 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar::all(229);
//	}
//	else if (x > lf + 7 * off && x < lf + 8 * off && y > top && y < top + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(134, 134, 229);
//	}

//	if (x > lf && x < lf + off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(58, 58, 229);
//	}
//	else if (x > lf + off && x < lf + 2 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(22, 91, 229);
//	}
//	else if (x > lf + 2 * off && x < lf + 3 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(53, 168, 229);
//	}
//	else if (x > lf + 3 * off && x < lf + 4 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(142, 170, 3);
//	}
//	else if (x > lf + 4 * off && x < lf + 5 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(229, 152, 0);
//	}
//	else if (x > lf + 5 * off && x < lf + 6 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(207, 83, 73);
//	}
//	else if (x > lf + 6 * off && x < lf + 7 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(229, 85, 142);
//	}
//	else if (x > lf + 7 * off && x < lf + 8 * off && y > top + off1 && y < top + off1 + off)
//	{
//		_is_in_combo = true;
//		_cur_color = Scalar(118, 76, 229);
//	}

//	if (_is_in_combo && _opt != btn_draw_free && _opt != btn_draw_straight && _opt != btn_draw_rect && _opt != btn_draw_circle && _opt != btn_draw_arrow)
//	{
//		_opt = btn_draw_free;
//	}

//	_show_cpicker = 0;
//	_working = act_display;
//	//b = true;
//}
//else if (_show_cpicker == 1)
//{
//	int top3 = TOOLBAR_TOP_HEIGHT;
//	top3 = _win_h - color_picker.rows;

//	int lf3 = 169;
//	lf3 = TOOLBAR_BTN_LEFT + 5 + 120;

//	if ((x > lf3 && x < lf3 + color_picker.cols) && (y > top3 && y < top3 + color_picker.rows))
//	{
//		_cursor = id_arrow;
//	}
//}
//load_bitmap(color_picker, IDB_COLOR_PICKER);
//load_bitmap(color_el, IDB_COLOR_EL);

//_hbmp = LoadBitmap(_his, MAKEINTRESOURCE(IDB_TEXT));











//if (_app_type != FROM_EXE)
//{
//	if (hMapFile == NULL)
//	{
//		hMapFile = OpenFileMapping(
//			//INVALID_HANDLE_VALUE,
//			//NULL,
//			FILE_MAP_READ /*| SEC_RESERVE*/,
//			0,
//			//1024 * 64,
//			L"Paint_Mapping_Object"
//		);
//	}
//	if (hMapFile)
//	{
//		pBuf = (char *)MapViewOfFile(
//			hMapFile,
//			FILE_MAP_READ,
//			0,
//			0,
//			1024 * 64
//		);
//		cvui::printf(_frame, _win_w - 400, _win_h + 10, "%d ", strlen(pBuf));
//		if (strcmp(pBuf, "close") == 0)
//		{
//			UnmapViewOfFile(pBuf);
//			CloseHandle(hMapFile);
//			break;
//		}
//		else if (_lstFile.empty()/* && strlen(pBuf) > 8*/)
//		{
//			vector<string> v1 = tokenize(pBuf, "\r\n", true, "");
//			int len = v1.size();
//			for (int i = 0; i < len; i++)
//			{
//				_lstFile.push_back(_u2w(v1[i]));
//			}
//			_len_files = _lstFile.size();
//			_idx_file = 0;
//			list<wstring>::iterator it;
//			for (it = _lstFile.begin(); it != _lstFile.end(); it++)
//			{
//				praser_file(*it);
//				_idx_file++;
//				if (_ori_id == fname)
//				{
//					break;
//				}
//			}
//			//UnmapViewOfFile(pBuf);
//			//CloseHandle(hMapFile);
//		}
//	}
//}
//HANDLE hMapFile = NULL;
//char *pBuf = NULL;
//hMapFile = CreateFileMapping(
//	INVALID_HANDLE_VALUE,
//	NULL,
//	PAGE_READWRITE /*| SEC_RESERVE*/,
//	0,
//	1024 * 64,
//	L"Paint_Mapping_Object"
//);
//pBuf = (char *)MapViewOfFile(
//	hMapFile,
//	FILE_MAP_ALL_ACCESS,
//	0,
//	0,
//	1024 * 64
//);

/*			int top3 = TOOLBAR_TOP_HEIGHT;
top3 = _win_h - 168;
int lf3 = TOOLBAR_BTN_LEFT + 5;
if (_menu_btn == menu_rotate)
{
switch (cur_xy)
{
case btn_rota270:
txt = _lang.trans(L"Rotate left 90¡ã");
draw_tip_txt(lf3 + 40, top3 + off, txt.c_str());
break;
case btn_rota90:
txt = _lang.trans(L"Rotate right 90¡ã");
draw_tip_txt(lf3 + 40, top3 + 32 + off, txt.c_str());
break;
case btn_rota180:
txt = _lang.trans(L"Rotate 180¡ã");
draw_tip_txt(lf3 + 40, top3 + 64 + off, txt.c_str());
break;
case btn_fliph:
txt = _lang.trans(L"Flip horizontal");
draw_tip_txt(lf3 + 40, top3 + 96 + off, txt.c_str());
break;
case btn_flipv:
txt = _lang.trans(L"Flip vertical");
draw_tip_txt(lf3 + 40, top3 + 128 + off, txt.c_str());
break;
default:
break;
}
}
*/


/*
// ¼ÆËã²îÖµ¾ù·½¸ù
int CPaint::geiDiff(uchar b, uchar g, uchar r, uchar tb, uchar tg, uchar tr)
{
	return  int(sqrt(((b - tb)*(b - tb) + (g - tg)*(g - tg) + (r - tr)*(r - tr)) / 3));
}

// ±³¾°·ÖÀë
cv::Mat CPaint::BackgroundSeparation(cv::Mat src, Inputparama input)
{
	cv::Mat bgra, mask;
	// ×ª»¯ÎªBGRA¸ñÊ½£¬´øÍ¸Ã÷¶È£¬4Í¨µÀ
	cvtColor(src, bgra, COLOR_BGR2BGRA);
	mask = cv::Mat::zeros(bgra.size(), CV_8UC1);
	int row = src.rows;
	int col = src.cols;

	// Òì³£ÊýÖµÐÞÕý
	input.p.x = max(0, min(col, input.p.x));
	input.p.y = max(0, min(row, input.p.y));
	input.thresh = max(5, min(200, input.thresh));
	input.transparency = max(0, min(255, input.transparency));
	input.size = max(0, min(30, input.size));

	// È·¶¨±³¾°É«
	uchar ref_b = src.at<Vec3b>(input.p.y, input.p.x)[0];
	uchar ref_g = src.at<Vec3b>(input.p.y, input.p.x)[1];
	uchar ref_r = src.at<Vec3b>(input.p.y, input.p.x)[2];

	// ¼ÆËãÃÉ°æÇøÓò£¨ÑÚÄ¤£©
	for (int i = 0; i < row; ++i)
	{
		uchar *m = mask.ptr<uchar>(i);
		uchar *b = src.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			if ((geiDiff(b[3 * j], b[3 * j + 1], b[3 * j + 2], ref_b, ref_g, ref_r)) > input.thresh)
			{
				m[j] = 255;
			}
		}
	}

	cv::Mat tmask = cv::Mat::zeros(row + 50, col + 50, CV_8UC1);
	mask.copyTo(tmask(cv::Range(25, 25 + mask.rows), cv::Range(25, 25 + mask.cols)));

	// Ñ°ÕÒÂÖÀª£¬×÷ÓÃÊÇÌî³äÂÖÀªÄÚºÚ¶´
	vector<vector<Point>> contour;
	vector<Vec4i> hierarchy;
	// RETR_TREEÒÔÍø×´½á¹¹ÌáÈ¡ËùÓÐÂÖÀª£¬CHAIN_APPROX_NONE»ñÈ¡ÂÖÀªµÄÃ¿¸öÏñËØ
	findContours(tmask, contour, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	drawContours(tmask, contour, -1, Scalar(255), FILLED, 16);

	// ºÚÃ±ÔËËã»ñÈ¡Í¬±³¾°É«ÀàËÆµÄÇøÓò£¬Ê¶±ðºóÌî³ä
	cv::Mat hat;
	cv::Mat element = getStructuringElement(MORPH_ELLIPSE, Size(31, 31));
	cv::morphologyEx(tmask, hat, MORPH_BLACKHAT, element);
	hat.setTo(255, hat > 0);
	cv::Mat hatd;
	Clear_MicroConnected_Areas(hat, hatd, 450);
	tmask = tmask + hatd;
	mask = tmask(cv::Range(25, 25 + mask.rows), cv::Range(25, 25 + mask.cols)).clone();

	// ÑÚÄ¤ÂË²¨£¬ÊÇÎªÁË±ßÔµÐé»¯
	cv::blur(mask, mask, Size(2 * input.size + 1, 2 * input.size + 1));

	// ¸ÄÉ«
	for (int i = 0; i < row; ++i)
	{
		uchar *r = bgra.ptr<uchar>(i);
		uchar *m = mask.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			// ÃÉ°æÎª0µÄÇøÓò¾ÍÊÇ±ê×¼±³¾°Çø
			if (m[j] == 0)
			{
				r[4 * j] = uchar(input.color[0]);
				r[4 * j + 1] = uchar(input.color[1]);
				r[4 * j + 2] = uchar(input.color[2]);
				r[4 * j + 3] = uchar(input.transparency);
			}
			// ²»Îª0ÇÒ²»Îª255µÄÇøÓòÊÇÂÖÀªÇøÓò£¨±ßÔµÇø£©£¬ÐèÒªÐé»¯´¦Àí
			else if (m[j] != 255)
			{
				// ±ßÔµ´¦°´±ÈÀýÉÏÉ«
				int newb = (r[4 * j] * m[j] * 0.3 + input.color[0] * (255 - m[j])*0.7) / ((255 - m[j])*0.7 + m[j] * 0.3);
				int newg = (r[4 * j + 1] * m[j] * 0.3 + input.color[1] * (255 - m[j])*0.7) / ((255 - m[j])*0.7 + m[j] * 0.3);
				int newr = (r[4 * j + 2] * m[j] * 0.3 + input.color[2] * (255 - m[j])*0.7) / ((255 - m[j])*0.7 + m[j] * 0.3);
				int newt = (r[4 * j + 3] * m[j] * 0.3 + input.transparency * (255 - m[j])*0.7) / ((255 - m[j])*0.7 + m[j] * 0.3);
				newb = max(0, min(255, newb));
				newg = max(0, min(255, newg));
				newr = max(0, min(255, newr));
				newt = max(0, min(255, newt));
				r[4 * j] = newb;
				r[4 * j + 1] = newg;
				r[4 * j + 2] = newr;
				r[4 * j + 3] = newt;
			}
		}
	}
	return bgra;
}

void CPaint::Clear_MicroConnected_Areas(cv::Mat src, cv::Mat &dst, double min_area)
{
	// ±¸·Ý¸´ÖÆ
	dst = src.clone();
	std::vector<std::vector<cv::Point> > contours;  // ´´½¨ÂÖÀªÈÝÆ÷
	std::vector<cv::Vec4i>    hierarchy;

	// Ñ°ÕÒÂÖÀªµÄº¯Êý
	// µÚËÄ¸ö²ÎÊýCV_RETR_EXTERNAL£¬±íÊ¾Ñ°ÕÒ×îÍâÎ§ÂÖÀª
	// µÚÎå¸ö²ÎÊýCV_CHAIN_APPROX_NONE£¬±íÊ¾±£´æÎïÌå±ß½çÉÏËùÓÐÁ¬ÐøµÄÂÖÀªµãµ½contoursÏòÁ¿ÄÚ
	cv::findContours(src, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, cv::Point());

	if (!contours.empty() && !hierarchy.empty())
	{
		std::vector<std::vector<cv::Point> >::const_iterator itc = contours.begin();
		// ±éÀúËùÓÐÂÖÀª
		while (itc != contours.end())
		{
			// ¶¨Î»µ±Ç°ÂÖÀªËùÔÚÎ»ÖÃ
			cv::Rect rect = cv::boundingRect(cv::Mat(*itc));
			// contourAreaº¯Êý¼ÆËãÁ¬Í¨ÇøÃæ»ý
			double area = contourArea(*itc);
			// ÈôÃæ»ýÐ¡ÓÚÉèÖÃµÄãÐÖµ
			if (area < min_area)
			{
				// ±éÀúÂÖÀªËùÔÚÎ»ÖÃËùÓÐÏñËØµã
				for (int i = rect.y; i < rect.y + rect.height; i++)
				{
					uchar *output_data = dst.ptr<uchar>(i);
					for (int j = rect.x; j < rect.x + rect.width; j++)
					{
						// ½«Á¬Í¨ÇøµÄÖµÖÃ0
						if (output_data[j] == 255)
						{
							output_data[j] = 0;
						}
					}
				}
			}
			itc++;
		}
	}
}

int CPaint::pingpu(Mat img1, Mat &img2)
{
	//img1 = imread("");
	int rows = 0;
	int cols = 0;
	int new_rows = 5000;
	int new_cols = 5000;

	img2.create(new_rows, new_cols, img1.type());
	for (int i = 0; i < new_rows; i++)
	{
		if (rows < img1.rows)
		{
			for (int j = 0; j < new_cols; j++)
			{
				if (cols < img1.cols)
				{
					img2.at<Vec3b>(i, j)[0] = img1.at<Vec3b>(rows, cols)[0];
					img2.at<Vec3b>(i, j)[1] = img1.at<Vec3b>(rows, cols)[1];
					img2.at<Vec3b>(i, j)[2] = img1.at<Vec3b>(rows, cols)[2];
					cols += 1;
				}
				else
				{

					cols = 0;
				}
			}
			rows += 1;
			cols = 0;//ÖØÒªµÄÊÇÕâÀï&#xff0c;Ò»¶¨Òª¹éÁã
		}
		else
		{
			rows = 0;
		}

	}
	return 0;
}

int CPaint::Separation(Mat src, Mat &result)
{
	Inputparama input;
	input.thresh = 100;
	input.transparency = 255;
	input.size = 6;
	input.color = cv::Scalar(0, 0, 255);

	//clock_t s, e;
	//s = clock();
	result = BackgroundSeparation(src, input);
	//e = clock();
	//double dif = e - s;
	//cout << "time:" << dif << endl;

	return 0;
}
*/



		//case menu_select:
		//{
		//	//_frame(Rect(lf3, top3, _combo_width_text, 98)) = Scalar::all(43);

		//	//if (cvui::button(_frame, lf3, top3, select_idle, select_over, select_idle))
		//	//{
		//	//	if (!_inpaint_click && !bEmpty)
		//	//	{
		//	//		_opt = btn_select;
		//	//		_show_combo = 0;
		//	//		_working = act_display;
		//	//	}
		//	//}

		//	//top3 += TOOLBAR_BTN_STEP_Y;
		//	//bool b7 = (_rect_select.width != 0 && _opt == btn_select);
		//	//if (cvui::button(_frame, lf3, top3, !b7 ? copy_gray : copy_idle,
		//	//	!b7 ? copy_gray : copy_over, !b7 ? copy_gray : copy_over))
		//	//{
		//	//	if (!_inpaint_click && b7)
		//	//	{
		//	//		Rect rc;
		//	//		//_opt = btn_copy;
		//	//		copy_clipboard(rc);
		//	//		_show_combo = 0;
		//	//		_working = act_display;
		//	//	}
		//	//}

		//	//top3 += TOOLBAR_BTN_STEP_Y;
		//	//if (cvui::button(_frame, lf3, top3, !b7 ? cut_gray : cut_idle,
		//	//	!b7 ? cut_gray : cut_over, !b7 ? cut_gray : cut_over))
		//	//{
		//	//	if (!_inpaint_click && b7)
		//	//	{
		//	//		//_opt = btn_cut;
		//	//		cut_clipboard();
		//	//		_show_combo = 0;
		//	//		_working = act_display;
		//	//	}
		//	//}

		//}
		//break;
		//case menu_text:
		//{
		//	//top3 += TOOLBAR_BTN_STEP_Y;
		//	//_frame(Rect(lf3, top3, _combo_width_text, 98)) = Scalar::all(43);

		//	//if (cvui::button(_frame, lf3, top3, text_idle, text_over, text_idle))
		//	//{
		//	//	if (!_inpaint_click && !bEmpty)
		//	//	{
		//	//		_show_combo = 0;
		//	//		_opt = btn_draw_text;
		//	//		_working = act_display;
		//	//	}
		//	//}

		//	//top3 += TOOLBAR_BTN_STEP_Y;
		//	//if (cvui::button(_frame, lf3, top3, font_idle, font_over, font_over))
		//	//{
		//	//	if (!_inpaint_click && !bEmpty)
		//	//	{
		//	//		_tool_btn = btn_font;
		//	//	}

		//	//	_show_combo = 0;
		//	//	_working = act_display;
		//	//}

		//	//top3 += TOOLBAR_BTN_STEP_Y;
		//	//if (cvui::button(_frame, lf3, top3,
		//	//	(_input_bg_trans != 0) ? trans_idle : opaque_idle,
		//	//	(_input_bg_trans != 0) ? trans_over : opaque_over,
		//	//	(_input_bg_trans != 0) ? trans_over : opaque_over))
		//	//{
		//	//	if (!_inpaint_click && !bEmpty)
		//	//	{
		//	//		_tool_btn = btn_input_bg;
		//	//	}

		//	//	_show_combo = 0;
		//	//	_working = act_display;
		//	//}

		//}
		//break;


		/*
		void CPaint::grabcut()
		{
		if (_lena.empty())
		{
		return;
		}
		g_process = 1;

		Rect target_rc = _rect_segment;
		dst_rc_2_lena_rc(_rect_segment, target_rc);

		//Mat result;
		Mat bgModel, fgModel;
		Mat image, mask;
		Rect rc_init;

		if (_lena_h > _dst_zoom_h)
		{
		image = _dst.clone();
		rc_init = _rect_segment;

		if (!_seg_mask.empty())
		{
		mask = _seg_mask;
		}
		}
		else
		{
		image = _weightedImage.clone();
		rc_init = target_rc;

		if (!_seg_mask.empty())
		{
		mask = Mat::zeros(_weightedImage.size(), CV_8UC1);
		mask.setTo(GC_PR_FGD);

		Rect rc2(0, 0, _seg_mask.cols, _seg_mask.rows);
		Rect rc = rc2;
		dst_rc_2_lena_rc(rc2, rc);

		Mat dst;

		float f = (float)_lena_h / (float)_dst_zoom_h;
		resize(_seg_mask, dst, Size(0, 0), f, f, INTER_NEAREST);

		Rect rt = Rect(rc.x, rc.y, dst.cols, dst.rows);
		dst.copyTo(mask(rt));
		}
		}

		for (int i = 0; i < 1; i++)
		{
		//result = mask.clone();
		//grabCut(image, result, rc_init, bgModel, fgModel, 1, GC_INIT_WITH_RECT);
		g_process += 30;
		}
		//Mat src = imread("E:\\智能检测与图像处理\\3.jpg");
		////Rect rect(84, 84, 406, 318);
		//Rect rect;
		//Mat bgModel, fgModel;
		//Mat result(src.size(), CV_8U, Scalar(0));
		//Mat ROI(result(Rect(84, 84, 406, 700)));
		//ROI.setTo(GC_PR_FGD);//ROI设置为可能是前景

		//grabCut(src, result, rect, bgModel, fgModel, 1, GC_INIT_WITH_MASK);
		////threshold(result, result, 2, 255, CV_THRESH_BINARY);
		//imshow("grab", result);
		//compare(result, GC_PR_FGD, result, CMP_EQ);
		////result = result&1;
		//imshow("result", result);
		//Mat foreground(src.size(), CV_8UC3, Scalar(255, 255, 255));
		//src.copyTo(foreground, result);//意思是可以得到一个附加掩膜result的矩阵foreground
		//imshow("foreground", foreground); waitKey(0);


		Mat result(image.size(), CV_8U, Scalar(GC_PR_BGD));

		if (!mask.empty())
		{
		int w = result.cols;
		int h = result.rows;

		for (int i = 0; i < h; i++)
		{
		for (int j = 0; j < w; j++)
		{
		if (mask.at<uchar>(i, j) == 1)
		{
		result.at<uchar>(i, j) = 1;
		}
		else if (mask.at<uchar>(i, j) == 0)
		{
		result.at<uchar>(i, j) = 0;
		}
		}
		}

		//for (int i = 0; i < 1; i++)
		{
		rc_init = _rect_segment;
		grabCut(image, result, rc_init, bgModel, fgModel, 1, GC_INIT_WITH_MASK);
		g_process += 10;
		}
		}

		compare(result, GC_PR_FGD, result, CMP_EQ);

		if (!mask.empty())
		{
		int w = result.cols;
		int h = result.rows;

		for (int i = 0; i < h; i++)
		{
		for (int j = 0; j < w; j++)
		{
		if (mask.at<uchar>(i, j) == 1)
		{
		//result.at<uchar>(i, j) = 255;
		}
		}
		}
		}

		g_process = 100;

		Mat foreground(image.size(), CV_8UC3);
		image.copyTo(foreground, result);

		if (_lena_h > _dst_zoom_h)
		{
		foreground = foreground(_rect_segment);

		float f = (float)_lena_h / (float)_dst_zoom_h;
		resize(foreground, foreground, Size(0, 0), f, f, INTER_NEAREST);
		//resize(result, result, Size(0, 0), f, f, INTER_NEAREST);

		//resize(foreground, foreground, _weightedImage.size(), 0, 0, INTER_NEAREST);
		resize(result, result, _weightedImage.size(), 0, 0, INTER_NEAREST);

		//_weightedImage.setTo(0);
		Rect rt = Rect(target_rc.x, target_rc.y, foreground.cols, foreground.rows);
		foreground.copyTo(_weightedImage(rt));
		}
		else
		{
		//_weightedImage.setTo(0);
		foreground.copyTo(_weightedImage);
		}
		//_eraser_mask = Mat::zeros(result.size(), CV_8UC1);

		//save_to_file(_weightedImage, L"d:\\_weightedImage.bmp");

		_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		//_rect_segment = Rect(0, 0, 0, 0);
		_inpaint_click = false;

		float alphaReserve = 0.6;
		int	BChannel = 255;
		int	GChannel = 0;
		int	RChannel = 0;

		//Mat bg;
		//make_bgra_bg(_weightedImage, bg);

		int w = _weightedImage.cols;
		int h = _weightedImage.rows;
		for (int i = 0; i < h; i++)
		{
		for (int j = 0; j < w; j++)
		{
		if (result.at<uchar>(i, j) == 0)
		{
		//_eraser_mask.at<uchar>(i, j) = 255;
		//_weightedImage.at<Vec3b>(i, j) = bg.at<Vec3b>(i, j);

		//Vec3b& v = _weightedImage.at<Vec3b>(i, j);
		//v[0] = v[0] * alphaReserve + BChannel * (1 - alphaReserve);
		//v[1] = v[1] * alphaReserve + GChannel * (1 - alphaReserve);
		//v[2] = v[2] * alphaReserve + RChannel * (1 - alphaReserve);
		}
		}
		}

		//save_to_file(bg, L"d:\\_weightedImage2.bmp");


		//Rect rc = _rect_segment;
		//Point pt;
		//crop_ok(rc, pt);
		//_rect_segment = Rect(0, 0, _weightedImage.cols, _weightedImage.rows);

		_working = act_resize;


		//for (;;)
		//{
		//	int opt_type = 0;
		//	int len0 = _opt_arr.size();
		//	for (int i = 0; i < len0; i++)
		//	{
		//		opt_type = _opt_arr[i].opt_type;
		//	}
		//	if (opt_type == btn_seg_apply || opt_type == btn_seg_rect ||
		//		opt_type == btn_seg_fgd || opt_type == btn_seg_bgd)
		//	{
		//		_opt_arr.erase(std::begin(_opt_arr) + (len0 - 1), std::end(_opt_arr));
		//	}
		//	else
		//	{
		//		break;
		//	}
		//}

		opt_t op;
		op.opt_type = btn_seg_apply;
		op.inpaintedImage = _weightedImage.clone();
		op.pts = _pts;
		op.thickness = THICKNESS_VAL;
		add_opt(op);

		_grabcut_nb = 1;

		}
		*/

		//
		//void CPaint::grabcut()
		//{
		//	if (_lena.empty() || _seg_mask.empty())
		//	{
		//		return;
		//	}
		//	g_process = 1;
		//
		//	//Rect target_rc = _rect_segment;
		//	//dst_rc_2_lena_rc(_rect_segment, target_rc);
		//
		//	//Mat result;
		//	Mat bgModel, fgModel;
		//	Mat image, mask;
		//	Rect rc_init;
		//	if (_lena_h >= _dst_zoom_h)
		//	{
		//		//image = _weightedImage.clone();
		//		//resize(image, image, Size(_dst_zoom_w, _dst_zoom_h), 0, 0, INTER_NEAREST);
		//		//image = image(Rect(_dst_in_frame.x, _dst_in_frame.y, _seg_mask.cols, _seg_mask.rows));
		//		image = _dst.clone();
		//
		//		//image = _weightedImage(rc).clone();
		//		//resize(image, image, Size(_seg_mask.cols, _seg_mask.rows), 0, 0, INTER_NEAREST);
		//
		//		mask = _seg_mask;
		//	}
		//	else
		//	{
		//		image = _dst.clone();
		//		mask = _seg_mask.clone();
		//		//float f = (float)_lena_h / (float)_dst_zoom_h;
		//		resize(image, image, Size(_dst_in_lena.width, _dst_in_lena.height), 0, 0, INTER_NEAREST);
		//		resize(mask, mask, Size(_dst_in_lena.width, _dst_in_lena.height), 0, 0, INTER_NEAREST);
		//
		//	}
		////	else
		////	{
		////		image = _weightedImage.clone();
		////		//rc_init = target_rc;
		////
		////		mask = Mat::zeros(_weightedImage.size(), CV_8UC1);
		////		mask.setTo(GC_PR_FGD);
		////		Mat temp_mask;
		/////*
		////		Rect rc2(_dst_in_frame.x, _dst_in_frame.y, _seg_mask.cols, _seg_mask.rows);
		////		Rect rc = rc2;
		////		dst_rc_2_lena_rc(rc2, rc);
		////
		////		resize(_seg_mask, temp_mask, Size(rc.width, rc.height), 0, 0, INTER_NEAREST);
		////*/
		////
		////		float f = (float)_lena_h / (float)_dst_zoom_h;
		////		resize(_seg_mask, temp_mask, Size(0, 0), f, f, INTER_NEAREST);
		////
		////		Rect rt = Rect(_dst_in_lena.x, _dst_in_lena.y, temp_mask.cols, temp_mask.rows);
		////		temp_mask.copyTo(mask(rt));
		////	}
		//
		//
		//	//for (int i = 0; i < 1; i++)
		//	//{
		//	//	//result = mask.clone();
		//	//	//grabCut(image, result, rc_init, bgModel, fgModel, 1, GC_INIT_WITH_RECT);
		//	//	g_process += 30;
		//	//}
		//	//Mat src = imread("E:\\智能检测与图像处理\\3.jpg");
		//	////Rect rect(84, 84, 406, 318);
		//	//Rect rect;
		//	//Mat bgModel, fgModel;
		//	//Mat result(src.size(), CV_8U, Scalar(0));
		//	//Mat ROI(result(Rect(84, 84, 406, 700)));
		//	//ROI.setTo(GC_PR_FGD);//ROI设置为可能是前景
		//
		//	//grabCut(src, result, rect, bgModel, fgModel, 1, GC_INIT_WITH_MASK);
		//	////threshold(result, result, 2, 255, CV_THRESH_BINARY);
		//	//imshow("grab", result);
		//	//compare(result, GC_PR_FGD, result, CMP_EQ);
		//	////result = result&1;
		//	//imshow("result", result);
		//	//Mat foreground(src.size(), CV_8UC3, Scalar(255, 255, 255));
		//	//src.copyTo(foreground, result);//意思是可以得到一个附加掩膜result的矩阵foreground
		//	//imshow("foreground", foreground); waitKey(0);
		//
		//
		//	Mat result(image.size(), CV_8U, Scalar(GC_PR_BGD));
		//
		//	if (!mask.empty())
		//	{
		//		//int w = result.cols;
		//		//int h = result.rows;
		//
		//		//for (int i = 0; i < h; i++)
		//		//{
		//		//	for (int j = 0; j < w; j++)
		//		//	{
		//		//		if (mask.at<uchar>(i, j) == 1)
		//		//		{
		//		//			result.at<uchar>(i, j) = 1;
		//		//		}
		//		//		else if (mask.at<uchar>(i, j) == 0)
		//		//		{
		//		//			result.at<uchar>(i, j) = 0;
		//		//		}
		//		//	}
		//		//}
		//		result = mask.clone();
		//
		//		//for (int i = 0; i < 1; i++)
		//		{
		//			rc_init = Rect(0, 0, 1, 1);// _rect_segment;
		//			grabCut(image, result, rc_init, bgModel, fgModel, 1, GC_INIT_WITH_MASK);
		//			g_process += 10;
		//		}
		//
		//		compare(result, GC_PR_FGD, result, CMP_EQ);
		//
		//		int w = result.cols;
		//		int h = result.rows;
		//
		//		for (int i = 0; i < h; i++)
		//		{
		//			for (int j = 0; j < w; j++)
		//			{
		//				if (mask.at<uchar>(i, j) == 1)
		//				{
		//					result.at<uchar>(i, j) = 1;
		//				}
		//				else if (mask.at<uchar>(i, j) == 0)
		//				{
		//					result.at<uchar>(i, j) = 0;
		//				}
		//			}
		//		}
		//	}
		//
		//	g_process = 100;
		//
		//	//Mat foreground(image.size(), CV_8UC3);
		//	//image.copyTo(foreground, result);
		//
		//	if (_lena_h >= _dst_zoom_h)
		//	{
		//		//foreground = foreground(_rect_segment);
		//
		//		//float f = (float)_lena_h / (float)_dst_zoom_h;
		//		//resize(foreground, foreground, Size(0, 0), f, f, INTER_NEAREST);
		//		//resize(result, result, Size(0, 0), f, f, INTER_NEAREST);
		//
		//		//resize(foreground, foreground, _weightedImage.size(), 0, 0, INTER_NEAREST);
		//		//resize(result, result, _weightedImage.size(), 0, 0, INTER_NEAREST);
		//
		//		//_weightedImage.setTo(0);
		//		//Rect rt = Rect(target_rc.x, target_rc.y, foreground.cols, foreground.rows);
		//		//foreground.copyTo(_weightedImage(rt));
		//		resize(result, result, Size(_dst_in_lena.width, _dst_in_lena.height), 0, 0, INTER_NEAREST);
		//	}
		//	//else
		//	{
		//		_result = Mat::zeros(_weightedImage.size(), CV_8UC1);
		//		_result.setTo(0);
		//		result.copyTo(_result(_dst_in_lena));
		//		//foreground.copyTo(_weightedImage);
		//		//		Rect rt = Rect(_dst_in_lena.x, _dst_in_lena.y, temp_mask.cols, temp_mask.rows);
		//	}
		//	//_eraser_mask = Mat::zeros(result.size(), CV_8UC1);
		//
		//	//save_to_file(_weightedImage, L"d:\\_weightedImage.bmp");
		//
		//	//_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		//	//_rect_segment = Rect(0, 0, 0, 0);
		//	_inpaint_click = false;
		//
		//	//float alphaReserve = 0.6;
		//	//int	BChannel = 255;
		//	//int	GChannel = 0;
		//	//int	RChannel = 0;
		//
		//	//Mat bg;
		//	//make_bgra_bg(_weightedImage, bg);
		//
		//	//int w = _weightedImage.cols;
		//	//int h = _weightedImage.rows;
		//	//for (int i = 0; i < h; i++)
		//	//{
		//	//	for (int j = 0; j < w; j++)
		//	//	{
		//	//		if (result.at<uchar>(i, j) == 0)
		//	//		{
		//	//			//_eraser_mask.at<uchar>(i, j) = 255;
		//	//			//_weightedImage.at<Vec3b>(i, j) = bg.at<Vec3b>(i, j);
		//
		//	//			//Vec3b& v = _weightedImage.at<Vec3b>(i, j);
		//	//			//v[0] = v[0] * alphaReserve + BChannel * (1 - alphaReserve);
		//	//			//v[1] = v[1] * alphaReserve + GChannel * (1 - alphaReserve);
		//	//			//v[2] = v[2] * alphaReserve + RChannel * (1 - alphaReserve);
		//	//		}
		//	//	}
		//	//}
		//	//_result = result;
		//	//save_to_file(bg, L"d:\\_weightedImage2.bmp");
		//
		//
		//	//Rect rc = _rect_segment;
		//	//Point pt;
		//	//crop_ok(rc, pt);
		//	//_rect_segment = Rect(0, 0, _weightedImage.cols, _weightedImage.rows);
		//
		//	_working = act_resize;
		//
		//
		//	//for (;;)
		//	//{
		//	//	int opt_type = 0;
		//	//	int len0 = _opt_arr.size();
		//	//	for (int i = 0; i < len0; i++)
		//	//	{
		//	//		opt_type = _opt_arr[i].opt_type;
		//	//	}
		//	//	if (opt_type == btn_seg_apply || opt_type == btn_seg_rect || 
		//	//		opt_type == btn_seg_fgd || opt_type == btn_seg_bgd)
		//	//	{
		//	//		_opt_arr.erase(std::begin(_opt_arr) + (len0 - 1), std::end(_opt_arr));
		//	//	}
		//	//	else
		//	//	{
		//	//		break;
		//	//	}
		//	//}
		//
		//	opt_t op;
		//	op.opt_type = btn_seg_up;
		//	op.inpaintedImage = _result.clone();
		//	op.pts = _pts;
		//	op.thickness = THICKNESS_VAL;
		//	add_opt(op);
		//
		//	//_grabcut_nb = 1;
		//
		//}
		//
		//
		//void CPaint::grabcut_apply()
		//{
		//	if (_result.empty())
		//	{
		//		return;
		//	}
		//
		//	//int row = _weightedImage.rows;
		//	//int col = _weightedImage.cols;
		//
		//	int w = _result.rows;
		//	int h = _result.cols;
		//
		//	Mat bg;
		//	make_bgra_bg(_weightedImage, bg);
		//
		//	for (int i = 0; i < w; i++)
		//	{
		//		for (int j = 0; j < h; j++)
		//		{
		//			if (_result.at<uchar>(i, j) == 0)
		//			{
		//				//_eraser_mask.at<uchar>(i, j) = 255;
		//				_weightedImage.at<Vec3b>(i, j) = bg.at<Vec3b>(i, j);
		//
		//				//v[0] = 255;
		//				//v[1] = 255;
		//				//v[2] = 255;
		//			}
		//		}
		//	}
		//
		//	_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		//	_result = Mat::zeros(Size(0, 0), CV_8UC1);
		//
		//	opt_t op;
		//	op.opt_type = btn_seg_apply;
		//	op.inpaintedImage = _weightedImage.clone();
		//	op.pts = _pts;
		//	op.thickness = THICKNESS_VAL;
		//	add_opt(op);
		//
		//	//_grabcut_nb = 1;
		//}


/*
	if (!_eraser_mask.empty())
	{
		Mat mask = _eraser_mask(_dst_in_lena).clone();
		resize(mask, mask, Size(0, 0), f, f);

		Mat bg;
		make_bgra_bg(dst, bg);

		
		vector<Rect> vec_rc;
		if (!_sticker_arr.empty())
		{
			int nlen = _sticker_arr.size();
			for (int i = 0; i < nlen; i++)
			{
				Mat paper = _sticker_arr[i].dst;
				Rect rc = _sticker_arr[i].rc;

				//double f = (double)_dst_zoom_h / (double)_lena_h;
				Rect rc2 = Rect(rc.x, rc.y, paper.cols*_dst_zoom_h / _lena_h, paper.rows*_dst_zoom_h / _lena_h);
				bool b = false;
				if (rc2.x + rc2.width > dst.cols)
				{
					rc2.x = 0;
					b = true;
				}
				if (rc2.x + rc2.width > dst.cols)
				{
					rc2.width = dst.cols - rc2.x;
					b = true;
				}
				if (rc2.y + rc2.height > dst.rows)
				{
					rc2.y = 0;
					b = true;
				}
				if (rc2.y + rc2.height > dst.rows)
				{
					rc2.height = dst.rows - rc2.y;
					b = true;
				}
				if (b)
				{
					rc2 = Rect(0, 0, rc2.width, rc2.height);
					_sticker_arr[i].rc = rc2;
				}

				vec_rc.push_back(rc2);
			}
		}
		
		int len = vec_rc.size();
		

		Point pt;
		for (pt.y = 0; pt.y < dst.rows; pt.y++)
		{
			for (pt.x = 0; pt.x < dst.cols; pt.x++)
			{
				
				bool b = false;
				for (int k = 0; k < len; k++)
				{
					Rect rc = vec_rc[k];
					if (pt.x >= rc.x && pt.x <= rc.x + rc.width && pt.y >= rc.y && pt.y <= rc.y + rc.height)
					{
						b = true;
						break;
					}
				}

				if (!b)
				
				{
					Vec4b & v = dst.at<Vec4b>(pt);
					if (v[3] != 255)
					{
						Vec3b b = bg.at<Vec3b>(pt);
						v[0] = b[0];
						v[1] = b[1];
						v[2] = b[2];
					}
				}
			}
		}

		cv::cvtColor(dst, dst, CV_BGRA2BGR);
	}

*/

/*
void CPaint::sticker_ok(Mat weighted, int btn)
{
int len = _sticker_arr.size();
for (int k = 0; k < len; k++)
{
Rect rc0 = _sticker_arr[k].rc;
Mat tmp = _sticker_arr[k].dst;
Rect rc = rc0;
rect_base_lena(rc, rc0);

rc.width = min(tmp.cols, weighted.cols);
rc.height = min(tmp.rows, weighted.rows);

//double f = (double)_lena_h / (double)_dst_zoom_h;
rc.x = _dst_in_lena.x + rc.x * _lena_h / _dst_zoom_h;
rc.y = _dst_in_lena.y + rc.y * _lena_h / _dst_zoom_h;

if (rc.x + rc.width > weighted.cols)
{
rc.width = weighted.cols - rc.x;
}
if (rc.y + rc.height > weighted.rows)
{
rc.height = weighted.rows - rc.y;
}

//Rect rc2(rc.x - _dst_in_lena.x, rc.y - _dst_in_lena.y,rc.width, rc.height);
//rc2.x = rc2.x *_dst_zoom_h / _lena_h;
//rc2.y = rc2.y *_dst_zoom_h / _lena_h;
//rc2.width = rc2.width *_dst_zoom_h / _lena_h;
//rc2.height = rc2.height *_dst_zoom_h / _lena_h;

tmp = tmp(Rect(0, 0, rc.width, rc.height));

int channels = tmp.channels();

if (_seamless_mode == 9)
{
if (channels == 4)
{
Mat d2 = weighted(rc);
set_dst_by_v3(d2, tmp, 255);
}
else
{
tmp.copyTo(weighted(rc));
}
}
else if (_seamless_mode == 10)
{
//if (channels == 4)
//{
//	cv::cvtColor(tmp, tmp, CV_BGRA2BGR);
//}
//Rect roi_rect;
//Mat cpy_dst = weighted(rc);
//seamless_10(cpy_dst, roi_rect, tmp, Point(0, 0));
}
else
{
Mat mask(tmp.size(), CV_8UC1,  Scalar::all(255));//paper.at<Vec3b>(0, 0)
Point center(rc.x + rc.width / 2, rc.y + rc.height / 2);
if (channels == 4 || !_eraser_mask.empty())
{
mask.setTo(0);
}

bool b = false;
if (channels == 4)
{
b = set_mask_by_v3(mask, tmp, 255);

cv::cvtColor(tmp, tmp, CV_BGRA2BGR);
}

if (!_eraser_mask.empty())
{
Mat roi = _eraser_mask(rc);
b = set_mask_by_v1(mask, roi, 255);
}

if (!b)
{
mask.setTo(255);
}

seamlessClone(tmp, weighted, mask, center, weighted, _seamless_mode);
}

if (!_eraser_mask.empty())
{
Mat roi = _eraser_mask(rc);
Mat d2 = weighted(rc);

for (int i = 0; i < tmp.rows; i++)
{
for (int j = 0; j < tmp.cols; j++)
{
//if (d2.at<Vec3b>(i, j) != Vec3b(255, 255, 255))
{
//roi.at<uchar>(i, j) = 0;
}
}
}
}

}

if (btn != 0)
{
_sticker_arr.clear();
_pt_prev = Point(0, 0);
_pt_down = Point(0, 0);

int opt_type = 0;
int thickness = 0;
int len0 = _opt_arr.size();
for (int i = 0; i < len0; i++)
{
opt_type = _opt_arr[i].opt_type;
thickness = _opt_arr[i].thickness;
}
if (opt_type == btn_sticker || opt_type == btn_change_bg)
{
_opt_arr.erase(std::end(_opt_arr) - 1);
}

opt_t op;
op.opt_type = btn;
op.nav_type = 0;
op.thickness = 0;
//op.sticker_arr = _sticker_arr;
op.image = weighted.clone();
add_opt(op);

_working = act_display;
}
}
*/


/*
//display
if (!_sticker_arr.empty())
{
int nlen = _sticker_arr.size();
for (int i = 0; i < nlen; i++)
{
Mat paper = _sticker_arr[i].dst;
Rect rc;
rect_base_lena(rc, _sticker_arr[i].rc);

Mat tmp;
//double f = (double)_dst_zoom_h / (double)_lena_h;

Rect rc2 = Rect(rc.x, rc.y, paper.cols*_dst_zoom_h / _lena_h, paper.rows*_dst_zoom_h / _lena_h);
resize(paper, tmp, Size(rc2.width, rc2.height), 0, 0);

if (dst2.empty())
{
dst2 = dst.clone();
}

bool b = false;
if (rc2.x + rc2.width > dst2.cols)
{
rc2.x = 0;
b = true;
}
if (rc2.x + rc2.width > dst2.cols)
{
rc2.width = dst2.cols - rc2.x;
b = true;
}
if (rc2.y + rc2.height > dst2.rows)
{
rc2.y = 0;
b = true;
}
if (rc2.y + rc2.height > dst2.rows)
{
rc2.height = dst2.rows - rc2.y;
b = true;
}
if (b)
{
rc2 = Rect(0, 0, rc2.width, rc2.height);
tmp = tmp(rc2);
_sticker_arr[i].rc = rc2;
}

Point center(rc2.x + rc2.width / 2, rc2.y + rc2.height / 2);

int channels = tmp.channels();
if (_seamless_mode == 9)
{
if (channels == 4)
{
Mat d2 = dst2(rc2);
set_dst_by_v3(d2, tmp, 255);
}
else
{
tmp.copyTo(dst2(rc2));
}
}
else if (_seamless_mode == 10)
{
//if (channels == 4)
//{
//	cv::cvtColor(tmp, tmp, CV_BGRA2BGR);
//}

//Rect roi_rect;
//Mat cpy_dst = dst2(rc2);
//seamless_10(cpy_dst, roi_rect, tmp, Point(0, 0));

}
else
{
//Mat gray, mask;
//cvtColor(tmp, gray, COLOR_BGR2GRAY);
//threshold(gray, mask, 0, 255, THRESH_BINARY | THRESH_OTSU);
//Mat k = getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1));
//dilate(mask, mask, k);
//NORMAL_CLONE, cv::MIXED_CLONE or cv::MONOCHROME_TRANSFER
Mat mask(tmp.size(), CV_8U,  Scalar::all(255));
if (channels == 4)
{
mask.setTo(0);

bool b = set_mask_by_v3(mask, tmp, 255);
cv::cvtColor(tmp, tmp, CV_BGRA2BGR);
if (!b)
{
mask.setTo(255);
}
}

seamlessClone(tmp, dst2, mask, center, dst2, _seamless_mode);
}

dst2.copyTo(imageROI);

Point pt1 = Point(rc2.x + _dst_in_frame.x, rc2.y + _dst_in_frame.y);
Point pt2 = Point(pt1.x + rc2.width, pt1.y + rc2.height);

drawDashRect(_frame, 5, 5, pt1, pt2, CV_RGB(255, 255, 255), THICKNESS_VAL);
}
}

void CPaint::high_displayImage()
{
	if (_lena2.empty())
	{
		reset_frame();
		return;
	}

	int x0 = 0;
	int y0 = 0;
	Mat dst;

	Rect rc = get_display_win(Point(_lena_in_frame.x, _lena_in_frame.y), _dst_zoom_w, _dst_zoom_h, x0, y0);
	if (rc.width == 1 || rc.height == 1)
	{
		//WritePrivate("high_displayImage get_display_win", "out ruange");
		_lena_in_frame.x = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
		_lena_in_frame.y = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

		rc = get_display_win(Point(_lena_in_frame.x, _lena_in_frame.y), _dst_zoom_w, _dst_zoom_h, x0, y0);
	}

	int w0 = rc.width *_lena_h / _dst_zoom_h;
	int h0 = rc.height  *_lena_h / _dst_zoom_h;
	int x2 = rc.x *_lena_h / _dst_zoom_h;
	int y2 = rc.y  *_lena_h / _dst_zoom_h;

	if (x2 + w0 > _weightedImage.cols || y2 + h0 > _weightedImage.rows)
	{
		w0 = _weightedImage.cols - x2;
		h0 = _weightedImage.rows - y2;
		WritePrivate("high_displayImage error", "out ruange");
	}

	_dst_in_lena = Rect(x2, y2, w0, h0);

	if (!_eraser_mask.empty())
	{
		if (_idx != _mask_idx)
		{
			Mat tm;
			int len3 = countNonZero(_inpaintMask);
			if (len3 > 0)
			{
				tm = Mat::zeros(_weightedImage.size(), CV_8UC1);
				//_eraser_mask.copyTo(tm, ~_inpaintMask);
				_eraser_mask.copyTo(tm);

				set_mask_by_v1(tm, _inpaintMask, 255);

				//subtract(cv::Scalar(255,255,255),image,image_inv);
				//bitwrise_xor(_eraser_mask, _inpaintMask, tm);
			}
			else
			{
				tm = _eraser_mask;
			}

			cv::cvtColor(_weightedImage, _weightedImage4, CV_BGR2BGRA);
			vector<int> fromto = { 0, 3 };
			mixChannels(tm, _weightedImage4, fromto);
			_mask_idx = _idx;
		}
		dst = _weightedImage4(_dst_in_lena).clone();
	}
	else
	{
		dst = _weightedImage(_dst_in_lena).clone();
	}

	if (dst.cols > 1)
	{
		double f = (double)_dst_zoom_h / (double)_lena_h;
		resize(dst, dst, Size(0, 0), f, f);
	}

	int w = dst.cols;
	if (dst.cols > rc.width)
	{
		w = rc.width;
	}
	int h = dst.rows;
	if (dst.rows > rc.height)
	{
		h = rc.height;
	}

	if (w != dst.cols || h != dst.rows)
	{
		dst = dst(Rect(0, 0, w, h));
	}

	if (!_eraser_mask.empty())
	{
		//_dst_mask = Mat::zeros(dst.size(), CV_8UC1);
		//vector<int> fromto = { 3, 0 };
		//mixChannels(dst, _dst_mask, fromto);

		if (_bg.empty() || dst.cols != _dst.cols || dst.rows != _dst.rows)
		{
			make_bgra_bg(dst, _bg);
		}

		Point pt;
		for (pt.y = 0; pt.y < dst.rows; pt.y++)
		{
			Vec4b * p1 = dst.ptr<Vec4b>(pt.y);
			for (pt.x = 0; pt.x < dst.cols; pt.x++)
			{
				if (p1[pt.x][3] != 255)
				{
					p1[pt.x][0] = _bg.at<Vec3b>(pt)[0];
					p1[pt.x][1] = _bg.at<Vec3b>(pt)[1];
					p1[pt.x][2] = _bg.at<Vec3b>(pt)[2];
				}
			}
		}

		cv::cvtColor(dst, dst, CV_BGRA2BGR);
	}

	displayImage(1, dst, x0, y0);
}

double CPaint::init_zoom_r()
{
	double r = 1.0f;
	if (_lena2.empty())
	{
		return r;
	}

	int f = _dst_zoom_w * 100 / _lena_w;

	if (f >= 400)
	{
		r = 4.0f;
	}
	else if (f >= 350)
	{
		r = 3.5f;
	}
	else if (f >= 300)
	{
		r = 3.0f;
	}
	else if (f >= 250)
	{
		r = 2.5f;
	}
	else if (f >= 200)
	{
		r = 2.0f;
	}
	else if (f >= 150)
	{
		r = 1.5f;
	}
	else if (f >= 100)
	{
		r = 1.0f;
	}
	else if (f >= 90)
	{
		r = 0.9f;
	}
	else if (f >= 80)
	{
		r = 0.8f;
	}
	else if (f >= 70)
	{
		r = 0.7f;
	}
	else if (f >= 60)
	{
		r = 0.6f;
	}
	else if (f >= 50)
	{
		r = 0.5f;
	}
	else if (f >= 40)
	{
		r = 0.4f;
	}
	else if (f >= 30)
	{
		r = 0.3f;
	}
	else if (f >= 20)
	{
		r = 0.2f;
	}
	else if (f >= 10)
	{
		r = 0.1f;
	}
	else if (f >= 9)
	{
		r = 0.09f;
	}
	else if (f >= 8)
	{
		r = 0.08f;
	}
	else if (f >= 7)
	{
		r = 0.07f;
	}
	else
	{
		r = 0.06f;
	}
	return r;
}


//void CPaint::rect_base_lena(Rect &rc, Rect sr)
//{
//	rc.x = sr.x * _lena_w / _dst_zoom_w;
//	rc.y = sr.y * _lena_w / _dst_zoom_w;
//	rc.width = sr.width * _lena_w / _dst_zoom_w;
//	rc.height = sr.height * _lena_w / _dst_zoom_w;
//
//	rc.x = rc.x * _dst_zoom_w / _lena_w;
//	rc.y = rc.y * _dst_zoom_w / _lena_w;
//	rc.width = rc.width * _dst_zoom_w / _lena_w;
//	rc.height = rc.height * _dst_zoom_w / _lena_w;
//
//	if (sr.x != rc.x || sr.y != rc.y || sr.width != rc.width || sr.height != rc.height)
//	{
//		WritePrivate("_clip_mat display", "out ruange");
//		bool b = true;
//	}
//}

*/


		/*

				int k = cvui::lastKeyPressed();
				if (k > 0)
				{
					//std::cout << "lastKeyPressed: " << k << std::endl;
					switch (k)
					{
					case 26:
						_tool_btn = toolbar_undo;
						break;
					case 25:
						_tool_btn = toolbar_redo;
						break;
					//case 0x2e:
					//	_bMakeBGRA = true;
					//	_working = act_display;
					//	break;
					default:
						break;
					}
				}
				*/


	/*
	VideoCapture capture(_w2u(szSrc).c_str());
	if (capture.isOpened())
	{
		width = (long)capture.get(CAP_PROP_FRAME_WIDTH);
		height = (long)capture.get(CAP_PROP_FRAME_HEIGHT);
		int fps = (int)capture.get(CAP_PROP_FPS);
		int numFrames = (int)capture.get(CAP_PROP_FRAME_COUNT);

		duration = numFrames / fps;

		capture.release();
		return 0;
	}
	return 2;
	*/



	/*
	ret = 2;
	VideoCapture capture(_w2u(szSrc).c_str(), CAP_MSMF);

	if (capture.isOpened())
	{
		width = (long)capture.get(CAP_PROP_FRAME_WIDTH);
		height = (long)capture.get(CAP_PROP_FRAME_HEIGHT);
		int fps = (int)capture.get(CAP_PROP_FPS);
		int numFrames = (int)capture.get(CAP_PROP_FRAME_COUNT);

		duration = numFrames / fps;

		for (int i = 0; i < 50; i++)
		{
			Mat frame;
			Mat gray;
			if (capture.read(frame))
			{
				cvtColor(frame, gray, COLOR_BGR2GRAY, 1);
				if (countNonZero(gray) != 0)
				{
					long level = 0;
					ret = save_mat(frame, "2", szDst, level);
					break;
				}

				Sleep(10);
			}
		}
		capture.release();
	}
	return ret;
	


CV_API int capture_gif(int type, const wchar_t* szSrc, const wchar_t* szDst, long &width, long &height)
{
	int ret = 2;

	HANDLE hFile = CreateFileW(szSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		int fd = ::_open_osfhandle(reinterpret_cast <::intptr_t> (hFile), _A_RDONLY); // transferring h_file ownerhip
		if (-1 != fd)
		{
			int err;
			GifFileType* f = DGifOpenFileHandle(fd, &err);
			if (f != NULL)
			{
				ret = DGifSlurp(f);
				if (ret == GIF_OK)
				{
					width = f->SWidth;
					height = f->SHeight;

					int len3 = 0;
					Mat img;
					for (int i = 0; i < f->ImageCount; i++)
					{
						//SavedImage* image = &f->SavedImages[0]; // that compile but the result is a scratched img
						//img = Mat(Size(width, height), CV_8UC1, image->RasterBits);
						//len3 = countNonZero(img);
						//if (len3 != 0)
						//{
						//	break;
						//}
					}

					long level = 0;
					if (len3 == 0)
					{
						long mp4rotate = 0;
						HRESULT hr = GetThumbnailEx(type, max(width, height), szSrc, szDst, mp4rotate);
						if (hr != S_OK)
						{
							Sleep(100);
							hr = GetCacheThumbnail(type, width, height, szSrc, szDst, mp4rotate);
						}
					}
					else
					{
						ret = save_mat(img, "2", szDst, level);
					}

				}
				DGifCloseFile(f, &err);
			}
			else
			{
				CloseHandle(hFile);
			}
		}
		else
		{
			CloseHandle(hFile);
		}
	}
	return ret;
}
*/


	/*

	mp3dec_map_info_t map_info;
	ret = mp3dec_open_file(szSrc, &map_info);
	if (ret != 0)
	{
		return ret;
	}

	mp3dec_t mp3d;
	mp3dec_file_info_t info;
	memset(&info, 0, sizeof(mp3dec_file_info_t));

	ret = mp3dec_load_buf(&mp3d, map_info.buffer, map_info.size, &info);
	if (info.samples)
	{
		double m = (info.frame_len * 1.0) / info.hz;

		int FrameCount = info.samples;
		duration = (FrameCount*m);

		if (info.buffer_size > 0)
		{
			vector<uchar> vec_data(info.buffer, info.buffer + info.buffer_size);
			cv::Mat src = cv::imdecode(vec_data, 1);

			if (src.data == NULL)
			{
				return 2;
			}

			string t = "2";
			if (type == 1) {
				t = "1";
			}

			long level = 0;
			save_mat(src, t, szDst, level);

			cover = true;
			//DWORD dwWritten = 0;

			//HANDLE hWrite = CreateFileW(dst.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			//	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			//if (hWrite != INVALID_HANDLE_VALUE)
			//{
			//	WriteFile(hWrite, info.buffer, info.buffer_size, &dwWritten, NULL);
			//	CloseHandle(hWrite);
			//}
		}

		mp3dec_close_file(&map_info);
	}
	*/
/*
static void initGMMs(const Mat& img, const Mat& mask, GMM& bgdGMM, GMM& fgdGMM)
{
const int kMeansItCount = 10;
const int kMeansType = KMEANS_PP_CENTERS;

Mat bgdLabels, fgdLabels;
std::vector<Vec3f> bgdSamples, fgdSamples;
Point p;
for (p.y = 0; p.y < img.rows; p.y++)
{
for (p.x = 0; p.x < img.cols; p.x++)
{
if (mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD)
bgdSamples.push_back((Vec3f)img.at<Vec3b>(p));
else // GC_FGD | GC_PR_FGD
fgdSamples.push_back((Vec3f)img.at<Vec3b>(p));
}
}
CV_Assert(!bgdSamples.empty() && !fgdSamples.empty());
{
Mat _bgdSamples((int)bgdSamples.size(), 3, CV_32FC1, &bgdSamples[0][0]);
int num_clusters = GMM::componentsCount;
num_clusters = std::min(num_clusters, (int)bgdSamples.size());
kmeans(_bgdSamples, num_clusters, bgdLabels,
TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType);
}
{
Mat _fgdSamples((int)fgdSamples.size(), 3, CV_32FC1, &fgdSamples[0][0]);
int num_clusters = GMM::componentsCount;
num_clusters = std::min(num_clusters, (int)fgdSamples.size());
kmeans(_fgdSamples, num_clusters, fgdLabels,
TermCriteria(CV_TERMCRIT_ITER, kMeansItCount, 0.0), 0, kMeansType);
}

bgdGMM.initLearning();
for (int i = 0; i < (int)bgdSamples.size(); i++)
bgdGMM.addSample(bgdLabels.at<int>(i, 0), bgdSamples[i]);
bgdGMM.endLearning();

fgdGMM.initLearning();
for (int i = 0; i < (int)fgdSamples.size(); i++)
fgdGMM.addSample(fgdLabels.at<int>(i, 0), fgdSamples[i]);
fgdGMM.endLearning();
}

static double calcBeta(const Mat& img)
{
double beta = 0;
for (int y = 0; y < img.rows; y++)
{
for (int x = 0; x < img.cols; x++)
{
Vec3d color = img.at<Vec3b>(y, x);
if (x > 0) // left
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y, x - 1);
beta += diff.dot(diff);
}
if (y > 0 && x > 0) // upleft
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x - 1);
beta += diff.dot(diff);
}
if (y > 0) // up
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x);
beta += diff.dot(diff);
}
if (y > 0 && x < img.cols - 1) // upright
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x + 1);
beta += diff.dot(diff);
}
}
}
if (beta <= std::numeric_limits<double>::epsilon())
beta = 0;
else
beta = 1.f / (2 * beta / (4 * img.cols*img.rows - 3 * img.cols - 3 * img.rows + 2));

return beta;
}

static void calcNWeights(const Mat& img, Mat& leftW, Mat& upleftW, Mat& upW, Mat& uprightW, double beta, double gamma)
{
const double gammaDivSqrt2 = gamma / std::sqrt(2.0f);
leftW.create(img.rows, img.cols, CV_64FC1);
upleftW.create(img.rows, img.cols, CV_64FC1);
upW.create(img.rows, img.cols, CV_64FC1);
uprightW.create(img.rows, img.cols, CV_64FC1);
for (int y = 0; y < img.rows; y++)
{
for (int x = 0; x < img.cols; x++)
{
Vec3d color = img.at<Vec3b>(y, x);
if (x - 1 >= 0) // left
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y, x - 1);
leftW.at<double>(y, x) = gamma * exp(-beta*diff.dot(diff));
}
else
leftW.at<double>(y, x) = 0;
if (x - 1 >= 0 && y - 1 >= 0) // upleft
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x - 1);
upleftW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
}
else
upleftW.at<double>(y, x) = 0;
if (y - 1 >= 0) // up
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x);
upW.at<double>(y, x) = gamma * exp(-beta*diff.dot(diff));
}
else
upW.at<double>(y, x) = 0;
if (x + 1 < img.cols && y - 1 >= 0) // upright
{
Vec3d diff = color - (Vec3d)img.at<Vec3b>(y - 1, x + 1);
uprightW.at<double>(y, x) = gammaDivSqrt2 * exp(-beta*diff.dot(diff));
}
else
uprightW.at<double>(y, x) = 0;
}
}
}

static void assignGMMsComponents(const Mat& img, const Mat& mask, const GMM& bgdGMM, const GMM& fgdGMM, Mat& compIdxs)
{
Point p;
for (p.y = 0; p.y < img.rows; p.y++)
{
for (p.x = 0; p.x < img.cols; p.x++)
{
Vec3d color = img.at<Vec3b>(p);
compIdxs.at<int>(p) = mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD ?
bgdGMM.whichComponent(color) : fgdGMM.whichComponent(color);
}
}
}

static void learnGMMs(const Mat& img, const Mat& mask, const Mat& compIdxs, GMM& bgdGMM, GMM& fgdGMM)
{
bgdGMM.initLearning();
fgdGMM.initLearning();
Point p;
for (int ci = 0; ci < GMM::componentsCount; ci++)
{
for (p.y = 0; p.y < img.rows; p.y++)
{
for (p.x = 0; p.x < img.cols; p.x++)
{
if (compIdxs.at<int>(p) == ci)
{
if (mask.at<uchar>(p) == GC_BGD || mask.at<uchar>(p) == GC_PR_BGD)
bgdGMM.addSample(ci, img.at<Vec3b>(p));
else
fgdGMM.addSample(ci, img.at<Vec3b>(p));
}
}
}
}
bgdGMM.endLearning();
fgdGMM.endLearning();
}
*/


	/*
	Mat& mask0 = mask;//.getMatRef();
	Mat& bgdModel0 = bgModel;//.getMatRef();
	Mat& fgdModel0 = fgModel;//.getMatRef();
	GMM bgdGMM(bgdModel0);
	GMM fgdGMM(fgdModel0);
	Mat compIdxs(image.size(), CV_32SC1);
	initGMMs(image, mask0, bgdGMM, fgdGMM);


	const double gamma = 50;
	const double lambda = 9 * gamma;
	const double beta = calcBeta(image);

	Mat leftW, upleftW, upW, uprightW;
	calcNWeights(image, leftW, upleftW, upW, uprightW, beta, gamma);

	//for( int i = 0; i < iterCount; i++ )
	{
		//GCGraph<double> graph;
		assignGMMsComponents(image, mask, bgdGMM, fgdGMM, compIdxs);
		//if( mode != GC_EVAL_FREEZE_MODEL )
		learnGMMs(image, mask, compIdxs, bgdGMM, fgdGMM);
		//constructGCGraph(image, mask, bgdGMM, fgdGMM, lambda, leftW, upleftW, upW, uprightW, graph );
	   // estimateSegmentation( graph, mask );
	}

	*/


//void CPaint::coi_bg(Mat &dst)
//{
//	int channels = dst.channels();
//	if(channels != 4)
//	{
//		return;
//	}
//
//	Mat mask = Mat(dst.size(), CV_8UC1);
//	vector<int> fromto = { 3, 0 };
//	mixChannels(dst, mask, fromto);
//	
//	cv::cvtColor(dst, dst, CV_BGRA2BGR);
//	
//	Mat bg;
//	make_bgra_bg(dst, bg);
//
//	Point pt;
//	for (pt.y = 0; pt.y < mask.rows; pt.y++)
//	{
//		uchar * p1 = mask.ptr<uchar>(pt.y);
//		for (pt.x = 0; pt.x < mask.cols; pt.x++)
//		{
//			if (p1[pt.x] == 0)
//			{
//				dst.at<Vec3b>(pt) = bg.at<Vec3b>(pt);
//			}
//		}
//	}
//}


//UINT __stdcall CPaint::max_cmd(LPVOID pParam)
//{
//	CPaint * pThis = (CPaint*)pParam;
//
//	RECT rect;
//	GetWindowRect(pThis->_win_handle, &rect);
//
//	HMONITOR hMonitor;
//	MONITORINFO mi;
//	hMonitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
//
//	mi.cbSize = sizeof(mi);
//	GetMonitorInfo(hMonitor, &mi);
//
//	Rect position;
//	//fullscreen
//	position.x = mi.rcMonitor.left; position.y = mi.rcMonitor.top;
//	position.width = mi.rcMonitor.right - mi.rcMonitor.left; position.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
//
//	//before max
//	HWND pParent = ::GetParent(pThis->_win_handle);
//	MoveWindow(pParent, position.x, position.y, position.width, position.height, FALSE);
//
//	PostMessage(pParent, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
//	return 0;
//}


//string CLang::procLangData(string common)
//{
//	_langMap.clear();
//
//	string fout = "";
//	int pattern = 3;
//
//	char last = '\0';
//	for (size_t i = 0; i < common.length(); i++)
//	{
//		StatusChange(fout, pattern, common[i], last);
//	}
//
//	string key = "";
//	string val = "";
//	vector<string> v1 = tokenize(fout, "\n", true, "");
//	size_t len = v1.size();
//
//	for (size_t i = 0; i < len; i++)
//	{
//		string::size_type ndx = v1[i].find('=');
//		if (ndx != string::npos)
//		{
//			// yup; save the args....
//			val = v1[i].substr(ndx + 1);
//			// strip from file name....
//			key = v1[i].substr(0, ndx);
//			trim(key, " ");
//			trim(key, "'");
//
//			rtrim(val, ";");
//			trim(val, " ");
//			trim(val, "'");
//			trim(val, "\"");
//			//Replace(val, "\"", "\\\"");
//
//			_langMap.insert(map<string, string>::value_type(key, val));
//		}
//	}
//	return "";
//}

//void CLang::StatusChange(string& fout, int& pattern, char& c, char& last)
//{
//	switch (pattern)
//	{
//	case 0:
//	{
//		fout += c;
//		if (c == '"')
//			pattern = 3;
//		break;
//	}
//	case 1:
//	{
//		if (last != '\\' && c == '\n')
//		{
//			fout += c;
//			c = '\0';
//			pattern = 3;
//		}
//
//		break;
//	}
//	case 2:
//	{
//		if (last == '*' && c == '/')
//		{
//			c = '\0';
//			pattern = 3;
//		}
//		break;
//	}
//	case 3:
//	{
//		//fout<<c;
//		if (c == '"')
//		{
//			fout += c;
//			pattern = 0;
//		}
//		else if (c == '/')
//		{
//			if (last == '/')
//				pattern = 1;
//		}
//		else if (c == '*')
//		{
//			if (last == '/')
//				pattern = 2;
//		}
//		else
//		{
//			if (last == '/')
//			{
//				fout += last;
//			}
//
//			fout += c;
//			pattern = 3;
//		}
//		break;
//	}
//	}
//	last = c;
//}

//long CLang::Read_file(HINSTANCE hrcInst, UINT cr_id, string& szhtml)
//{
//	szhtml = "";
//
//	HRSRC hrs = FindResource(hrcInst, MAKEINTRESOURCE(cr_id), RT_HTML);
//	if (hrs != NULL)
//	{
//		DWORD dw = SizeofResource(hrcInst, hrs);
//
//		HGLOBAL hg = LoadResource(hrcInst, hrs);
//
//		if (hg != NULL)
//		{
//			void* p = LockResource(hg);
//			if (p != NULL)
//			{
//				szhtml = string((LPSTR)p, dw);
//			}
//			FreeResource(hg);
//		}
//	}
//	return 0;
//}



//PHOTOL_API int resize_img2(const wchar_t* szSrc, const wchar_t* szDst, long width, long height)
//{
//	wchar_t src[MAX_PATH] = { 0 };
//	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);
//
//	ifstream file(src, ios::in | ios::binary | ios::ate);
//	if (!file.is_open())
//	{
//		return -1;
//	}
//
//	streampos size = file.tellg();
//	file.seekg(0, ios::beg);
//	string buffer(size, 0);
//	file.read(&buffer[0], size);
//	file.close();
//
//	vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
//	Mat src2 = imdecode(vec_data, IMREAD_UNCHANGED);
//	int u = src2.depth();
//	if (src2.data != NULL && u != 0)
//	{
//		Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
//		temp.convertTo(src2, CV_8U);
//	}
//
//	if (src2.data == NULL)
//	{
//		return 2;
//	}
//
//	string ext = ".png";
//
//	int w = src2.cols;
//	int h = src2.rows;
//	double scale = get_scale(2, w, h);
//	if (width != 0 && height != 0)
//	{
//		scale = (double)width / (double)w;
//		ext = ".jpg";
//	}
//
//	Size ResImgSiz = Size(0, 0);
//	Mat ResImg2 = Mat(ResImgSiz, src2.type());
//	if (min(width, height) * scale < 2)
//	{
//		ResImg2 = src2;
//	}
//	else
//	{
//		resize(src2, ResImg2, ResImgSiz, scale, scale, CV_INTER_CUBIC);
//	}
//
//	vector<uchar> buf;
//	imencode(ext.c_str(), ResImg2, buf);
//
//	ofstream file2(szDst, ios::out | ios::binary);
//	if (!file2)
//	{
//		return -1;
//	}
//	file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
//	file2.close();
//
//	return 0;
//}



