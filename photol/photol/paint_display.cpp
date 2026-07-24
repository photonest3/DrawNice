///////////////////////////////////////////////////////////////////////
/// @file paint_display.cpp
/// @brief 绘图模块 - 显示功能实现
/// @details 实现图像显示功能,包括缩放显示、窗口适配、坐标转换等
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理图像在 GUI 窗口中的显示逻辑
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include <shellapi.h>
#include "resource.h"

bool CPaint::get_display_win(Point pt0)
{
	int x = 0;
	int y = 0;

	int x0 = pt0.x;
	int y0 = pt0.y;

	int roiw = _dst_zoom_w;
	int roih = _dst_zoom_h;

	int offx = 0;//TOOLBAR_LEFT_WIDTH;
	int offy = 0;//TOOLBAR_TOP_HEIGHT;
	if (pt0.x < 0)
	{
		x = abs(pt0.x) + offx;
		x0 = offx;
		roiw = _dst_zoom_w - x;
		if (roiw > _win_w)
		{
			roiw = _win_w - offx;
		}
	}
	else
	{
		if (pt0.x + _dst_zoom_w > _win_w)
		{
			x = 0;
			roiw = _win_w - pt0.x;
			x0 = _win_w - roiw;
		}
	}

	if (pt0.y < 0)
	{
		y = abs(pt0.y) + offy;
		y0 = offy;
		roih = _dst_zoom_h - y;
		if (roih > _win_h)
		{
			roih = _win_h - offy;
		}
	}
	else
	{
		if (pt0.y + _dst_zoom_h > _win_h)
		{
			y = 0;
			roih = _win_h - pt0.y;
			y0 = _win_h - roih;
		}
	}

	Rect rc = Rect(x, y, roiw, roih);

	int x2 = (int)((double)rc.x / _zoom_r);
	int w0 = (int)((double)rc.width / _zoom_r);
	int y2 = (int)((double)rc.y / _zoom_r);
	int h0 = (int)((double)rc.height / _zoom_r);

	if (x2 + w0 > _weightedImage.cols)
	{
		w0 = _weightedImage.cols - x2;
		//WritePrivate("high_displayImage error1", "x2 + w0 > _weightedImage.cols");
	}
	if (y2 + h0 > _weightedImage.rows)
	{
		//WritePrivate("high_displayImage error21", y2);
		//WritePrivate("high_displayImage error22", h0);
		//WritePrivate("high_displayImage error23", _weightedImage.rows);
		h0 = _weightedImage.rows - y2;
	}

	if (w0 <= 0 || h0 <= 0)
	{
		//WritePrivate("high_displayImage error", "w0=0");
		return false;
	}

	_dst_in_lena = Rect(x2, y2, w0, h0);
	_dst_in_frame = Point(x0, y0);
	return true;
}

void CPaint::high_displayImage()
{
	if (_lena2.empty())
	{
		reset_frame();
		return;
	}
	if (_dst_zoom_w == 0)
	{
		return;
	}

	Mat dst;

	bool b = get_display_win(_lena_in_frame);
	if (!b)
	{
		_lena_in_frame.x = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
		_lena_in_frame.y = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

		b = get_display_win(_lena_in_frame);
	}

	if (!b)
	{
		_lena_in_frame.x = 100;
		_lena_in_frame.y = 100;
		b = get_display_win(_lena_in_frame);
	}

	if (!b)
	{
		WritePrivate("high_displayImage error", "w0=0");
		reset_frame();
		return;
	}

	dst = _weightedImage(_dst_in_lena).clone();

	set_opt_arr();

	Size sz = dst.size();
	sz.width = min(sz.width * _dst_zoom_w / _lena_w, _frame.cols - _dst_in_frame.x);
	sz.height = min(sz.height * _dst_zoom_w / _lena_w, _frame.rows - _dst_in_frame.y);

	resize(dst, dst, sz, 0, 0, resizemode());

	if (!_eraser_mask.empty())
	{
		if (_eraser_mask_tmp.empty())
		{
			_eraser_mask_tmp = _eraser_mask.clone();
		}

		if (_idx != _mask_idx)
		{
			_eraser_mask_tmp = _eraser_mask.clone();
			int len3 = countNonZero(_inpaintMask);
			if (len3 > 0)
			{
				set_mask_by_v1(_eraser_mask_tmp, _inpaintMask);
			}
			_mask_idx = _idx;
		}

		Mat tm = _eraser_mask_tmp(_dst_in_lena);
		resize(tm, tm, dst.size(), 0, 0, resizemode());

		if (_bg.empty() || dst.cols != _dst.cols || dst.rows != _dst.rows || dst.cols != _bg.cols || dst.rows != _bg.rows)
		{
			make_bgra_bg(dst, _bg);
		}

		Point pt;
		for (pt.y = 0; pt.y < dst.rows; pt.y++)
		{
			uchar* p1 = tm.ptr<uchar>(pt.y);
			for (pt.x = 0; pt.x < dst.cols; pt.x++)
			{
				if (p1[pt.x] <= 1)
				{
					dst.at<Vec3b>(pt) = _bg.at<Vec3b>(pt);
				}
			}
		}
	}
	displayImage(1, dst);
}

void CPaint::redraw(int idx, int fx)
{
	_pt1_sticker = Point(0, 0);
	_pt2_sticker = Point(0, 0);

	if (_opt_arr.empty())
	{
		_weightedImage = _lena2.clone();
		_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
		_lena_bak = _weightedImage.clone();
		_working = act_display;
		return;
	}

	if (_opt_arr[0].opt_type == btn_init)
	{
		if (idx == 0)
		{
			load_image("1", NULL, false);
			high_displayImage();
			_dst_tips = Mat::zeros(Size(0, 0), CV_8UC1);
			return;
		}
		else
		{
			load_image("1", _file_temp.c_str(), false);
			_lena_in_frame = _opt_arr[0].lena_in_frame;
		}
	}

	if (_lena2.empty())
	{
		return;
	}
	_crop_mouseup = false;

	reset_frame();

	int len = (int)_opt_arr.size();
	//_opt = 0;


	if (!_opt_arr.empty())
	{
		int ii = len - 1;
		int opt = _opt_arr[ii].opt_type;
		if (opt == btn_change_bg_ok)
		{
			if (len == idx)
			{
				_brfore_change_bg = Rect(_lena_in_frame.x, _lena_in_frame.y, _dst_zoom_w, _dst_zoom_h);
			}
			else if (ii == idx && fx == 0)
			{
				_change_bg = Rect(_lena_in_frame.x, _lena_in_frame.y, _dst_zoom_w, _dst_zoom_h);
			}
		}
	}

	if (idx >= 0)
	{
		int cur_deg = 0;

		_result = Mat::zeros(Size(0, 0), CV_8UC1);
		_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		_eraser_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		_rect_seg_select = Rect(0, 0, 0, 0);
		_sticker_arr.clear();
		_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);

		_lena_w = _lena2.cols;
		_lena_h = _lena2.rows;
		init_fVal(_fVal);

		if (idx == 0)
		{
			_weightedImage = _lena2.clone();
			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
			_lena_bak = _weightedImage.clone();

			if (!_bgra_mask.empty())
			{
				_eraser_mask = _bgra_mask.clone();
			}

			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);

			_lena_in_frame = _lena_in_frame0;

		}
		else if (idx > 0 && idx <= len)
		{
			redraw_helper(idx, cur_deg);
		}

		if (abs(_total_deg % 180) != abs(cur_deg % 180))
		{
			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;
			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
		}
		_total_deg = cur_deg;
	}

	_mask_idx = -997;

	int k = idx - 1;
	if (k >= 0 && k < _opt_arr.size())
	{
		int na = _opt_arr[k].nav_type;
		//if (na == nav_filter)
		//{
		//}
		//else if (na != 0 && na != _nav_btn)


		if (na == nav_inp)
		{
			_nav_btn = na;
			_opt = _opt_arr[k].opt_type;
			_pointer_btn = 0;
		}
	}

	high_displayImage();

	if (!_opt_arr.empty())
	{
		Rect rc0 = Rect(0, 0, 0, 0);
		int ii = len - 1;
		int opt = _opt_arr[ii].opt_type;
		if (opt == btn_change_bg || opt == btn_change_bg_ok)
		{
			if (opt == btn_change_bg)
			{
				_opt_arr.erase(end(_opt_arr) - 1);
			}

			if (len == idx && opt == btn_change_bg_ok)
			{
				rc0 = _change_bg;
			}
			else if (ii == idx && fx == 0)
			{
				rc0 = _brfore_change_bg;
				_lena_w = _weightedImage.cols;
				_lena_h = _weightedImage.rows;
			}
		}

		if (rc0.width > 0)
		{
			xrect_img = rc0.x;
			yrect_img = rc0.y;
			_dst_zoom_w = rc0.width;
			_dst_zoom_h = rc0.height;

			_lena_in_frame.x = xrect_img;
			_lena_in_frame.y = yrect_img;
			_lena_in_frame0 = _lena_in_frame;
			_dst_in_frame = Point(_lena_in_frame.x, _lena_in_frame.y);

			_zoom_r = (double)_dst_zoom_w / (double)_lena_w;

			_working = act_display;
		}
	}
}

void CPaint::redraw_helper(int idx, int& cur_deg)
{
	cur_deg = 0;
	if (idx > _opt_arr.size())
	{
		return;
	}

	int ival = 0;
	if (_opt_arr[0].opt_type == btn_init)
	{
		ival = 1;
		if (idx == 1)
		{
			return;
		}
	}

	int inp = 0;
	for (int i = ival; i < idx; i++)
	{
		int opt_type = _opt_arr[i].opt_type;
		if (opt_type == inpaint_ok)
		{
			inp = i;
		}
	}

	if (inp > 0)
	{
		_weightedImage = _opt_arr[inp].image.clone();
		_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
		_lena_bak = _weightedImage.clone();
	}
	else
	{
		_weightedImage = _lena2.clone();
		_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
		_lena_bak = _weightedImage.clone();
	}

	if (!_bgra_mask.empty())
	{
		_eraser_mask = _bgra_mask.clone();
	}

	opt_t pre_op;

	int angle = 0;
	for (int i = inp; i < idx; i++)
	{
		int opt_type = _opt_arr[i].opt_type;
		int pts_len = (int)_opt_arr[i].pts.size();
		int thick = _opt_arr[i].thickness;
		_lena_in_frame = _opt_arr[i].lena_in_frame;
		_dst_zoom_w = (int)(_lena_w * _zoom_r);
		_dst_zoom_h = (int)(_lena_h * _zoom_r);
		init_fVal(_fVal);

		bool b = false;
		if (1 == idx)
		{
			if (opt_type == btn_rota)
			{
				angle = _opt_arr[i].thickness;
				b = true;
			}
		}
		else
		{
			if (pre_op.opt_type == btn_rota)
			{
				if (opt_type != btn_rota)
				{
					b = true;
				}
			}

			if (i == idx - 1)
			{
				if (opt_type == btn_rota)
				{
					angle += _opt_arr[i].thickness;
					b = true;
				}
			}
		}

		if (b)
		{
			cur_deg = angle;
			int ret = rotate(angle);
		}

		if (opt_type == btn_brush || opt_type == btn_free_select)
		{
			int th = thick;
			if (th <= 0)
			{
				th = _inpaint_thickness;
			}

			Point pt0 = _opt_arr[i].pts[0];
			for (int j = 1; j < pts_len; j++)
			{
				Point pt = _opt_arr[i].pts[j];
				line(_inpaintMask, pt0, pt, Scalar::all(255), th, 8, 0);
				pt0 = pt;
			}

			if (opt_type == btn_free_select)
			{
				vector<vector<Point>> arr;
				arr.push_back(_opt_arr[i].pts);

				fillPoly(_inpaintMask, arr, Scalar::all(255));
			}
		}
		else if (opt_type == btn_magic || opt_type == btn_draw_fill || opt_type == btn_draw_magic)
		{
			Point pt0 = _opt_arr[i].pts[0];
			int loDiff = _opt_arr[i].loDiff;
			int upDiff = _opt_arr[i].upDiff;

			Rect roi_rect;
			if (opt_type == btn_magic)
			{
				Mat afterROI = _inpaintMask;
				seamless_10(afterROI, roi_rect, _weightedImage.clone(), pt0, loDiff, upDiff);
			}
			else if (opt_type == btn_draw_fill)
			{
				Mat afterROI = Mat::zeros(_weightedImage.size(), CV_8UC1);
				seamless_10(afterROI, roi_rect, _weightedImage.clone(), pt0, loDiff, upDiff);
				afterROI = afterROI(roi_rect);

				Scalar color = _opt_arr[i].color;
				drawWeighted_picker(_weightedImage, roi_rect, afterROI, color);
				_lena_bak = _weightedImage.clone();
			}
			else
			{
				seamless_10(_eraser_mask, roi_rect, _weightedImage.clone(), pt0, loDiff, upDiff, 0);
			}
		}
		else if (opt_type == btn_eraser)
		{
			int th = thick;
			if (th <= 0)
			{
				th = _inpaint_thickness;
			}
			paint_eraser(3, _opt_arr[i].pts, th);
		}
		else if (opt_type == btn_rect)
		{
			Point pt0 = _opt_arr[i].pts[0];
			Point pt1 = _opt_arr[i].pts[1];

			rectangle(_inpaintMask, pt0, pt1, Scalar::all(255), -1, LINE_8, 0);
		}
		else if (opt_type == btn_draw_free || opt_type == btn_draw_straight || opt_type == btn_draw_eraser ||
			opt_type == btn_draw_circle || opt_type == btn_draw_arrow || opt_type == btn_draw_rect)
		{
			Point pt0 = _opt_arr[i].pts[0];

			int th = thick;
			if (th <= 0)
			{
				th = _draw_thickness;

				if (opt_type == btn_draw_eraser)
				{
					th = _erase_thickness;
				}
			}

			Scalar color = _opt_arr[i].color;
			if (opt_type == btn_draw_eraser)
			{
				color = Scalar::all(255);
				//if (_eraser_mask.empty())
				//{
				//	_eraser_mask = Mat::zeros(_weightedImage.size(), CV_8UC1);
				//}
			}

			for (int j = 1; j < pts_len; j++)
			{
				Point pt = _opt_arr[i].pts[j];
				if (opt_type == btn_draw_free || opt_type == btn_draw_straight)
				{
					line(_weightedImage, pt0, pt, color, th, 8, 0);
					line(_lena_bak, pt0, pt, color, th, 8, 0);
					if (!_eraser_mask.empty())
					{
						line(_eraser_mask, pt0, pt, Scalar::all(255), th, 8, 0);
					}
				}
				else if (opt_type == btn_draw_eraser)
				{
					line(_weightedImage, pt0, pt, color, th, 8, 0);

					line(_lena_bak, pt0, pt, color, th, 8, 0);
					//if (_channels == 4)
					//{
					//	if (!_eraser_mask.empty())
					//	{
					//		line(_eraser_mask, pt0, pt, color, th, 8, 0);
					//	}
					//}
				}
				else if (opt_type == btn_draw_circle)
				{
					Size sz = Size(abs(pt.x - pt0.x), abs(pt.y - pt0.y));
					Point c = Point(pt0.x + sz.width / 2, pt0.y + sz.height / 2);

					ellipse(_weightedImage, c, sz, 0, 0, 360, color, th, 8);
					ellipse(_lena_bak, c, sz, 0, 0, 360, color, th, 8);
					if (!_eraser_mask.empty())
					{
						ellipse(_eraser_mask, c, sz, 0, 0, 360, Scalar::all(255), th, 8);
					}
				}
				else if (opt_type == btn_draw_arrow) {

					arrowedLine(_weightedImage, pt0, pt, color, th, 8, 0, 0.1);
					arrowedLine(_lena_bak, pt0, pt, color, th, 8, 0, 0.1);
					if (!_eraser_mask.empty())
					{
						arrowedLine(_eraser_mask, pt0, pt, Scalar::all(255), th, 8, 0, 0.1);
					}
				}
				else if (opt_type == btn_draw_rect) {
					rectangle(_weightedImage, pt0, pt, color, th, LINE_8, 0);
					rectangle(_lena_bak, pt0, pt, color, th, LINE_8, 0);
					if (!_eraser_mask.empty())
					{
						rectangle(_eraser_mask, pt0, pt, Scalar::all(255), th, LINE_8, 0);
					}
				}
				pt0 = pt;
			}
		}
		else if (opt_type == btn_draw_mosaic)
		{
			paint_mosaic(_opt_arr[i].pts, _erase_thickness);
		}
		else if (opt_type == btn_rota)
		{
		}
		else if (opt_type == btn_flipv || opt_type == btn_fliph)
		{
			int type = 0;
			if (opt_type == btn_fliph)
			{
				type = 1;
			}

			flip(_weightedImage, _weightedImage, type);
			flip(_inpaintMask, _inpaintMask, type);
			flip(_lena_bak, _lena_bak, type);

			if (!_eraser_mask.empty())
			{
				flip(_eraser_mask, _eraser_mask, type);
			}
		}
		else if (opt_type == btn_crop)
		{
			Point pt0 = _opt_arr[i].pts[0];
			Point pt1 = _opt_arr[i].pts[1];

			Rect rt(pt0, pt1);

			_weightedImage = _weightedImage(rt);
			_inpaintMask = _inpaintMask(rt);
			_lena_bak = _lena_bak(rt);

			if (!_eraser_mask.empty())
			{
				_eraser_mask = _eraser_mask(rt);
			}

			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;

			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);

		}
		else if (opt_type == btn_pasteas)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_lena_bak = _weightedImage.clone();
			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
			_eraser_mask = _opt_arr[i].mask.clone();

			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;

			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);

		}
		else if (opt_type == btn_draw_text)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_eraser_mask = _opt_arr[i].mask.clone();
			_lena_bak = _weightedImage.clone();
		}
		else if (opt_type == btn_draw_sky)
		{
			_weightedImage = _opt_arr[i].image.clone();
			//_eraser_mask = _opt_arr[i].mask.clone();
			_lena_bak = _weightedImage.clone();
		}
		else if (opt_type == btn_change_bg)
		{
			break;
		}
		else if (opt_type == btn_sticker)
		{
			_sticker_arr = _opt_arr[i].sticker_arr;
		}
		else if (opt_type == btn_sticker_ok || opt_type == btn_change_bg_ok)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_eraser_mask = _opt_arr[i].mask.clone();
			_lena_bak = _weightedImage.clone();
			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);

			if (opt_type == btn_change_bg_ok)
			{
				_lena_w = _weightedImage.cols;
				_lena_h = _weightedImage.rows;
			}
		}
		else if (opt_type == btn_seg_fgd || opt_type == btn_seg_bgd)
		{
			_seg_mask = _opt_arr[i].mask.clone();
			_result = _opt_arr[i].image.clone();
		}
		else if (opt_type == btn_seg_rect)
		{
			Point pt0 = _opt_arr[i].pts[0];
			Point pt1 = _opt_arr[i].pts[1];

			_rect_seg_select = Rect(pt0, pt1);

			_seg_mask = _opt_arr[i].mask.clone();
			_result = _opt_arr[i].image.clone();
		}
		else if (opt_type == btn_seg_apply)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_eraser_mask = _opt_arr[i].mask.clone();

			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
			_lena_bak = _weightedImage.clone();
			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;

			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
		}
		else if (opt_type == btn_draw_trans)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_eraser_mask = _opt_arr[i].mask.clone();

			_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
			_lena_bak = _weightedImage.clone();
			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;
		}
		else if (opt_type == btn_filter)
		{
			_weightedImage = _opt_arr[i].image.clone();
			_lena_bak = _opt_arr[i].mask.clone();

			_fVal = _opt_arr[i].fVal;
		}
		else if (opt_type == btn_paste)
		{
			if (thick == 3)
			{
				//Mat tmp = _opt_arr[i].image;
				//tmp.copyTo(_weightedImage(Rect(0, 0, tmp.cols, tmp.rows)));
			}
			else
			{
				_weightedImage = _opt_arr[i].image.clone();
				_eraser_mask = _opt_arr[i].mask.clone();
				_lena_bak = _weightedImage.clone();
			}
		}

		if (opt_type == btn_rota)
		{
			angle += _opt_arr[i].thickness;
		}
		else
		{
			angle = 0;
		}

		pre_op = _opt_arr[i];
	}

	double alphaReserve = 0.6;
	int BChannel = 0;
	int GChannel = 0;
	int RChannel = 255;

	vector<Point> ps;
	findNonZero(_inpaintMask, ps);
	int len = (int)ps.size();
	for (int i = 0; i < len; i++)
	{
		if (_inpaintMask.at<uchar>(ps[i]) == 255)
		{
			Vec3b& v = _weightedImage.at<Vec3b>(ps[i]);
			alpha(v, BChannel, GChannel, RChannel, alphaReserve);
		}
	}
}

void CPaint::displayImage(int type, Mat dst)
{
	if (_lena2.empty())
	{
		return;
	}

	if (type == 100)
	{
		if (!_weighted_era.empty())
		{
			int len3 = countNonZero(_weighted_mask);
			if (len3 > 0)
			{
				set_mask_by_v1(_weighted_era, _weighted_mask);
			}

			Mat era = _weighted_era(_dst_in_lena);
			resize(era, era, dst.size(), 0, 0, resizemode());

			if (_bg.empty() || dst.cols != _dst.cols || dst.rows != _dst.rows || dst.cols != _bg.cols || dst.rows != _bg.rows)
			{
				make_bgra_bg(dst, _bg);
			}

			Point pt;
			for (pt.y = 0; pt.y < dst.rows; pt.y++)
			{
				uchar* p1 = era.ptr<uchar>(pt.y);
				for (pt.x = 0; pt.x < dst.cols; pt.x++)
				{
					if (p1[pt.x] <= 1)
					{
						dst.at<Vec3b>(pt) = _bg.at<Vec3b>(pt);
					}
				}
			}
		}
	}
	else
	{
		reset_frame();
	}

	Mat imageROI = _frame(Rect(_dst_in_frame.x, _dst_in_frame.y, dst.cols, dst.rows));
	dst.copyTo(imageROI);

	if (type == 1)
	{
		_dst = dst.clone();
	}

	Mat dst2;

	if (!_clip_mat.empty())
	{
		Rect rc, sr;
		sr = _rect_paste;
		rc = Rect(0, 0, sr.width, sr.height);
		proc_clip_rc(rc, sr, dst);

		if (rc.width <= 0 || rc.height <= 0 || rc.width > _clip_mat.cols || rc.height > _clip_mat.rows)
		{
		}
		else
		{
			Point pt1 = Point(_dst_in_frame.x + sr.x, _dst_in_frame.y + sr.y);
			Point pt2 = Point(pt1.x + sr.width, pt1.y + sr.height);

			drawDashRect(_frame, 5, 5, pt1, pt2);
			dst2 = _frame(Rect(_dst_in_frame.x, _dst_in_frame.y, dst.cols, dst.rows)).clone();
		}
	}

	if (_rect_select.width > 0 && _rect_select.height > 0)
	{
		if (dst2.empty())
		{
			dst2 = dst.clone();
		}
		Rect rc = _rect_select;
		show_rect(dst2, rc, 0);
		dst2.copyTo(imageROI);
	}
	else if (_rect_crop.width >= 60 && _rect_crop.height >= 60)
	{
		Rect rc = _rect_crop;
		rc.x = _dst_in_frame.x + rc.x;
		rc.y = _dst_in_frame.y + rc.y;

		show_rect(_frame, rc, btn_crop);
	}

	if (!_seg_mask.empty() && _rect_seg_select.width > 0)
	{
		if (dst2.empty())
		{
			dst2 = dst.clone();
		}

		drawWeighted_seg(dst2, _seg_mask);
		dst2.copyTo(imageROI);
	}

	if (!_result.empty())
	{
		if (dst2.empty())
		{
			dst2 = dst.clone();
		}
		Mat result = _result.clone();

		if (_lena_w >= _dst_zoom_w)
		{
		}
		else
		{
			//Mat image = _weightedImage(_dst_in_lena).clone();
			//resize_mask(image, result, dst2.size());
			resize(result, result, dst2.size(), 0, 0, resizemode());
		}

		double alphaReserve = 0.6;
		int BChannel = 0;
		int GChannel = 0;
		int RChannel = 255;

		for (int i = 0; i < dst2.rows; i++)
		{
			uchar* p1 = result.ptr<uchar>(i);
			Vec3b* ptr = dst2.ptr<Vec3b>(i);

			for (int j = 0; j < dst2.cols; j++)
			{
				if (p1[j] == 0)
				{
					BChannel = 0;
					RChannel = 255;
				}
				else
				{
					BChannel = 255;
					RChannel = 0;
				}
				alpha(ptr[j], BChannel, GChannel, RChannel, alphaReserve);
			}
		}

		dst2.copyTo(imageROI);
	}

	if (!_sticker_arr.empty())
	{
		if (dst2.empty())
		{
			dst2 = dst.clone();
		}

		sticker_ok(dst2, imageROI);

		//if (_opt == btn_change_bg)
		//{
		//	Rect rc;
		//	rc.width = _dst.cols;
		//	rc.height = _dst.rows;
		//	rc.x = _dst_in_frame.x;
		//	rc.y = _dst_in_frame.y;

		//	show_rect(_frame, rc, btn_change_bg);
		//}
	}
	else
	{
		_pt1_sticker = Point(0, 0);
		_pt2_sticker = Point(0, 0);
	}

	if (_hInputWnd != NULL && _input_win_in_dst.width > 0)
	{
		if (dst2.empty())
		{
			dst2 = dst.clone();
		}
		RECT rw;
		::GetWindowRect(_win_handle, &rw);
		_win_handle_rw.left = rw.left;
		_win_handle_rw.top = rw.top;
		_win_handle_rw.right = rw.right;
		_win_handle_rw.bottom = rw.bottom;

		Rect rc = _input_win_in_dst;

		Point pt1 = Point(rc.x, rc.y);
		Point pt2 = Point(pt1.x + rc.width, pt1.y + rc.height);

		drawDashRect(dst2, 1, 5, pt1, pt2);
		dst2.copyTo(imageROI);

		if (_input_win_in_dst_last != _input_win_in_dst)
		{
			::MoveWindow(_hInputWnd, pt1.x + rw.left + 4 + _dst_in_frame.x,
				pt1.y + rw.top + 4 + _dst_in_frame.y, rc.width - 8, rc.height - 8, TRUE);

			HWND hEdit = GetDlgItem(_hInputWnd, IDC_EDIT1);
			::MoveWindow(hEdit, 0, 0, rc.width - 8, rc.height - 8, TRUE);

			if (!IsWindowVisible(_hInputWnd))
			{
				::ShowWindow(_hInputWnd, SW_SHOW);
				SetForegroundWindow(_hInputWnd);
			}

			RECT rc0;
			::GetWindowRect(_hInputWnd, &rc0);
			_input_wnd_offset = Point(rc0.left - rw.left, rc0.top - rw.top);

			LOGFONT lf = _input_lf;
			lf.lfHeight = lf.lfHeight * _dst_zoom_w / _lena_w;

			HFONT hfont = CreateFontIndirect(&lf);
			::PostMessage(hEdit, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
			//DeleteObject(hfont);
			//::SendMessage(hEdit, EM_SETMARGINS, EC_USEFONTINFO, MAKELPARAM(30, 20));//EC_LEFTMARGIN

			SetFocus(hEdit);
			//::PostMessage(hEdit, WM_SETFOCUS, 0, 0);

			if (wcslen(_input_buf) > 0)
			{
				SetDlgItemText(_hInputWnd, IDC_EDIT1, _input_buf);
				memset(_input_buf, 0, 2048 * sizeof(wchar_t));
			}

			_input_win_in_dst_last = _input_win_in_dst;
		}
	}

	if (!dst2.empty())
	{
		_dst_tips = dst2.clone();
	}
	else
	{
		_dst_tips = dst.clone();
	}
}

void CPaint::set_opt_arr()
{
	if (_opt_arr.empty() || _idx <= 0)
	{
		_lena_in_frame0 = _lena_in_frame;
		return;
	}

	EnterCriticalSection(&_cs);
	{
		if (_idx > 0 && _idx <= _opt_arr.size())
		{
			bool b = false;
			int kk = 0;
			for (int i = 0; i < _opt_arr.size(); i++)
			{
				if (_opt_arr[i].opt_type == btn_crop)
				{
					kk = i;
					b = true;
					break;
				}
			}

			if (b)
			{
				if (_idx - 1 < kk)
				{
					for (int i = 0; i < kk; i++)
					{
						_opt_arr[i].lena_in_frame = _lena_in_frame;
					}
					_lena_in_frame0 = _lena_in_frame;
				}
				else
				{
					for (int i = kk; i < _opt_arr.size(); i++)
					{
						_opt_arr[i].lena_in_frame = _lena_in_frame;
					}
				}
			}
			else
			{
				for (int i = 0; i < _opt_arr.size(); i++)
				{
					_opt_arr[i].lena_in_frame = _lena_in_frame;
				}
				_lena_in_frame0 = _lena_in_frame;
			}
		}
	}
	LeaveCriticalSection(&_cs);
}