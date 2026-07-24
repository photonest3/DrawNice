///////////////////////////////////////////////////////////////////////
/// @file paint_mouse.cpp
/// @brief PhotoNest 绘图模块 - 鼠标事件处理实现文件
/// @details 实现鼠标事件处理功能:
///           - 鼠标按下事件 (mouse_down)
///           - 鼠标移动事件
///           - 鼠标释放事件
///           - 输入框交互处理
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的核心交互文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "cvui.h"
#include "resource.h"
#include<fstream>
#include "unitil2.h"
#include <process.h>

void CPaint::mouse_down()
{
	_space_key_mouse = false;

	_pt_down = Point(0, 0);
	_pts.clear();
	_drag_rc_type = 0;

	_pt_prev = Point(0, 0);
	int pre_input_box = 0;
	_is_in_combo = false;

	bool b1 = mouse_down_input(_input_win_in_dst, pre_input_box);
	if (b1)
	{
		return;
	}

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;
	_pt_all_down = Point(x, y);

	nav_saveas_is_in_combo(x, y, _is_in_combo);

	if (!_is_in_combo)
	{
		mouse_down_init(_rect_crop, _pt_prev);
		if (_rect_select.width != 0)
		{
			if (!_for_select)
			{
				mouse_down_init(_rect_select, _pt_prev);
			}
		}
	}

	if (!_sticker_arr.empty())
	{
		b1 = mouse_down_sticker();
		if (b1)
		{
			_is_sticker_move = true;
			return;
		}
	}

	b1 = is_area_inp(x, y);
	if (b1)
	{
		return;
	}

	if (!_clip_mat.empty())
	{
		if (x > _lf_undo && x < _lf_undo + 32 && y > 0 && y < TOOLBAR_TOP_HEIGHT)
		{
			//undo
		}
		else if (x > _lf_undo + TOOLBAR_BTN_STEP_X && x < _lf_undo + TOOLBAR_BTN_STEP_X + 32 && y > 0 && y < TOOLBAR_TOP_HEIGHT)
		{
			//redo
		}
		else
		{
			int x0 = x - _dst_in_frame.x;
			int y0 = y - _dst_in_frame.y;
			Point pt(x0, y0);

			if (is_out_rect(_rect_paste, pt, 0))
			{
				paste_ok();
			}
		}
	}

	if (!_down_nav_click)
	{
		int lf3 = get_toolbar_lf();
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top;

		if (_nav_btn == nav_inp)
		{
			//top3 += TOOLBAR_BTN_STEP_Y;
			if (x > lf3 && x < lf3 + _combo_width_inp && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 5)
			{
				_is_in_combo = true;
			}
		}
		else if (_nav_btn == nav_segment)
		{
			//top3 += TOOLBAR_BTN_STEP_Y;
			if (x > lf3 && x < lf3 + _combo_width_seg && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 4)
			{
				_is_in_combo = true;
			}
		}
		else if (_nav_btn == nav_sticker)
		{
			//top3 += TOOLBAR_BTN_STEP_Y;//* 3;
			if (x > lf3 && x < lf3 + _combo_width_sticker && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 2)
			{
				_is_in_combo = true;
			}
		}
		else if (_nav_btn == nav_filter)
		{
			//top3 += TOOLBAR_BTN_STEP_Y;
			if (x > lf3 && x < lf3 + _combo_width_filter && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 9)
			{
				_is_in_combo = true;
			}
		}
		else if (_nav_btn == nav_draw)
		{
			//top3 += TOOLBAR_BTN_STEP_Y;
			if (x > lf3 && x < lf3 + _combo_width_draw && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 8)
			{
				_is_in_combo = true;
			}
		}
	}

	if (_show_help == 1)
	{
		int lf0 = _win_w + TOOLBAR_RIGHT_WIDTH - 10 - _combo_width_help;
		if (x > lf0 && x < lf0 + _combo_width_help &&
			y > TOOLBAR_BTN_TOP && y < TOOLBAR_TOP_HEIGHT + TOOLBAR_MEN_STEP_Y * 4)
		{
			_is_in_combo = true;
		}
		else
		{
			_show_help = 2;
			_working = act_display;
		}
	}

	if (_opt == btn_sticker || _opt == btn_change_bg)
	{
		int bottom = TOOLBAR_TOP_HEIGHT;
		if (_down_sticker_click)
		{
			bottom = TOOLBAR_TOP_HEIGHT + TOOLBAR_MEN_STEP_Y * 4;
		}

		if (x > _lf_font && x <  _lf_font + lbl_seamless.cols  && y > TOOLBAR_BTN_TOP && y < bottom)
		{
			_is_in_combo = true;
		}

		if (x > _lf_font + lbl_seamless.cols + 10 && x <  _lf_font + lbl_seamless.cols + 10 + 32 && y > TOOLBAR_BTN_TOP && y < TOOLBAR_TOP_HEIGHT)
		{
			_is_in_combo = true;
		}
	}


	if (_opt == btn_rota_bar)
	{
		int off = 5;
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;
		int lf = 8 + _combo_width_draw;

		if (x > lf && x < lf + (crop_custom_idle.cols + off) * 5 && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
		{

		}
		else
		{
			lf = 8 + crop_custom_idle.cols + off;
			int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 7;

			if (x > lf && x < lf + crop_custom_idle.cols && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
			{

			}
			else
			{
				_opt = 0;
			}
		}
	}
	else if (_opt == btn_crop_bar)
	{
		int off = 5;
		int lf = 8 + _combo_width_draw;

		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;

		if (x > lf && x < lf + (crop_custom_idle.cols + off) * 7 && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
		{
		}
		else
		{
			lf = 8;
			int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 7;

			if (x > lf && x < lf + crop_custom_idle.cols && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
			{

			}
			else
			{
				_opt = 0;
			}
		}
	}

	if (!_is_in_combo)
	{
		blank_click();

		navbar_state_ui(x, y, "down");

		if (_opt == btn_brush || _opt == btn_free_select || _opt == btn_eraser || _opt == btn_draw_mosaic)
		{
			_weighted_temp = _weightedImage.clone();
			_weighted_mask = _inpaintMask.clone();
			if (!_eraser_mask.empty())
			{
				_weighted_era = _eraser_mask.clone();
			}
		}

		if (is_area_dst(x, y))
		{
			_pt_down = Point(x, y);

			int x0 = x - _lena_in_frame.x;
			int y0 = y - _lena_in_frame.y;
			_prePoint = get_lena_pt(x0, y0);

			xrect_img = _lena_in_frame.x;
			yrect_img = _lena_in_frame.y;

			if (_pointer_btn == btn_move)
			{
				return;
			}

			if (_opt == btn_draw_picker)
			{
				Vec3b v = _weightedImage.at<Vec3b>(_prePoint);
				_picker_val = Scalar(v[0], v[1], v[2]);
				_opt = _last_draw_opt;
				return;
			}

			if (_opt == btn_brush || _opt == btn_free_select)
			{
				_pts.push_back(_prePoint);
			}
			else if (_opt == btn_eraser)
			{
				_pts.push_back(_prePoint);
			}
			else if (_opt == btn_rect)
			{
				_pts.push_back(_prePoint);
			}
			else if (_opt == btn_draw_mosaic)
			{
				_pts.push_back(_prePoint);
			}
			else if (_opt == btn_seg_fgd || _opt == btn_seg_bgd)
			{
				_in_rect_seg_select = false;
				_prePoint.x = _pt_down.x - _dst_in_frame.x;
				_prePoint.y = _pt_down.y - _dst_in_frame.y;
				_pts.push_back(_prePoint);
				if (at_rect(_prePoint, _rect_seg_select))
				{
					_in_rect_seg_select = true;
				}
			}
			else if (_opt == btn_seg_rect)
			{
				if (!_seg_mask.empty())
				{
					_seg_bak = _seg_mask.clone();
					_rect_seg_bak = _rect_seg_select;
				}
				else
				{
					_seg_bak = Mat::zeros(0, 0, CV_8UC1);
					_rect_seg_bak = Rect(0, 0, 0, 0);
				}

				_rect_seg_select = Rect(0, 0, 0, 0);
				_seg_mask = Mat::zeros(0, 0, CV_8UC1);

				_prePoint.x = _pt_down.x - _dst_in_frame.x;
				_prePoint.y = _pt_down.y - _dst_in_frame.y;
				_pts.push_back(_prePoint);
			}
			else if (_opt == btn_magic || _opt == btn_draw_fill || _opt == btn_draw_magic)
			{
				_pts.push_back(_prePoint);

				Rect roi_rect;
				if (_opt == btn_magic)
				{
					Mat beforeROI = _inpaintMask.clone();
					Mat afterROI = _inpaintMask;

					seamless_10(afterROI, roi_rect, _weightedImage.clone(), _prePoint, _loDiff, _upDiff);
					beforeROI = beforeROI(roi_rect);
					afterROI = afterROI(roi_rect);
					drawWeighted(_weightedImage(roi_rect), beforeROI, afterROI);
				}
				else if (_opt == btn_draw_fill)
				{
					Mat afterROI = Mat::zeros(_weightedImage.size(), CV_8UC1);

					seamless_10(afterROI, roi_rect, _weightedImage.clone(), _prePoint, _loDiff, _upDiff);
					afterROI = afterROI(roi_rect);
					drawWeighted_picker(_weightedImage, roi_rect, afterROI, _picker_val);
					_lena_bak = _weightedImage.clone();
				}
				else
				{
					seamless_10(_eraser_mask, roi_rect, _weightedImage.clone(), _prePoint, _loDiff, _upDiff, 0);
				}

				_working = act_display;
				_changed = true;
			}
			else if (_opt == btn_draw_free || _opt == btn_draw_eraser)
			{
				//if (_channels == 4 && _eraser_mask.empty())
				//{
				//	_eraser_mask = Mat::zeros(_weightedImage.size(), CV_8UC1);
				//}

				_pts.push_back(_prePoint);
				if (!_eraser_mask.empty())
				{
					_weighted_era = _eraser_mask.clone();
				}
			}
			else if (_opt == btn_draw_straight || _opt == btn_draw_arrow || _opt == btn_draw_rect || _opt == btn_draw_circle)
			{
				_pts.push_back(_prePoint);
			}

			if (_opt == btn_draw_text)
			{
				bool bNew = false;
				if (_weighted_befoe_input.empty())
				{
					bNew = true;
				}
				else
				{
					if (pre_input_box == 1)
					{
					}
					else
					{
						bNew = true;
					}
				}

				if (bNew && pre_input_box == 0)
				{
					_weighted_befoe_input = _weightedImage.clone();

					POINT pt;
					GetCursorPos(&pt);
					memset(_input_buf, 0, 2048 * sizeof(wchar_t));
					bool b2 = load_input_wnd(pt, 200, 0);
				}

				_working = act_display;
			}
		}
	}

}

void CPaint::mouse_isdown()
{
	_curr_rt = Rect(0, 0, 0, 0);
	int x = cvui::mouse().x;
	int y = cvui::mouse().y;

	if (_lena2.empty())
	{
		return;
	}

	bool b7 = false;
	if (_hInputWnd != NULL)
	{
		int x0 = x - _dst_in_frame.x;
		int y0 = y - _dst_in_frame.y;
		Point pt(x0, y0);

		int off = 40;
		if (pt.x > _input_win_in_dst.x + off && pt.x < _input_win_in_dst.x + _input_win_in_dst.width - off
			&& pt.y > _input_win_in_dst.y + off && pt.y < _input_win_in_dst.y + _input_win_in_dst.height - off)
		{
		}
		else
		{
			b7 = mouse_isdown_input(_input_win_in_dst, _pt_prev);
			if (_drag_rc_type != 0)
			{
				return;
			}
		}
	}
	if (_rect_crop.width != 0)
	{
		b7 = mouse_isdown_crop(_rect_crop, _pt_prev);
		if (_drag_rc_type != 0)
		{
			if (_drag_rc_type != 9)
			{
				_crop_opt = crop_0_0;
			}
			return;
		}
	}

	//if (_opt == btn_change_bg)
	//{
	//	b7 = false;
	//	Rect rc;
	//	rc.width = _dst.cols;
	//	rc.height = _dst.rows;
	//	rc.x = _dst_in_frame.x;
	//	rc.y = _dst_in_frame.y;

	//	int x0 = cvui::mouse().x;
	//	int y0 = cvui::mouse().y;
	//	Point pt(x0, y0);

	//	if (is_crop50(pt, rc))
	//	{
	//		if (!_sticker_arr.empty())
	//		{
	//			int k = get_thread_idx();
	//			threads[k].stop = false;
	//			threads[k].opt = btn_change_bg_ok;
	//			_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_sticker, this, NULL, NULL);
	//			threads[k].hdl = _handle_thread;
	//		}
	//		_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);
	//		_opt = 0;
	//		_last_opt = 0;
	//		_pointer_btn = btn_move;
	//		b7 = true;
	//	}
	//}

	if (_rect_select.width != 0)
	{
		b7 = mouse_isdown_input(_rect_select, _pt_prev);
		if (_drag_rc_type == 9)
		{
			//if (::GetKeyState(VK_CONTROL) >> 15)
			{
				Rect rc;
				copy_clipboard(rc);
				_rect_paste = _rect_select;
				paste_clipboard();

				_rect_select = Rect(0, 0, 0, 0);
				_working = act_display;
				_pointer_btn = btn_move;
			}
			return;
		}
		else if (_drag_rc_type != 0)
		{
			return;
		}
	}

	if (_rect_paste.width != 0)
	{
		b7 = mouse_isdown_paste(_rect_paste, _pt_prev);
		if (_drag_rc_type == 9)
		{
			_weightedImage = _lena_bak.clone();
			paste_ok(_weightedImage);
			return;
		}
		else if (_drag_rc_type != 0)
		{
			return;
		}
	}

	if (!_sticker_arr.empty())
	{
		int max_idx = 0;
		int nlen = (int)_sticker_arr.size();
		for (int i = 0; i < nlen; i++)
		{
			if (_sticker_arr[i].idx > max_idx)
			{
				max_idx = _sticker_arr[i].idx;
			}
		}
		for (int i = 0; i < nlen; i++)
		{
			if (_sticker_arr[i].idx == max_idx)
			{
				_curr_sticker_id = _sticker_arr[i].id;
				b7 = mouse_isdown_sticker(_sticker_arr[i].rc, _pt_prev, _sticker_arr[i].id);

				_curr_rt = _sticker_arr[i].rc;
				_curr_rt.x = _dst_in_lena.x + _curr_rt.x * _lena_w / _dst_zoom_w;
				_curr_rt.y = _dst_in_lena.y + _curr_rt.y * _lena_w / _dst_zoom_w;
				break;
			}
		}
		return;
	}

	if (b7)
	{
		return;
	}

	bool b1 = is_area_inp(x, y);
	if (b1 || _is_in_combo)
	{
		return;
	}
	_mask_idx = -997;

	if (_pointer_btn == btn_move)
	{
		if (isdo_nothing())
		{
			return;
		}

		int dx = x - _pt_down.x;
		int dy = y - _pt_down.y;

		int x1 = xrect_img + dx;
		int y1 = yrect_img + dy;

		if (_pt_down.x != 0 && _pt_down.y != 0 && (_lena_in_frame.x != x1 || _lena_in_frame.y != y1) &&
			x > TOOLBAR_LEFT_WIDTH + 10 && y > TOOLBAR_TOP_HEIGHT + 10 &&
			x < _win_w - 10 && y < _win_h - 10)
		{
			_lena_in_frame.x = x1;
			_lena_in_frame.y = y1;
			_working = act_display;
		}
		return;
	}

	bool b = is_area_dst(x, y);

	if (_opt == btn_brush || _opt == btn_free_select)
	{
		pts_empty(b, true, x, y);
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			_pts.push_back(pt);

			int th = _inpaint_thickness;
			if (_opt == btn_free_select)
			{
				Point pt0 = _pts[0];
				Rect rc(pt0, pt);
				if (rc.width > 0 && rc.height > 0)
				{
					_curr_rt = Rect(pt.x, pt.y, 0, 0);
					paint_brush(1, _pts, th);
				}
			}
			else
			{
				_curr_rt = Rect(pt.x, pt.y, 0, 0);
				paint_brush(1, _pts, th);
			}
		}
	}
	else if (_opt == btn_eraser)
	{
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			_pts.push_back(pt);
			_curr_rt = Rect(pt.x, pt.y, 0, 0);

			int th = _inpaint_thickness;
			paint_eraser(1, _pts, th);
		}
	}
	else if (_opt == btn_rect)
	{
		pts_empty(b, true, x, y);
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			if (_pts.size() == 1)
			{
				_pts.push_back(pt);
			}
			else
			{
				_pts[1] = pt;
			}

			Point pt0 = _pts[0];
			int x0 = pt0.x;
			int y0 = pt0.y;
			int x1 = pt0.x;
			int y1 = pt0.y;
			get_rect2(pt, x0, y0, x1, y1);

			_curr_rt = Rect(pt0, pt);
			if (_curr_rt.width > 0 && _curr_rt.height > 0)
			{
				get_rect1(THICKNESS_VAL, _lena_w, _lena_h, x0, y0, x1, y1);

				Rect rect(x0, y0, x1 - x0, y1 - y0);

				_weighted_temp = _weightedImage.clone();
				_weighted_mask = _inpaintMask.clone();

				Mat beforeROI = _weighted_mask(rect).clone();
				rectangle(_weighted_mask, pt0, pt, Scalar::all(255), -1, LINE_8, 0);

				if (!_eraser_mask.empty())
				{
					_weighted_era = _eraser_mask.clone();
					rectangle(_weighted_era, pt0, pt, Scalar::all(255), -1, LINE_8, 0);
				}

				drawWeighted(_weighted_temp(rect), beforeROI, _weighted_mask(rect));

				Mat dst_temp;// = _weighted_temp(_dst_in_lena);
				resize(_weighted_temp(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());

				displayImage(100, dst_temp);
			}
		}
	}
	else if (_opt == btn_draw_mosaic)
	{
		pts_empty(b, true, x, y);
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			_pts.push_back(pt);

			int th = _erase_thickness;
			Point pt0 = _pts[0];
			Rect rc(pt0, pt);
			//if (rc.width > 0 && rc.height > 0)
			{
				_curr_rt = Rect(pt.x, pt.y, 0, 0);
				paint_brush(1, _pts, th);
			}
		}

	}
	else if (_opt == btn_seg_fgd || _opt == btn_seg_bgd)
	{
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, false, x, y);
			_curr_rt = Rect(pt.x, pt.y, 0, 0);

			if (at_rect(pt, _rect_seg_select))
			{
				_in_rect_seg_select = true;
				int c = GC_FGD;
				if (_opt == btn_seg_bgd)
				{
					c = GC_BGD;
				}

				if (_seg_mask.empty())
				{
					_seg_mask = Mat::zeros(_dst.size(), CV_8UC1);
					if (_opt == btn_seg_bgd)
					{
						_seg_mask.setTo(GC_PR_FGD);
					}
					else
					{
						_seg_mask.setTo(GC_PR_BGD);
					}
				}

				line(_seg_mask, _prePoint, pt, c, 2/*THICKNESS_VAL*/, 8, 0);
			}
			_prePoint = pt;
			_working = act_display;
		}
	}
	else if (_opt == btn_magic || _opt == btn_draw_fill || _opt == btn_draw_magic)
	{
	}
	else if (_opt == btn_seg_rect)
	{
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, false, x, y);
			if (_pts.size() == 1)
			{
				_pts.push_back(pt);
			}
			else
			{
				_pts[1] = pt;
			}
			Point pt0 = _pts[0];

			_curr_rt = Rect(pt0, pt);
			if (_curr_rt.width > 0 && _curr_rt.height > 0)
			{
				Mat dst_temp = _dst.clone();
				rectangle(dst_temp, pt0, pt, Scalar(0, 255, 0), THICKNESS_VAL, LINE_8, 0);

				displayImage(100, dst_temp);
			}
		}
	}
	else if (_opt == btn_crop)
	{
		Point pt(0, 0);
		bool ispt = vvv(b, x, y, pt);
		if (!ispt)
		{
			return;
		}

		if (!is_crop50(pt, _rect_crop))
		{
			if (_crop_mouseup)
			{
				_crop_mouseup = false;
				_working = act_display;
			}
			else
			{
				Point pt0(_pt_down.x - _dst_in_frame.x, _pt_down.y - _dst_in_frame.y);
				proc_crop_pt(pt, pt0);

				_curr_rt = Rect(pt0, pt);
				if (_curr_rt.width > 0 && _curr_rt.height > 0)
				{
					Rect rc = _curr_rt;

					Mat dst_temp = _dst.clone();
					drawDashRect(dst_temp, 1, 3, Point(rc.x, rc.y), Point(rc.x + rc.width, rc.y + rc.height));
					displayImage(100, dst_temp);
				}
			}
		}
	}
	else if (_opt == btn_draw_free || _opt == btn_draw_eraser)
	{
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			_pts.push_back(pt);
			_curr_rt = Rect(pt.x, pt.y, 0, 0);

			int th = _draw_thickness;
			if (_opt == btn_draw_eraser)
			{
				th = _erase_thickness;
			}
			paint_free(1, _pts, th);
		}
	}
	else if (_opt == btn_draw_straight || _opt == btn_draw_arrow || _opt == btn_draw_rect || _opt == btn_draw_circle)
	{
		if (!_pts.empty())
		{
			Point pt = get_mouse_pt(b, true, x, y);
			if (_pts.size() == 1)
			{
				_pts.push_back(pt);
			}
			else
			{
				_pts[1] = pt;
			}

			Point pt0 = _pts[0];

			_curr_rt = Rect(pt0, pt);
			//if (_curr_rt.width > 0 && _curr_rt.height > 0)
			{
				int th = _draw_thickness;

				_weighted_temp = _weightedImage.clone();
				if (!_eraser_mask.empty())
				{
					_weighted_era = _eraser_mask.clone();
				}

				if (_opt == btn_draw_straight)
				{
					line(_weighted_temp, pt0, pt, _picker_val, th, 8, 0);
					if (!_weighted_era.empty())
					{
						line(_weighted_era, pt0, pt, Scalar::all(255), th, 8, 0);
					}
				}
				else if (_opt == btn_draw_arrow)
				{
					arrowedLine(_weighted_temp, pt0, pt, _picker_val, th, 8, 0, 0.1);
					if (!_weighted_era.empty())
					{
						arrowedLine(_weighted_era, pt0, pt, Scalar::all(255), th, 8, 0, 0.1);
					}
				}
				else if (_opt == btn_draw_circle)
				{
					Size sz = _curr_rt.size();
					Point c = Point(pt0.x + sz.width / 2, pt0.y + sz.height / 2);
					ellipse(_weighted_temp, c, sz, 0, 0, 360, _picker_val, th, 8);
					if (!_weighted_era.empty())
					{
						ellipse(_weighted_era, c, sz, 0, 0, 360, Scalar::all(255), th, 8);
					}
					_curr_rt = Rect(pt0.x - sz.width / 2, pt0.y - sz.height / 2, sz.width * 2, sz.height * 2);
				}
				else if (_opt == btn_draw_rect)
				{
					rectangle(_weighted_temp, pt0, pt, _picker_val, th, LINE_8, 0);
					if (!_weighted_era.empty())
					{
						rectangle(_weighted_era, pt0, pt, Scalar::all(255), th, LINE_8, 0);
					}
				}


				Mat dst_temp;// = _weighted_temp(_dst_in_lena);
				resize(_weighted_temp(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());

				displayImage(100, dst_temp);
			}
		}
	}
	else if (_opt == btn_draw_select)
	{
		Point pt(0, 0);
		bool ispt = vvv(b, x, y, pt);
		if (!ispt)
		{
			return;
		}

		Point pt0(_pt_down.x - _dst_in_frame.x, _pt_down.y - _dst_in_frame.y);

		_curr_rt = Rect(pt0, pt);
		if (_curr_rt.width > 0 && _curr_rt.height > 0)
		{
			Rect rc = _curr_rt;

			Mat dst_temp = _dst.clone();
			drawDashRect(dst_temp, 1, 3, Point(rc.x, rc.y), Point(rc.x + rc.width, rc.y + rc.height));
			displayImage(100, dst_temp);
		}
	}
}

void CPaint::mouse_up()
{
	int x = cvui::mouse().x;
	int y = cvui::mouse().y;

	navbar_state_ui(x, y, "up");

	if (_lena2.empty())
	{
		return;
	}
	_filter_mouseup = true;

	if (_is_sticker_move)
	{
		_is_sticker_move = false;
		_working = act_display;
		return;
	}

	if (_pointer_btn == btn_move)
	{
		return;
	}

	if (_hInputWnd != NULL)
	{
		_pt_prev = Point(0, 0);
	}

	_weighted_temp = Mat::zeros(0, 0, CV_8UC1);
	_weighted_mask = Mat::zeros(0, 0, CV_8UC1);
	_weighted_era = Mat::zeros(0, 0, CV_8UC1);

	if (_pt_all_down.x < TOOLBAR_LEFT_WIDTH)
	{
		if (_nav_btn == nav_inp || _nav_btn == nav_draw)
		{
			if (_pt_all_down.y > _left_bar_rc.y + _left_bar_rc.height && _pt_all_down.y < _win_h)
			{
				_pointer_btn = btn_move;
			}
		}
	}

	bool b1 = is_area_inp(x, y);
	if (b1 || _is_in_combo)
	{
		return;
	}

	if (_rect_crop.width != 0)
	{
		int x0 = x - _dst_in_frame.x;
		int y0 = y - _dst_in_frame.y;
		Point pt(x0, y0);

		if (!is_out_rect(_rect_crop, pt, 20))
		{
			_pt_prev = Point(0, 0);
			_curr_rt = Rect(0, 0, 0, 0);
			return;
		}
	}

	if (_rect_select.width != 0)
	{
		int x0 = x - _dst_in_frame.x;
		int y0 = y - _dst_in_frame.y;
		Point pt(x0, y0);

		if (!is_out_rect(_rect_select, pt, 20))
		{
			_pt_prev = Point(0, 0);
			_curr_rt = Rect(0, 0, 0, 0);
			return;
		}
	}

	if (!_clip_mat.empty())
	{
		int x0 = x - _dst_in_frame.x;
		int y0 = y - _dst_in_frame.y;
		Point pt(x0, y0);
		if (at_rect(pt, _rect_paste))
		{
			_pt_prev = Point(0, 0);
			_curr_rt = Rect(0, 0, 0, 0);
			return;
		}
	}

	bool b = is_area_dst(x, y);

	if (!_pts.empty())
	{
		if (_opt == btn_seg_fgd || _opt == btn_seg_bgd || _opt == btn_seg_rect)
		{
			bool b8 = false;
			if (_opt == btn_seg_rect)
			{
				Point pt = _pts[1];
				Point pt0 = _pts[0];
				Rect rc(pt0, pt);

				if (rc.width > 10 && rc.height > 10 && rc.width < _dst.cols && rc.height < _dst.rows)
				{
					_rect_seg_select = rc;
					_seg_mask = Mat::zeros(_dst.size(), CV_8UC1);
					_seg_mask.setTo(GC_BGD);
					(_seg_mask(rc)).setTo(Scalar(GC_PR_FGD));
				}
				else
				{
					if (!_seg_bak.empty())
					{
						_seg_mask = _seg_bak.clone();
						_rect_seg_select = _rect_seg_bak;
						b8 = true;
					}
					else
					{
						_seg_mask = Mat::zeros(_dst.size(), CV_8UC1);
						_rect_seg_select = Rect(0, 0, 0, 0);
					}
					_working = act_display;
				}
			}
			else
			{
				_curr_rt = Rect(0, 0, 0, 0);
			}

			if ((_opt == btn_seg_rect && _rect_seg_select.width > 10 && _rect_seg_select.height > 10
				&& _rect_seg_select.width < _dst.cols && _rect_seg_select.height < _dst.rows) ||
				((_opt == btn_seg_fgd || _opt == btn_seg_bgd) && _in_rect_seg_select))
			{
				if (!_seg_mask.empty() && !b8)
				{
					int k = get_thread_idx();
					threads[k].stop = false;
					_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_segment, this, NULL, NULL);
					threads[k].hdl = _handle_thread;
				}
			}
			else
			{
				if ((_opt == btn_seg_fgd || _opt == btn_seg_bgd) && !_in_rect_seg_select)
				{
					_disp_nb = 1;
					_show_err_type = 1;
				}
			}
		}
		else if (_opt == btn_brush || _opt == btn_free_select)
		{
			paint_brush(2, _pts, _inpaint_thickness);

			_curr_rt = Rect(0, 0, 0, 0);
			_changed = true;
			_working = act_display;
		}
		else if (_opt == btn_eraser)
		{
			int th = _inpaint_thickness;
			paint_eraser(2, _pts, th);
			_curr_rt = Rect(0, 0, 0, 0);
			_changed = true;
			_working = act_display;
		}
		else if (_opt == btn_rect)
		{
			Point pt = _pts[1];
			Point pt0 = _pts[0];
			int x0 = 0;
			int y0 = 0;
			int x1 = 0;
			int y1 = 0;

			int len = (int)_pts.size();
			for (int i = 0; i < len; i++)
			{
				Point pt2 = _pts[i];
				get_rect2(pt2, x0, y0, x1, y1);
			}
			get_rect1(THICKNESS_VAL, _lena_w, _lena_h, x0, y0, x1, y1);

			Rect rect(x0, y0, x1 - x0, y1 - y0);
			Mat beforeROI = _inpaintMask(rect).clone();

			rectangle(_inpaintMask, pt0, pt, Scalar::all(255), -1, LINE_8, 0);

			Mat afterROI = _inpaintMask(rect);
			drawWeighted(_weightedImage(rect), beforeROI, afterROI);

			_curr_rt = Rect(0, 0, 0, 0);
			_changed = true;
			_working = act_display;
		}
		else if (_opt == btn_draw_mosaic)
		{
			paint_mosaic(_pts, _erase_thickness);

			_curr_rt = Rect(0, 0, 0, 0);
			_changed = true;
			_working = act_display;
		}
		else if (_opt == btn_draw_free || _opt == btn_draw_straight || _opt == btn_draw_eraser
			|| _opt == btn_draw_arrow || _opt == btn_draw_rect || _opt == btn_draw_circle)
		{
			Point pt;
			Point pt0 = _pts[0];
			int th = _draw_thickness;

			Scalar color = _picker_val;
			if (_opt == btn_draw_eraser)
			{
				color = Scalar::all(255);
			}
			if (_opt == btn_draw_eraser)
			{
				th = _erase_thickness;
			}

			int pts_len = (int)_pts.size();
			for (int j = 1; j < pts_len; j++)
			{
				pt = _pts[j];

				if (_opt == btn_draw_free || _opt == btn_draw_straight)
				{
					line(_weightedImage, pt0, pt, _picker_val, th, 8, 0);
					line(_lena_bak, pt0, pt, _picker_val, th, 8, 0);
					if (!_eraser_mask.empty())
					{
						line(_eraser_mask, pt0, pt, Scalar::all(255), th, 8, 0);
					}
				}
				else if (_opt == btn_draw_eraser)
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
				else if (_opt == btn_draw_circle)
				{
					Size sz = Size(abs(pt.x - pt0.x), abs(pt.y - pt0.y));
					Point c = Point(pt0.x + sz.width / 2, pt0.y + sz.height / 2);
					ellipse(_weightedImage, c, sz, 0, 0, 360, _picker_val, th, 8);
					ellipse(_lena_bak, c, sz, 0, 0, 360, _picker_val, th, 8);

					if (!_eraser_mask.empty())
					{
						ellipse(_eraser_mask, c, sz, 0, 0, 360, Scalar::all(255), th, 8);
					}
				}
				else if (_opt == btn_draw_arrow) {
					arrowedLine(_weightedImage, pt0, pt, _picker_val, th, 8, 0, 0.1);
					arrowedLine(_lena_bak, pt0, pt, _picker_val, th, 8, 0, 0.1);

					if (!_eraser_mask.empty())
					{
						arrowedLine(_eraser_mask, pt0, pt, Scalar::all(255), th, 8, 0, 0.1);
					}
				}
				else if (_opt == btn_draw_rect) {
					rectangle(_weightedImage, pt0, pt, _picker_val, th, LINE_8, 0);
					rectangle(_lena_bak, pt0, pt, _picker_val, th, LINE_8, 0);

					if (!_eraser_mask.empty())
					{
						rectangle(_eraser_mask, pt0, pt, Scalar::all(255), th, LINE_8, 0);
					}
				}
				pt0 = pt;
			}

			_curr_rt = Rect(0, 0, 0, 0);
			_changed = true;
			_working = act_display;
		}
	}
	else if (_opt == btn_crop)
	{
		Point pt = Point(0, 0);
		bool ispt = vvv_help(b, x, y, pt);
		if (!ispt)
		{
			_curr_rt = Rect(0, 0, 0, 0);
			return;
		}

		if (_drag_rc_type == 0 && !is_crop50(pt, _rect_crop))
		{
			Point pt0(_pt_down.x - _dst_in_frame.x, _pt_down.y - _dst_in_frame.y);
			proc_crop_pt(pt, pt0);

			Rect rc(pt0, pt);
			if (rc.width > 60 && rc.height > 60)
			{
				_rect_crop = rc;
			}
			else
			{
				_rect_crop = Rect(0, 0, 0, 0);
				_curr_rt = Rect(0, 0, 0, 0);
			}
			_pt_prev = Point(0, 0);
			_crop_mouseup = true;
			_working = act_display;
		}
	}
	else if (_opt == btn_draw_select)
	{
		Point pt = Point(0, 0);
		bool ispt = vvv_help(b, x, y, pt);
		if (!ispt)
		{
			return;
		}
		Point pt0(_pt_down.x - _dst_in_frame.x, _pt_down.y - _dst_in_frame.y);

		Rect rc(pt0, pt);
		if (rc.width > 0 && rc.height > 0)
		{
			_rect_select = rc;
		}
		else
		{
			_rect_select = Rect(0, 0, 0, 0);
			_curr_rt = Rect(0, 0, 0, 0);
		}

		_pt_prev = Point(0, 0);
		_working = act_display;
	}

}

void CPaint::mouse_well_up()
{
	if (isdo_nothing())
	{
		return;
	}

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;
	bool b = is_area_dst(x, y);
	if (b)
	{
		if (_zoom_r < 4.0)
		{
			int ow = _dst_zoom_w;
			_zoom_r = get_zoom_r(0);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
			_dst_zoom_w = (int)(_lena_w * _zoom_r);

			int dx = x - _lena_in_frame.x;
			int dy = y - _lena_in_frame.y;

			_lena_in_frame.x = x - dx * _dst_zoom_w / ow;
			_lena_in_frame.y = y - dy * _dst_zoom_w / ow;

			_crop_mouseup = false;
			_working = act_display;
		}
	}
}

void CPaint::mouse_well_down()
{
	if (isdo_nothing())
	{
		return;
	}

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;
	bool b = is_area_dst(x, y);
	if (b)
	{
		if (_zoom_r > 0.05)
		{
			int ow = _dst_zoom_w;

			_zoom_r = get_zoom_r(1);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
			_dst_zoom_w = (int)(_lena_w * _zoom_r);

			if (_dst.cols < _win_w / 2 || _dst.rows < _win_h / 2)
			{
				_lena_in_frame.x = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
				_lena_in_frame.y = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;
			}
			else
			{
				int dx = x - _lena_in_frame.x;
				int dy = y - _lena_in_frame.y;

				_lena_in_frame.x = x - dx * _dst_zoom_w / ow;
				_lena_in_frame.y = y - dy * _dst_zoom_w / ow;
			}

			_crop_mouseup = false;
			_working = act_display;
		}
	}
}

void CPaint::mouse_down_r()
{
	if (isdo_nothing())
	{
		return;
	}

	if (!_clip_mat.empty())
	{
		return;
	}

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;
	if (is_area_dst(x, y))
	{
		_space_key_mouse = true;
		_pt_down = Point(x, y);

		xrect_img = _lena_in_frame.x;
		yrect_img = _lena_in_frame.y;
	}
}

void CPaint::mouse_isdown_r()
{
	if (isdo_nothing())
	{
		return;
	}

	if (!_clip_mat.empty())
	{
		return;
	}

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;
	if (is_area_dst(x, y))
	{
		int dx = x - _pt_down.x;
		int dy = y - _pt_down.y;

		int x1 = xrect_img + dx;
		int y1 = yrect_img + dy;

		if (_pt_down.x != 0 && _pt_down.y != 0 && (_lena_in_frame.x != x1 || _lena_in_frame.y != y1) &&
			x > TOOLBAR_LEFT_WIDTH + 10 && y > TOOLBAR_TOP_HEIGHT + 10 &&
			x < _win_w - 10 && y < _win_h - 10)
		{
			_lena_in_frame.x = x1;
			_lena_in_frame.y = y1;
			_working = act_display;
		}
	}
}
