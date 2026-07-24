///////////////////////////////////////////////////////////////////////
/// @file paint_clip.cpp
/// @brief 绘图模块 - 剪贴板操作实现
/// @details 实现图像剪贴板操作,包括 Bitmap 信息填充、
///          Mat 到 Bitmap 转换、剪贴板读写等功能
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理 Windows GDI 位图格式与 OpenCV Mat 的转换
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include<fstream>

void CPaint::FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);

	bmih->biWidth = width;
	div_t di = div(width, 4);
	if (di.rem != 0)
	{
		bmih->biWidth = di.quot * 4 + 4;
	}
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;
}

HBITMAP CPaint::mat2bitmap(const cv::Mat imgTmp, HDC hDC)
{
	uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	int bmp_w = imgTmp.cols, bmp_h = imgTmp.rows;
	int nchannels = imgTmp.channels();
	int bpp = (imgTmp.depth() + 1) * nchannels;
	FillBitmapInfo(bmi, bmp_w, bmp_h, bpp * 8, 0);

	char* pBits = NULL;
	HBITMAP hBitmap = CreateDIBSection(hDC, bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	if (hBitmap != 0)
	{
		int pixelBytes = imgTmp.channels() * (imgTmp.depth() + 1); // 计算一个像素多少个字节

		div_t di = div(bmp_w, 4);
		if (di.rem == 0)
		{
			memcpy(pBits, imgTmp.data, pixelBytes * bmp_w * bmp_h);
		}
		else
		{
			// 原始图像宽度不是 4 的倍数&#xff0c;将实际图片数据一行一行拷贝过去 
			int ntempW = di.quot * 4 + 4;
			for (int i = 0; i < bmp_h; i++)
			{
				memcpy(pBits + pixelBytes * ntempW * i, imgTmp.data + pixelBytes * bmp_w * i, pixelBytes * bmp_w);
			}
		}
	}

	return hBitmap;
}

bool CPaint::copy_clipboard(Rect& rc)
{
	if (_lena2.empty())
	{
		return false;
	}

	if (_rect_select.width == 0)
	{
		return false;
	}

	rc = _rect_select;
	Rect sr = _rect_select;
	dst_rc_2_lena_rc(sr, rc);

	if (rc.x < 0 || rc.y < 0 || rc.width <= 0 || rc.height <= 0 ||
		rc.x + rc.width > _weightedImage.cols || rc.y + rc.height > _weightedImage.rows)
	{
		return false;
	}

	HWND hWnd = _win_handle;
	if (!::OpenClipboard(hWnd))
	{
		return false;
	}

	bool b = true;

	HBITMAP srcBmp = NULL;
	HBITMAP dstBmp = NULL;

	HDC hDC = ::GetDC(hWnd);
	HDC srcDC = CreateCompatibleDC(hDC);
	HDC dstDC = CreateCompatibleDC(hDC);

	Mat dst;

	if (!_eraser_mask.empty())
	{
		Mat _weightedImage4;
		cv::cvtColor(_weightedImage, _weightedImage4, CV_BGR2BGRA);
		vector<int> fromto = { 0, 3 };
		mixChannels(_eraser_mask, _weightedImage4, fromto);
		dst = _weightedImage4(rc).clone();
	}
	else
	{
		dst = _weightedImage(rc).clone();
	}

	int w = dst.cols;
	int h = dst.rows;

	do
	{
		if (!::EmptyClipboard())
		{
			b = false;
			break;
		}

		srcBmp = mat2bitmap(dst, hDC);
		if (srcBmp != 0)
		{
			SelectObject(srcDC, srcBmp);

			dstBmp = CreateCompatibleBitmap(hDC, w, h);
			SelectObject(dstDC, dstBmp);

			if (!::BitBlt(dstDC, 0, 0, w, h, srcDC, 0, 0, SRCCOPY))
			{
				b = false;
				break;
			}

			::SetClipboardData(CF_BITMAP, dstBmp);
		}
		else
		{
			b = false;
		}

	} while (0);

	DeleteDC(hDC);
	DeleteDC(srcDC);
	DeleteDC(dstDC);

	if (srcBmp) ::DeleteObject(srcBmp);
	if (dstBmp) ::DeleteObject(dstBmp);

	::CloseClipboard();

	return b;
}

void CPaint::paste_clipboard()
{
	if (!_clip_mat.empty())
	{
		paste_ok();
	}

	HWND hWnd = _win_handle;

	::OpenClipboard(hWnd);
	if (::IsClipboardFormatAvailable(CF_BITMAP))
	{
		HANDLE hBitmap = ::GetClipboardData(CF_BITMAP);
		BITMAP bm;
		GetObject(hBitmap, sizeof(BITMAP), &bm);

		int channels = bm.bmBitsPixel == 1 ? 1 : bm.bmBitsPixel / 8;

		_clip_mat.create(cvSize(bm.bmWidth, bm.bmHeight), CV_MAKETYPE(CV_8U, channels));
		GetBitmapBits((HBITMAP)hBitmap, bm.bmWidth * bm.bmHeight * channels, _clip_mat.data);

		if (channels == 4)
		{
			HWND hwd = ::GetClipboardOwner();
			//gallery error
			if (IsWindow(_hParent) && _hParent != hwd)
			{
				cvtColor(_clip_mat, _clip_mat, CV_BGRA2BGR);
			}
			else if (hWnd != hwd)
			{
				cvtColor(_clip_mat, _clip_mat, CV_BGRA2BGR);
			}
			else
			{
				Mat tmp = Mat::zeros(_clip_mat.size(), CV_8UC1);
				vector<int> fromto = { 3, 0 };
				mixChannels(_clip_mat, tmp, fromto);
				int z = countNonZero(tmp);
				if (z == 0)
				{
					cvtColor(_clip_mat, _clip_mat, CV_BGRA2BGR);
				}
			}
		}

		channels = _clip_mat.channels();

		if (_lena2.empty())
		{
			string alb = "private";
			if (PRODUCT_NAME == L"PhotoNest")
			{
				alb = "public";
			}
			_curr_nav_dto.file_name = get_appdata_path(alb);
			_curr_nav_dto.file_name += L"\\temp.png";

			vector<int> ql;
			ql.push_back(IMWRITE_PNG_COMPRESSION);
			ql.push_back(3);

			vector<uchar> buf;
			cv::imencode(".png", _clip_mat, buf, ql);

			ofstream file2(_curr_nav_dto.file_name.c_str(), ios::out | ios::binary);
			if (!file2)
			{
				return;
			}
			file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
			file2.close();

			opt_t op;
			op.opt_type = btn_init;
			op.nav_type = 0;
			op.thickness = THICKNESS_VAL;
			op.mask = _clip_mat.clone();

			load_image("first", _curr_nav_dto.file_name.c_str(), false);

			//import
			add_opt(op, false);
		}
		else if (_clip_mat.cols > _lena2.cols && _clip_mat.rows > _lena2.rows)
		{
			if (channels == 4)
			{
				_bgra_mask = Mat::zeros(_clip_mat.size(), CV_8UC1);

				for (int i = 0; i < _clip_mat.rows; i++)
				{
					Vec4b* ptr = _clip_mat.ptr<Vec4b>(i);
					for (int j = 0; j < _clip_mat.cols; j++)
					{
						Vec4b& v = ptr[j];
						if (v[3] == 0)
						{
							v[0] = 255;
							v[1] = 255;
							v[2] = 255;
						}
					}
				}

				vector<int> fromto = { 3, 0 };
				mixChannels(_clip_mat, _bgra_mask, fromto);

				cv::cvtColor(_clip_mat, _clip_mat, CV_BGRA2BGR);
				_eraser_mask = _bgra_mask.clone();
			}
			else
			{
				_bgra_mask = Mat::zeros(Size(0, 0), CV_8UC1);
				_eraser_mask = Mat::zeros(Size(0, 0), CV_8UC1);
			}



			opt_t op;
			op.opt_type = btn_pasteas;
			op.nav_type = 0;
			op.image = _clip_mat.clone();
			op.mask = _eraser_mask.clone();
			op.thickness = 0;

			_weightedImage = _clip_mat.clone();
			_lena_bak = _weightedImage.clone();
			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);

			_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
			_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);
			_rect_seg_select = Rect(0, 0, 0, 0);

			_weighted_befoe_input = Mat::zeros(Size(0, 0), CV_8UC3);
			_result = Mat::zeros(Size(0, 0), CV_8UC1);
			_eraser_mask_tmp = Mat::zeros(Size(0, 0), CV_8UC3);
			_curr_rt = Rect(0, 0, 0, 0);


			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;
			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);

			add_opt(op, false);

			_clip_mat = Mat::zeros(Size(0, 0), CV_8UC3);
		}
		else
		{
			if (_rect_paste.width == 0 || _rect_paste.height == 0)
			{
				int lf = 0;
				if (_dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
				{
					lf = TOOLBAR_LEFT_WIDTH;
				}
				_rect_paste = Rect(lf, 0, _clip_mat.cols * _dst_zoom_w / _lena_w, _clip_mat.rows * _dst_zoom_w / _lena_w);
			}

			if (!_eraser_mask.empty())
			{
				_eraser_mask_bak = _eraser_mask.clone();
			}
			_weightedImage = _lena_bak.clone();
			paste_ok(_weightedImage);

			opt_t op;
			op.opt_type = btn_paste;
			op.nav_type = 0;
			op.thickness = THICKNESS_VAL;
			op.image = _clip_mat.clone();
			add_opt(op);
		}
		//save_to_file(_clip_mat, L"d:\\test\\a.png");
	}
	::CloseClipboard();

	//if (IsClipboardFormatAvailable(CF_TEXT))//判断剪切板上的数据格式
	//{
	//	HANDLE hclip;
	//	char *buf;
	//	hclip = GetClipboardData(CF_TEXT);
	//	buf = (char*)GlobalLock(hclip);
	//	GlobalUnlock(hclip);
	//}
}

void CPaint::paste_ok(Mat& weighted)
{
	if (!_clip_mat.empty())
	{
		_mask_idx = -997;
		Rect target_rc, rc, sr;
		sr = _rect_paste;

		//int_xy(sr);

		rc = Rect(0, 0, sr.width, sr.height);
		proc_clip_rc(rc, sr, _dst);
		target_rc = rc;

		target_rc.x = _dst_in_lena.x + (int)(sr.x * _lena_w / _dst_zoom_w + .5) + rc.x;
		target_rc.y = _dst_in_lena.y + (int)(sr.y * _lena_w / _dst_zoom_w + .5) + rc.y;

		if (rc.width <= 0 || rc.height <= 0 || rc.width > _clip_mat.cols || rc.height > _clip_mat.rows)
		{
			_clip_mat = Mat::zeros(Size(0, 0), CV_8UC3);

			_rect_paste = Rect(0, 0, 0, 0);
			_pt_prev = Point(0, 0);
			_pt_down = Point(0, 0);

			_working = act_display;
			return;
		}

		Mat clip = _clip_mat(rc).clone();
		int channels = _clip_mat.channels();
		if (channels == 4)
		{
			Mat d2 = weighted(target_rc);
			set_dst_by_v3(d2, clip);
		}
		else
		{
			clip.copyTo(weighted(target_rc));
		}

		if (!_eraser_mask.empty())
		{
			_eraser_mask = _eraser_mask_bak.clone();
			Mat roi = _eraser_mask(target_rc);
			if (channels == 4)
			{
				set_mask_by_v3(roi, clip);
			}
			else
			{
				roi.setTo(255);
			}
		}

		_pointer_btn = 0;
	}
}

void CPaint::paste_ok()
{
	EnterCriticalSection(&_cs);
	{
		int opt_type = 0;
		int thickness = 0;
		int len0 = (int)_opt_arr.size();
		for (int i = 0; i < len0; i++)
		{
			opt_type = _opt_arr[i].opt_type;
			thickness = _opt_arr[i].thickness;
		}
		if (opt_type == btn_paste && thickness == 3)
		{
			_opt_arr.erase(end(_opt_arr) - 1);
		}
	}
	LeaveCriticalSection(&_cs);

	_clip_mat = Mat::zeros(Size(0, 0), CV_8UC3);

	_rect_paste = Rect(0, 0, 0, 0);
	_pt_prev = Point(0, 0);
	_pt_down = Point(0, 0);

	opt_t op;
	op.opt_type = btn_paste;
	op.nav_type = 0;
	op.thickness = 0;
	op.mask = _eraser_mask.clone();
	op.image = _weightedImage.clone();
	add_opt(op);

	_working = act_display;

	_lena_bak = _weightedImage.clone();

	if (_opt == btn_draw_select)
	{
		_pointer_btn = 0;
	}
	else
	{
		_pointer_btn = btn_move;
	}
}

void CPaint::proc_clip_rc(Rect& rc, Rect sr, Mat dst)
{
	if (sr.x < 0)
	{
		rc.x = abs(sr.x);
		rc.width = rc.width - rc.x;
	}

	if (sr.x + sr.width > dst.cols)
	{
		if (sr.x < 0)
		{
			rc.x = abs(sr.x);
			rc.width = dst.cols;
		}
		else
		{
			rc.x = 0;
			rc.width = dst.cols - sr.x;
		}
	}

	if (sr.y < 0)
	{
		rc.y = abs(sr.y);
		rc.height = rc.height - rc.y;
	}

	if (sr.y + sr.height > dst.rows)
	{
		if (sr.y < 0)
		{
			rc.height = dst.rows;
			rc.y = abs(sr.y);
		}
		else
		{
			rc.height = dst.rows - abs(sr.y);
			rc.y = 0;
		}
	}

	rc.x = rc.x * _lena_w / _dst_zoom_w;
	rc.y = rc.y * _lena_w / _dst_zoom_w;
	rc.width = rc.width * _lena_w / _dst_zoom_w;
	rc.height = rc.height * _lena_w / _dst_zoom_w;

	if (rc.x + rc.width > _clip_mat.cols)
	{
		rc.width = _clip_mat.cols - rc.x;
	}
	if (rc.y + rc.height > _clip_mat.rows)
	{
		rc.height = _clip_mat.rows - rc.y;
	}
}
