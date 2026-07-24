///////////////////////////////////////////////////////////////////////
/// @file paint_utils.cpp
/// @brief 绘图模块 - 工具函数实现
/// @details 实现绘图工具的通用函数,包括导航按钮状态管理、
///          配置文件读写、SVG 处理、文件系统操作等
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件依赖 cvui, SimpleIni, lunasvg 等第三方库
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
#include<fstream>
#include "lunasvg.h"
#include "unitil2.h"
extern HINSTANCE _his;

bool CPaint::show_unapplied(int btn, bool state)
{
	_sel_nav_btn = btn;
	_sel_nav_state = state;

	if (!_seg_mask.empty() || !_sticker_arr.empty() || canInpaint())
	{
		_do_nav_btn = btn_nothing;
		_do_nav_state = false;
		return true;
	}
	return false;
}

bool CPaint::is_unapplied(bool canInp)
{
	if (!_seg_mask.empty() || !_sticker_arr.empty() || canInp)
	{
		_do_nav_btn = _nav_btn;
		_do_nav_state = true;
		return true;
	}

	return false;
}

void CPaint::show_msg(int x, int y, string txt, Scalar backcolor, Scalar color)
{
	wstring val = _u2w(_lang.trans(txt));

	Mat lbl = get_lbl(val.c_str(), backcolor, color);

	if (x == 0)
	{
		x = (_win_w - lbl.cols) / 2;
	}
	Rect rect(x, y, lbl.cols + 40, lbl.rows + 10);
	rectangle(_frame, rect, backcolor, -1, LINE_8, 0);
	lbl.copyTo(_frame(Rect(x + 5, y + 5, lbl.cols, lbl.rows)));
}

bool CPaint::isdo_nothing()
{
	if (_lena2.empty())
	{
		return true;
	}

	if (_rect_crop.width != 0 || _rect_select.width != 0)
	{
		return true;
	}
	if (_hInputWnd != NULL)
	{
		return true;
	}
	if (!_seg_mask.empty())
	{
		return true;
	}
	if (!_sticker_arr.empty())
	{
		return true;
	}

	if (!_clip_mat.empty())
	{
		return true;
	}

	return false;
}

void CPaint::select_btn(int opt)
{
	_opt = opt;
	_last_opt = opt;
	_pointer_btn = 0;
}


void CPaint::select_btn2(int opt)
{
	_opt = opt;
	_last_opt = opt;
	_pointer_btn = 0;
	_working = act_tips_ui;
}

void CPaint::blank_click()
{
	if (!_inpaint_click)
	{
		bool bexist = false;
		bool state = true;
		if (_nav_btn == nav_inp)
		{
			bexist = true;
			state = (_opt >= btn_brush && _opt <= btn_inpaint) ? true : false;
		}
		else if (_nav_btn == nav_segment)
		{
			bexist = true;
			_down_sticker_click = false;
			state = (_opt == btn_seg_fgd || _opt == btn_seg_bgd || _opt == btn_change_bg || _opt == btn_seg_rect) ? true : false;
		}
		else if (_nav_btn == nav_sticker)
		{
			bexist = true;
			_down_sticker_click = false;
			state = (_opt == btn_sticker) ? true : false;
		}
		else if (_nav_btn == nav_filter)
		{
			bexist = true;
			state = false;
		}
		else if (_nav_btn == nav_draw)
		{
			bexist = true;
			state = (_opt >= btn_draw_free && _opt <= btn_draw_select) ? true : false;
		}
	}

	if (_working != act_display)
	{
		_working = act_tips_ui;
	}
}

void CPaint::print_bottom_info(int x, wchar_t* buf, int fx)
{
	Mat lbl = get_lbl(buf, Scalar::all(43), Scalar::all(128));
	if (!lbl.empty() && lbl.rows <= 24)
	{
		Rect rc;
		rc.x = (fx == 0) ? x - lbl.cols : x;
		rc.y = _win_h + (FOOTER_HEIGHT - lbl.rows) / 2;

		rc.width = lbl.cols;
		rc.height = lbl.rows;

		lbl.copyTo(_frame(rc));
	}
}

//void CPaint::load_bitmap(Mat& img, UINT resID)
//{
//	HBITMAP hbmp = ::LoadBitmap(_his, MAKEINTRESOURCE(resID));
//	DWORD err = GetLastError();
//	if (hbmp)
//	{
//		BITMAP bm = { 0 };
//		int ret = GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bm);
//		if (ret == 0 || ret > sizeof(BITMAP))
//		{
//		}
//		else
//		{
//			int nChannels = bm.bmBitsPixel == 1 ? 1 : bm.bmBitsPixel / 8;
//			int depth = bm.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
//
//			//Mat src;CV_8UC3
//			img.create(cvSize(bm.bmWidth, bm.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
//			GetBitmapBits(hbmp, bm.bmHeight * bm.bmWidth * nChannels, img.data);
//
//			if (nChannels == 4)
//			{
//				cvtColor(img, img, CV_BGRA2BGR);
//			}
//		}
//		DeleteObject(hbmp);
//	}
//}

void CPaint::draw_tip_txt(int x1, int y1, const char* txt, Scalar color, bool bottom)
{
	if (strlen(txt) == 0)
	{
		return;
	}

	Mat dst = putTextZH(_u2w(txt).c_str(), Scalar::all(43), color);

	int w = dst.cols + 32;
	int h = dst.rows + 8;
	Mat temp = Mat(Size(w, h), CV_8UC3);
	temp = Scalar::all(43);
	rectangle(temp, Point(0, 0), Point(w - 1, h - 1), Scalar::all(96), 1, LINE_8, 0);

	dst.copyTo(temp(Rect(16, 4, dst.cols, dst.rows)));

	x1 = min(x1, _win_w - temp.cols);
	if (bottom)
	{
		y1 = y1 - temp.rows - 2;
	}
	temp.copyTo(_frame(Rect(x1, y1, temp.cols, temp.rows)));

}

Mat CPaint::get_lbl(const wchar_t* txt, Scalar bkColor, Scalar color, int fontSize)
{
	if (wcslen(txt) == 0)
	{
		return Mat(32, 32, CV_8UC3);
	}
	Mat dst = putTextZH(txt, bkColor, color, fontSize);

	return dst;
}

void CPaint::draw_txt(int x1, int y1, const char* txt, Scalar color, Scalar bkColor)
{
	if (strlen(txt) == 0)
	{
		return;
	}

	Mat dst = putTextZH(_u2w(txt).c_str(), bkColor, color);

	dst.copyTo(_frame(Rect(x1, y1, dst.cols, dst.rows)));
}

int CPaint::draw_button(const char* txt, Mat& idle, Mat& over, Mat& gray, Scalar color)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}
	Mat temp1 = putTextZH(_u2w(txt).c_str(), Scalar::all(43), color);
	Mat temp2 = putTextZH(_u2w(txt).c_str(), Scalar::all(82), color);
	Mat temp3 = putTextZH(_u2w(txt).c_str(), Scalar::all(43), Scalar::all(128));

	idle = Mat(temp1.rows + 10, temp1.cols + 16, CV_8UC3);
	over = idle.clone();
	gray = idle.clone();

	idle = Scalar::all(43);
	over = Scalar::all(82);
	gray = Scalar::all(43);
	temp1.copyTo(idle(Rect(8, 5, temp1.cols, temp1.rows)));
	temp2.copyTo(over(Rect(8, 5, temp2.cols, temp2.rows)));
	temp3.copyTo(gray(Rect(8, 5, temp3.cols, temp3.rows)));

	return 0;
}

int CPaint::draw_button(const char* txt, Mat& idle, Mat& over, Scalar bkColor, Scalar color, int fontSize)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}

	Mat temp1 = putTextZH(_u2w(txt).c_str(), bkColor, color, fontSize);
	Mat temp2 = putTextZH(_u2w(txt).c_str(), Scalar::all(82), color, fontSize);

	idle = Mat(temp1.rows + 10, temp1.cols + 16, CV_8UC3);
	over = idle.clone();

	idle = bkColor;
	over = Scalar::all(82);

	temp1.copyTo(idle(Rect(8, 5, temp1.cols, temp1.rows)));
	temp2.copyTo(over(Rect(8, 5, temp2.cols, temp2.rows)));

	return 0;
}
int  CPaint::draw_ico_button(const char* txt, Mat& idle, Mat& over, Scalar bkColor, Scalar color, int fontSize)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}
	over = idle.clone();

	int rows = over.rows;
	int cols = over.cols;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Vec3b& v = over.at<Vec3b>(i, j);
			if (v[0] == 43)
			{
				over.at<Vec3b>(i, j) = Vec3b(69, 69, 69);
			}
		}
	}

	Mat temp1 = putTextZH(_u2w(txt).c_str(), bkColor, color, fontSize);
	Mat temp2 = putTextZH(_u2w(txt).c_str(), bkColor, color, fontSize);

	Mat idle2 = Mat(max(temp1.rows, idle.rows), temp1.cols + 16 + idle.cols, CV_8UC3);
	Mat over2 = idle2.clone();

	idle2 = bkColor;
	over2 = Scalar::all(82);

	int top = (idle2.rows - temp1.rows) / 2;
	idle.copyTo(idle2(Rect(0, 0, idle.cols, idle.rows)));
	temp1.copyTo(idle2(Rect(idle.cols + 8, top, temp1.cols, temp1.rows)));

	over.copyTo(over2(Rect(0, 0, over.cols, over.rows)));
	temp2.copyTo(over2(Rect(idle.cols + 8, top, temp2.cols, temp2.rows)));

	idle = idle2;
	over = over2;
	return 0;
}

int CPaint::draw_ico_button(const char* txt, Mat& btn, int rows, int off)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}
	int h = rows == 0 ? btn.rows : rows;
	int w = rows == 0 ? btn.cols : 0;

	Mat temp1 = putTextZH(_u2w(txt).c_str(), Scalar::all(43), Scalar::all(190));
	Mat idle2 = Mat(max(temp1.rows, h), temp1.cols + off * 2 + w, CV_8UC3, Scalar::all(43));

	if (rows == 0)
	{
		btn.copyTo(idle2(Rect(0, 0, btn.cols, btn.rows)));
	}

	int top = (idle2.rows - temp1.rows) / 2;
	temp1.copyTo(idle2(Rect(w + off, top, temp1.cols, temp1.rows)));

	btn = idle2;
	return 0;
}

int CPaint::draw_ico_button3(const char* txt, Mat& btn)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}
	int h = btn.rows;
	int w = btn.cols;

	Mat temp1 = putTextZH(_u2w(txt).c_str(), Scalar::all(43), Scalar::all(190));
	Mat idle2 = Mat(max(temp1.rows, h), temp1.cols + 2 * 2 + w, CV_8UC3, Scalar::all(43));

	Rect rc(4, 0, btn.cols - 4, btn.rows);
	btn(rc).copyTo(idle2(Rect(0, 0, btn.cols - 4, btn.rows)));

	int top = (idle2.rows - temp1.rows) / 2;
	temp1.copyTo(idle2(Rect(w + 2, top, temp1.cols, temp1.rows)));

	btn = idle2;
	return 0;
}

int CPaint::draw_ico_button(Mat temp, Mat& idle, Mat& over)
{
	Mat idle2 = temp.clone();
	Mat over2 = temp.clone();

	idle2 = Scalar::all(43);
	over2 = Scalar::all(43);

	over = idle.clone();
	int rows = over.rows;
	int cols = over.cols;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Vec3b& v = over.at<Vec3b>(i, j);
			if (v[0] == 43)
			{
				over.at<Vec3b>(i, j) = Vec3b(69, 69, 69);
			}
		}
	}

	idle.copyTo(idle2(Rect(0, 0, idle.cols, idle.rows)));
	over.copyTo(over2(Rect(0, 0, over.cols, over.rows)));

	idle = idle2;
	over = over2;
	return 0;
}

int CPaint::draw_ico_button(const char* txt, Mat& idle, Scalar bk, Scalar color)
{
	if (strlen(txt) == 0)
	{
		return 1;
	}

	Mat temp1 = putTextZH(_u2w(txt).c_str(), bk, color);
	Mat idle2 = Mat(max(temp1.rows, idle.rows), temp1.cols + 16 + idle.cols, CV_8UC3, bk);

	int top = (idle2.rows - temp1.rows) / 2;
	idle.copyTo(idle2(Rect(0, 0, idle.cols, idle.rows)));
	temp1.copyTo(idle2(Rect(idle.cols + 8, top, temp1.cols, temp1.rows)));

	idle = idle2;
	return 0;
}

Mat CPaint::putTextZH(const wchar_t* str, Scalar bkcolor, Scalar color, int fontSize, const wchar_t* fn)
{
	Scalar_<uchar> bg = bkcolor;
	Scalar_<uchar> ii = color;

	LOGFONT lf;
	//lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(_hDC, LOGPIXELSY), 72);
	lf.lfHeight = -MulDiv(fontSize, _pixelsy, 72);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_LIGHT; //normal 400
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	wcscpy_s(lf.lfFaceName, _fn);

	HFONT hf = CreateFontIndirect(&lf);
	HFONT hOldFont = (HFONT)SelectObject(_hDC, hf);

	Mat dst(32, 32, CV_8UC3);

	bool isok = false;
	SIZE size;
	BOOL b = GetTextExtentPoint32(_hDC, str, (int)wcslen(str), &size);
	if (b)
	{
		int z = size.cx;
		div_t di = div(z, 4);
		if (di.rem != 0)
		{
			size.cx = di.quot * 4 + 4;
		}

		dst = Mat(Size(size.cx, size.cy), CV_8UC3, bkcolor);

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
		HBITMAP hBmp = CreateDIBSection(_hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

		if (hBmp != 0)
		{
			HBITMAP hOldBmp = (HBITMAP)SelectObject(_hDC, hBmp);

			RECT rt;
			rt.left = 0;
			rt.top = 0;
			rt.right = dst.cols;
			rt.bottom = dst.rows;

			HBRUSH hbrush = CreateSolidBrush(RGB(bg[2], bg[1], bg[0]));
			FillRect(_hDC, &rt, hbrush);

			if (hbrush != NULL)
			{
				DeleteObject(hbrush);
			}

			SetTextColor(_hDC, RGB(ii[2], ii[1], ii[0]));
			SetBkColor(_hDC, RGB(bg[2], bg[1], bg[0]));

			TextOut(_hDC, 0, 0, str, (int)wcslen(str));

			GetBitmapBits(hBmp, bih.biWidth * bih.biHeight * 3, dst.data);

			SelectObject(_hDC, hOldBmp);
			SelectObject(_hDC, hOldFont);
			DeleteObject(hf);
			DeleteObject(hBmp);

			for (int i = 0; i < dst.rows; i++)
			{
				for (int j = 0; j < dst.cols; j++)
				{
					Vec3b& v = dst.at<Vec3b>(i, j);
					if (v[0] == 0 && v[1] == 0 && v[2] == 0)
					{
						v[0] = bg[0];
						v[1] = bg[1];
						v[2] = bg[2];
					}
				}
			}

			if (dst.rows > 24)
			{
				resize(dst, dst, Size(dst.cols, 22), 0, 0);
			}
			isok = true;
		}
	}

	if (!isok)
	{
		return Mat(32, 32, CV_8UC3);
	}

	return dst;
}

void CPaint::make_draw_ico(Mat& ico, Scalar color)
{
	Scalar_<uchar> ii = color;
	for (int i = 0; i < ico.rows; i++)
	{
		for (int j = 0; j < ico.cols; j++)
		{
			Vec3b& v = ico.at<Vec3b>(i, j);
			if ((v[0] != 43 && v[1] != 43 && v[2] != 43))
			{
				v[0] = ii[0];
				v[1] = ii[1];
				v[2] = ii[2];
			}
		}
	}
}

void CPaint::make_draw_ico(Mat& ico, int c, Scalar color)
{
	Scalar_<uchar> ii = color;
	for (int i = 0; i < ico.rows; i++)
	{
		for (int j = 0; j < ico.cols; j++)
		{
			Vec3b& v = ico.at<Vec3b>(i, j);
			if (v[0] == c && v[1] == c && v[2] == c)
			{
				v[0] = ii[0];
				v[1] = ii[1];
				v[2] = ii[2];
			}
		}
	}
}

void CPaint::praser_file(wstring szline)
{
	vector<wstring> v1 = tokenize(szline, L"|", false, L"");
	int len = (int)v1.size();
	if (len == 5)
	{
		_curr_nav_dto.id = v1[0];
		_curr_nav_dto.file_name = v1[1];
		_curr_nav_dto.ori_name = v1[2];
		_curr_nav_dto.file_time = v1[3];
		_curr_nav_dto.rotation = v1[4];
	}
}

void CPaint::make_bgra_bg(Mat src, Mat& alpha)
{
	alpha = Mat(src.size(), CV_8UC3, Scalar::all(204));

	div_t row = div(alpha.rows, 8);
	div_t col = div(alpha.cols, 8);

	for (int i = 0; i < col.quot + 1; i++)
	{
		for (int j = 0; j < row.quot + 1; j++)
		{
			bool b = false;
			Point pt0;
			Point pt1;

			if ((i % 2 == 1 && j % 2 == 0) || (i % 2 == 0 && j % 2 == 1))
			{
				if (i == col.quot && j == row.quot)
				{
					b = true;
					pt0 = Point(i * 8, j * 8);
					pt1 = Point(i * 8 + col.rem, j * 8 + row.rem);
				}
				else if (i == col.quot)
				{
					b = true;
					pt0 = Point(i * 8, j * 8);
					pt1 = Point(i * 8 + col.rem, j * 8 + 8);
				}
				else if (j == row.quot)
				{
					b = true;
					pt0 = Point(i * 8, j * 8);
					pt1 = Point(i * 8 + 8, j * 8 + row.rem);
				}
				else
				{
					b = true;
					pt0 = Point(i * 8, j * 8);
					pt1 = Point(i * 8 + 8, j * 8 + 8);
				}
			}

			if (b)
			{
				rectangle(alpha, pt0, pt1, Scalar::all(255), -1, LINE_8, 0);
			}
		}
	}
}

void CPaint::WritePrivate(string key, int val)
{
	CSimpleIniA ini;
	ini.SetUnicode(true);

	string alb = "private";
	if (PRODUCT_NAME == L"PhotoNest")
	{
		alb = "public";
	}

	wstring iniPath = get_appdata_path(alb);
	iniPath += L"\\prof.ini";

	ini.LoadFile(iniPath.c_str());
	ini.SetLongValue("section", key.c_str(), val);
	ini.SaveFile(iniPath.c_str());
}

void CPaint::WritePrivate(string key, const char* val)
{
	CSimpleIniA ini;
	ini.SetUnicode(true);

	string alb = "private";
	if (PRODUCT_NAME == L"PhotoNest")
	{
		alb = "public";
	}
	wstring iniPath = get_appdata_path(alb);
	iniPath += L"\\prof.ini";

	ini.LoadFile(iniPath.c_str());
	ini.SetValue("section", key.c_str(), val);
	ini.SaveFile(iniPath.c_str());
}

void CPaint::ReadPrivate(string key, string& val)
{
	val = "";

	CSimpleIniA ini;
	ini.SetUnicode(true);

	string alb = "private";
	if (PRODUCT_NAME == L"PhotoNest")
	{
		alb = "public";
	}
	wstring iniPath = get_appdata_path(alb);
	iniPath += L"\\prof.ini";

	ini.LoadFile(iniPath.c_str());

	const char* pv1 = ini.GetValue("section", key.c_str());
	if (pv1 != nullptr)
	{
		val = pv1;
	}
}

void CPaint::make_over(Mat src, Mat& over, int src_color, int over_color)
{
	over = src.clone();

	int	BChannel = 0;
	int	GChannel = 0;
	int	RChannel = 0;

	int rows = over.rows;
	int cols = over.cols;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Vec3b& v = over.at<Vec3b>(i, j);
			if (v[0] == src_color && v[1] == src_color && v[2] == src_color)
			{
				over.at<Vec3b>(i, j) = Vec3b(over_color, over_color, over_color);
			}
		}
	}
}

void CPaint::make_gray(Mat src, Mat& over, Mat& gray)
{
	over = src.clone();
	gray = src.clone();

	double alphaReserve = 0.5;
	int	BChannel = 0;
	int	GChannel = 0;
	int	RChannel = 0;

	int rows = src.rows;
	int cols = src.cols;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			Vec3b& v = gray.at<Vec3b>(i, j);
			if (v[0] == 43 && v[1] == 43 && v[2] == 43)
			{
				over.at<Vec3b>(i, j) = Vec3b(96, 96, 96);
			}
			else
			{
				if (v[1] == 255)
				{
					alphaReserve = 0.4;
				}
				else
				{
					alphaReserve = 0.5;
				}
				alpha(v, BChannel, GChannel, RChannel, alphaReserve);
			}
		}
	}
}

void CPaint::make_gray(const char* txt, Mat& src, Mat& over, Mat& gray)
{
	make_gray(src, over, gray);

	draw_ico_button(txt, src, Scalar::all(43), Scalar(190, 190, 190));
	draw_ico_button(txt, over, Scalar::all(82), Scalar(190, 190, 190));
	draw_ico_button(txt, gray, Scalar::all(43), Scalar(96, 96, 96));
}

void CPaint::drawWeighted(Mat& weighted, Mat beforeROI, Mat afterROI, int BChannel, int GChannel, int RChannel)
{
	double alphaReserve = 0.6;
	//int	BChannel = 0;
	//int	GChannel = 0;
	//int	RChannel = 255;

	for (int i = 0; i < beforeROI.rows; i++)
	{
		uchar* p = beforeROI.ptr<uchar>(i);
		uchar* p1 = afterROI.ptr<uchar>(i);
		Vec3b* ptr = weighted.ptr<Vec3b>(i);
		for (int j = 0; j < beforeROI.cols; j++)
		{
			if (p1[j] == 255 && p[j] != 255)
			{
				alpha(ptr[j], BChannel, GChannel, RChannel, alphaReserve);
			}
		}
	}
}

void CPaint::drawWeighted_picker(Mat& weighted, Rect rect, Mat afterROI, Scalar color)
{
	Scalar_<uchar> ii = color;
	Vec3b val = Vec3b(ii[0], ii[1], ii[2]);

	for (int i = 0; i < afterROI.rows; i++)
	{
		uchar* p1 = afterROI.ptr<uchar>(i);
		for (int j = 0; j < afterROI.cols; j++)
		{
			if (p1[j] == 255)
			{
				weighted.at<Vec3b>(rect.y + i, rect.x + j) = val;
				if (!_eraser_mask.empty())
				{
					_eraser_mask.at<uchar>(rect.y + i, rect.x + j) = 255;
				}
			}
		}
	}
}

void CPaint::drawWeighted_seg(Mat& weighted, Mat mask)
{
	Rect rc = _rect_seg_select;

	if (rc.width > 0)
	{
		Point pt0(rc.x, rc.y);
		Point pt(pt0.x + rc.width, pt0.y + rc.height);
		rectangle(weighted, pt0, pt, Scalar(0, 255, 0), THICKNESS_VAL, LINE_8, 0);

		for (int i = rc.y; i < rc.y + rc.height; i++)
		{
			uchar* p1 = mask.ptr<uchar>(i);
			for (int j = rc.x; j < rc.x + rc.width; j++)
			{
				if (p1[j] == 1)
				{
					weighted.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
				}
				else if (p1[j] == 0)
				{
					weighted.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
				}
			}
		}
	}
}

void CPaint::make_gray2(Mat& src)
{
	double alphaReserve = 0.5;
	int	BChannel = 64;
	int	GChannel = 64;
	int	RChannel = 64;

	for (int i = 0; i < src.rows; i++)
	{
		Vec3b* ptr = src.ptr<Vec3b>(i);
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b& v = ptr[j];
			if (v[0] != 43 && v[1] != 43 && v[2] != 43)
			{
				alpha(v, BChannel, GChannel, RChannel, alphaReserve);
			}
		}
	}
}

void CPaint::save_to_file(Mat dst, wstring szDst2)
{
	vector<int> ql;
	ql.push_back(IMWRITE_PNG_COMPRESSION);
	ql.push_back(3);

	vector<uchar> buf;
	imencode(".png", dst, buf, ql);

	ofstream file2(szDst2.c_str(), ios::out | ios::binary);
	if (!file2)
	{
		return;
	}
	file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
	file2.close();
}

long CPaint::GetFolderPics(wstring strPath, list<wstring >& lstFile)
{
	wstring curr_path = L"";

	wchar_t buf[MAX_PATH] = { 0 };
	wcscpy_s(buf, MAX_PATH, strPath.c_str());

	wchar_t* p = wcsrchr(buf, L'/');
	if (p != NULL)
	{
		p[0] = 0;
		curr_path = buf;
	}

	curr_path.erase(curr_path.find_last_not_of(L"/") + 1);

	wchar_t* buf1 = (wchar_t*)malloc(1024 * sizeof(wchar_t));
	if (buf1 != NULL)
	{
		memset(buf1, 0, 1024 * sizeof(wchar_t));
		swprintf_s(buf1, 1024, L"%s/*", curr_path.c_str());
	}

	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFile(buf1, &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0)
		{
			if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
			{
			}
			else
			{
				bool b = is_picture(ffd.cFileName);
				if (b)
				{
					wstring fullname = curr_path + L"/" + ffd.cFileName;
					lstFile.push_back(fullname);
				}
			}
		}

		while (FindNextFileW(hFind, &ffd))
		{
			if (wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0)
			{
				if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || (ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				{
				}
				else
				{
					bool b = is_picture(ffd.cFileName);
					if (b) {
						wstring fullname = curr_path + L"/" + ffd.cFileName;
						lstFile.push_back(fullname);
					}
				}
			}
		}
		FindClose(hFind);
	}

	free(buf1);
	return 0;
}

bool CPaint::is_picture(wchar_t* cFileName)
{
	wstring curr_name = L"";
	wchar_t* p = wcsrchr(cFileName, L'.');
	if (p != NULL)
	{
		curr_name = p;
	}

	transform(curr_name.begin(), curr_name.end(), curr_name.begin(), ::tolower);

	if (curr_name == L".jpg" || curr_name == L".jpeg" || curr_name == L".png" || curr_name == L".webp" || curr_name == L".bmp" || curr_name == L".dib")
	{
		return true;
	}

	return false;
}

long CPaint::read_svg(HINSTANCE hrcInst, UINT cr_id, string& xml)
{
	xml = "";

	HRSRC hrs = FindResource(hrcInst, MAKEINTRESOURCE(cr_id), RT_HTML);
	if (hrs != NULL)
	{
		DWORD dw = SizeofResource(hrcInst, hrs);
		HGLOBAL hg = LoadResource(hrcInst, hrs);
		if (hg != NULL)
		{
			void* p = LockResource(hg);
			if (p != NULL)
			{
				xml = string((LPSTR)p, dw);
			}
			FreeResource(hg);
		}
	}
	return 0;
}

void CPaint::set_svg(int w, int h, UINT cr_id, Mat& dst, int w0, int h0, Scalar color)
{
	dst = Mat(Size(w0, h0), CV_8UC3, color);

	string xml = "";
	read_svg(_his, cr_id, xml);
	if (!xml.empty())
	{
		uint32_t width = w, height = h;
		uint32_t bgColor = 0x2B2B2BFF;
		if (color == Scalar::all(218))
		{
			bgColor = 0xDADADAFF;
		}

		auto document = lunasvg::Document::loadFromData(xml);
		if (!document)
		{
			return;
		}

		auto bitmap = document->renderToBitmap(width, height, bgColor);
		if (!bitmap.valid())
		{
			return;
		}

		width = bitmap.width();
		height = bitmap.height();

		auto stride = bitmap.stride();
		auto rowData = bitmap.data();

		int x0 = (w0 - width) / 2;
		int y0 = (h0 - height) / 2;

		Mat roi = dst(Rect(x0, y0, width, height));

		for (uint32_t y = 0; y < height; y++)
		{
			auto data = rowData;
			for (uint32_t x = 0; x < width; x++)
			{
				//auto b = data[0];
				//auto g = data[1];
				//auto r = data[2];
				//auto a = data[3];

				Vec3b& v = roi.at<Vec3b>(y, x);

				//if (color == Scalar::all(210) && data[0] != 0 && data[0] != 0x2B)
				//{
				//	v[0] = 0xff;
				//	v[1] = 0xff;
				//	v[2] = 0xff;
				//}
				//else
				{
					v[0] = data[0];
					v[1] = data[1];
					v[2] = data[2];
				}
				data += 4;
			}
			rowData += stride;
		}
	}
}

int CPaint::get_thread_idx()
{
	for (int i = 0; i < 100; i++)
	{
		if (threads[i].step == 0)
		{
			return i;
		}
	}
	return 0;
}

t_struct* CPaint::get_thread()
{
	t_struct* ptrT = &threads[0];
	for (int i = 0; i < 100; i++)
	{
		if (threads[i].hdl == _handle_thread)
		{
			ptrT = &threads[i];
			break;
		}
	}
	return ptrT;
}

void CPaint::set_dst_by_v3(Mat& dst, Mat src)
{
	for (int i = 0; i < src.rows; i++)
	{
		Vec4b* p1 = src.ptr<Vec4b>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (p1[j][3] != 0)
			{
				Vec3b& v1 = dst.at<Vec3b>(i, j);
				v1[0] = p1[j][0];
				v1[1] = p1[j][1];
				v1[2] = p1[j][2];
			}
		}
	}
}

void CPaint::set_mask_by_v1(Mat& mask, Mat src)
{
	for (int i = 0; i < src.rows; i++)
	{
		uchar* p1 = src.ptr<uchar>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (p1[j] != 0)
			{
				mask.at<uchar>(i, j) = 255;
			}
		}
	}
}

bool CPaint::set_mask_by_v3(Mat& mask, Mat src)
{
	bool b = false;
	for (int i = 0; i < src.rows; i++)
	{
		Vec4b* p1 = src.ptr<Vec4b>(i);
		for (int j = 0; j < src.cols; j++)
		{
			if (p1[j][3] != 0)
			{
				mask.at<uchar>(i, j) = 255;
				b = true;
			}
		}
	}
	return b;
}

Rect CPaint::get_rc255(Mat mask)
{
	int x0 = mask.cols;
	int y0 = mask.rows;
	int x1 = 0;
	int y1 = 0;

	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			if (mask.at<uchar>(i, j) == 255)
			{
				if (y0 > i)
				{
					y0 = i;
				}
				if (y1 < i)
				{
					y1 = i;
				}

				if (x0 > j)
				{
					x0 = j;
				}
				if (x1 < j)
				{
					x1 = j;
				}
			}
		}
	}

	Rect rc(x0, y0, x1 - x0, y1 - y0);
	if (rc.width <= 0 || rc.height <= 0)
	{
		rc = Rect(0, 0, mask.cols, mask.rows);
	}

	return rc;
}

int CPaint::resizemode()
{
	//INTER_LINEAR;//
	return INTER_LINEAR;//(_lena_w > _dst_zoom_w) ? INTER_AREA : INTER_LINEAR;
}

void CPaint::int_xy(Rect &sr)
{
	sr.x = (int)(sr.x / _zoom_r);
	sr.y = (int)(sr.y / _zoom_r);

	sr.x = (int)(sr.x * _zoom_r);
	sr.y = (int)(sr.y * _zoom_r);
}
