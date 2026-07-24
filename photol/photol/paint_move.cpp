///////////////////////////////////////////////////////////////////////
/// @file paint_move.cpp
/// @brief PhotoNest 绘图模块 - 移动和裁剪功能实现文件
/// @details 实现鼠标移动和裁剪框交互功能:
///           - 裁剪光标获取 (get_crop_cursor)
///           - 矩形选择区域判断
///           - 鼠标移动事件处理
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的交互功能文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>

void CPaint::get_crop_cursor(Rect rc, UINT& cursor, int off)
{
	int x0 = cvui::mouse().x - _dst_in_frame.x;
	int y0 = cvui::mouse().y - _dst_in_frame.y;
	Point pt(x0, y0);

	if (x0 > 0 && x0 < TOOLBAR_LEFT_WIDTH &&
		y0 > TOOLBAR_TOP_HEIGHT && y0 < TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y * 9)
	{
		cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_ARROW));
		return;
	}

	if (_rect_select.width != 0)
	{
		if (!is_out_rect(_rect_select, pt, 20))
		{
			cursor = IDCUR_MOVE;
		}
	}

	if (_rect_crop.width != 0)
	{
		if (!is_out_rect(_rect_crop, pt, 20))
		{
			cursor = IDCUR_MOVE;
		}

		if (_crop_mouseup)
		{
			if (is_crop50(pt, _rect_crop))
			{
				cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_ARROW));
			}
		}
	}

	if (_hInputWnd != NULL)
	{
		if ((pt.x > rc.x - _input_off && pt.x < rc.x && pt.y > rc.y - _input_off && pt.y < rc.y + rc.height + _input_off) ||
			(pt.x > rc.x + rc.width && pt.x < rc.x + rc.width + _input_off && pt.y > rc.y - _input_off && pt.y < rc.y + rc.height + _input_off) ||
			(pt.y > rc.y - _input_off && pt.y < rc.y && pt.x > rc.x - _input_off && pt.x < rc.x + rc.width + _input_off) ||
			(pt.y > rc.y && pt.y < rc.y + rc.height + _input_off && pt.x > rc.x - _input_off && pt.x < rc.x + rc.width + _input_off)
			)
		{
			cursor = IDCUR_MOVE;// LOWORD(reinterpret_cast<DWORD_PTR>(IDC_ARROW));
		}
	}

	if (rc.width != 0)
	{
		if (pt.x > rc.x - off &&
			pt.x < rc.x + off &&
			pt.y > rc.y - off + rc.height / 2 &&
			pt.y < rc.y + off + rc.height / 2
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZEWE));
		}
		else if (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width &&
			pt.y > rc.y - off + rc.height / 2 &&
			pt.y < rc.y + off + rc.height / 2
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZEWE));
		}
		else if (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 &&
			pt.y > rc.y - off &&
			pt.y < rc.y + off
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENS));
		}
		else if (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 &&
			pt.y > rc.y - off + rc.height &&
			pt.y < rc.y + off + rc.height
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENS));
		}
		else if (pt.x > rc.x - off &&
			pt.x < rc.x + off &&
			pt.y > rc.y - off &&
			pt.y < rc.y + off
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENWSE));
		}
		else if (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width &&
			pt.y > rc.y - off + rc.height &&
			pt.y < rc.y + off + rc.height
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENWSE));
		}
		else if (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width &&
			pt.y > rc.y - off &&
			pt.y < rc.y + off
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENESW));
		}
		else if (pt.x > rc.x - off &&
			pt.x < rc.x + off &&
			pt.y > rc.y - off + rc.height &&
			pt.y < rc.y + off + rc.height
			)
		{
			cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_SIZENESW));
		}
	}
}

bool CPaint::mouse_down_input(Rect rc, int& pre_input_box)
{
	bool b = false;
	_for_font = false;
	_for_select = false;

	int x = cvui::mouse().x;
	int y = cvui::mouse().y;

	bool in_inp = false;
	if (x > 0 && x < TOOLBAR_LEFT_WIDTH &&
		y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y * 9)
	{
		in_inp = true;
		_is_in_combo = true;
		_rect_crop = Rect(0, 0, 0, 0);
		_rect_select = Rect(0, 0, 0, 0);
	}

	//int top3 = TOOLBAR_TOP_HEIGHT + _off_top;

	int lf4 = get_toolbar_lf();
	int top4 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
	if (x > lf4 && x < lf4 + text_draw_idle.cols &&
		y > top4 + TOOLBAR_MEN_STEP_Y * 5 && y < top4 + TOOLBAR_MEN_STEP_Y * 5 + TOOLBAR_MEN_STEP_Y)
	{
		_for_font = true;
	}

	lf4 += rect_draw_idle.cols + 5;
	if (x > lf4 && x < lf4 + text_draw_idle.cols &&
		y > top4 + TOOLBAR_MEN_STEP_Y * 6 && y < top4 + TOOLBAR_MEN_STEP_Y * 6 + TOOLBAR_MEN_STEP_Y)
	{
		_for_font = true;
	}

	int lf1 = _lf_copy - copy_idle.cols - 2;
	if (x > lf1 && x < lf1 + cut_idle.cols &&
		y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + 32 && _down_cut_click)
	{
		//cut
		_for_select = true;
	}

	if (y > TOOLBAR_BTN_TOP && y < TOOLBAR_TOP_HEIGHT)
	{
		int lf3 = _lf_copy;
		if (x > lf3 - copy_idle.cols + 2 && x < lf3 - copy_idle.cols + 2 + 32)
		{
			//copy
			_for_select = true;
		}

		if (x > lf3 && x < lf3 + down_idle.cols)
		{
			//down
			_is_in_combo = true;
		}

		if (x > _lf_move + TOOLBAR_BTN_STEP_X + paste_idle.cols + 2 &&
			x < _lf_move + TOOLBAR_BTN_STEP_X + paste_idle.cols + 2 + down_idle.cols)
		{
			//paste
			_is_in_combo = true;
		}
	}

	if (_hInputWnd != NULL)
	{
		Point pt;
		pt.x = x - _dst_in_frame.x;
		pt.y = y - _dst_in_frame.y;

		if (pt.x > rc.x - _input_off && pt.x < rc.x + rc.width + _input_off &&
			pt.y > rc.y - _input_off && pt.y < rc.y + rc.height + _input_off && !in_inp)
		{
			SetForegroundWindow(_hInputWnd);
			SetFocus(GetDlgItem(_hInputWnd, IDC_EDIT1));
			pre_input_box = 1;
		}
		else
		{
			_tool_btn = toolbar_input;
			pre_input_box = in_inp ? 1 : 2;
			b = true;
		}
	}

	return b;
}

void CPaint::mouse_down_init(Rect& rc, Point& prev_pt)
{
	int x = cvui::mouse().x;
	int y = cvui::mouse().y;

	if (rc.width != 0)
	{
		if (x > TOOLBAR_LEFT_WIDTH && x < _win_w && y > TOOLBAR_BTN_TOP && y < _win_h)
		{
			int x0 = cvui::mouse().x - _dst_in_frame.x;
			int y0 = cvui::mouse().y - _dst_in_frame.y;
			Point pt(x0, y0);
			if (is_out_rect(rc, pt, 20))
			{
				rc = Rect(0, 0, 0, 0);
				prev_pt = Point(0, 0);
			}
		}
	}
}

bool CPaint::mouse_isdown_paste(Rect& rc, Point& prev_pt)
{
	bool b = false;
	if (rc.width != 0)
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);
		if (_drag_rc_type == 9 || at_rect(pt, _rect_paste))
		{
			if (prev_pt.x == 0)
			{
				prev_pt = pt;
				_drag_rc_type = 9;
			}
			else
			{
				b = proc_target_rc(rc, 0, pt, prev_pt);
				if (b)
				{
					prev_pt = Point(0, 0);
					_drag_rc_type = 0;
				}
				else
				{
					prev_pt = pt;
				}

				_working = act_display;
			}
			return true;
		}
	}
	return false;
}

bool CPaint::mouse_isdown_input(Rect& rc, Point& prev_pt, int off, int off1)
{
	bool b = false;
	if (rc.width != 0)
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;

		Point pt(x0, y0);

		if (_drag_rc_type == 1 || (pt.x > rc.x - off && pt.x < rc.x + off &&
			pt.y > rc.y - off + rc.height / 2 && pt.y < rc.y + off + rc.height / 2))
		{
			if (pt.x >= 0)
			{
				if (pt.x + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
				{
					rc.x = TOOLBAR_LEFT_WIDTH + 10 - _dst_in_frame.x;
				}
				else
				{
					rc.width += rc.x - pt.x;
					rc.x = pt.x;
				}
			}
			_drag_rc_type = 1;
			b = true;
		}
		else if (_drag_rc_type == 2 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off + rc.height / 2 && pt.y < rc.y + off + rc.height / 2))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}
			_drag_rc_type = 2;
			b = true;
		}
		else if (_drag_rc_type == 3 || (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}
			_drag_rc_type = 3;
			b = true;
		}
		else if (_drag_rc_type == 4 || (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}
			_drag_rc_type = 4;
			b = true;
		}
		else if (_drag_rc_type == 5 || (pt.x > rc.x - off &&
			pt.x < rc.x + off && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (pt.x >= 0)
			{
				if (pt.x + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
				{
					rc.x = TOOLBAR_LEFT_WIDTH + 10 - _dst_in_frame.x;
				}
				else
				{
					rc.width += rc.x - pt.x;
					rc.x = pt.x;
				}
			}
			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}
			_drag_rc_type = 5;
			b = true;
		}
		else if (_drag_rc_type == 6 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}

			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}

			_drag_rc_type = 6;
			b = true;
		}
		else if (_drag_rc_type == 7 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}


			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}

			_drag_rc_type = 7;
			b = true;
		}
		else if (_drag_rc_type == 8 || (pt.x > rc.x - off &&
			pt.x < rc.x + off && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (pt.x >= 0)
			{
				if (pt.x + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
				{
					rc.x = TOOLBAR_LEFT_WIDTH + 10 - _dst_in_frame.x;
				}
				else
				{
					rc.width += rc.x - pt.x;
					rc.x = pt.x;
				}
			}

			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}

			_drag_rc_type = 8;
			b = true;
		}

		if (b)
		{
			_working = act_display;
			return b;
		}

		if (_drag_rc_type == 9 || (pt.x > rc.x - off1 &&
			pt.x < rc.x + rc.width + off1 && pt.y >  rc.y - off1 && pt.y < rc.y + rc.height + off1))
		{
			if (prev_pt.x == 0)
			{
				prev_pt = pt;
				_drag_rc_type = 9;
			}
			else
			{
				int xx = rc.x + pt.x - prev_pt.x;
				int yy = rc.y + pt.y - prev_pt.y;


				rc.x = xx;
				rc.y = yy;

				if (_dst_in_frame.x > TOOLBAR_LEFT_WIDTH && xx < 0)
				{
					rc.x = 0;
				}
				else if (xx + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
				{
					rc.x = TOOLBAR_LEFT_WIDTH + 10 - _dst_in_frame.x;
				}

				if (xx + rc.width > _dst.cols)
				{
					rc.x = _dst.cols - rc.width;
				}

				if (yy < 0)
				{
					rc.y = 0;
				}
				if (yy + rc.height > _dst.rows)
				{
					rc.y = _dst.rows - rc.height;
				}

				prev_pt = pt;
				_working = act_display;
				return true;
			}
		}
	}
	return false;
}

bool CPaint::mouse_isdown_crop(Rect& rc, Point& prev_pt, int off)
{
	bool b = false;
	if (rc.width != 0)
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);

		if (_crop_mouseup)
		{
			if (is_crop50(pt, rc))
			{
				crop_ok(rc, prev_pt);

				_rect_crop = Rect(0, 0, 0, 0);
				_crop_mouseup = true;

				_working = act_display;
				return true;
			}
		}

		if (_drag_rc_type == 1 || (pt.x > rc.x - off &&
			pt.x < rc.x + off && pt.y > rc.y - off + rc.height / 2 && pt.y < rc.y + off + rc.height / 2))
		{
			if (pt.x >= 0)
			{
				rc.width += rc.x - pt.x;
				rc.x = pt.x;
			}
			_drag_rc_type = 1;
			b = true;
		}
		else if (_drag_rc_type == 2 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off + rc.height / 2 && pt.y < rc.y + off + rc.height / 2))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}
			_drag_rc_type = 2;
			b = true;
		}
		else if (_drag_rc_type == 3 || (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}
			_drag_rc_type = 3;
			b = true;
		}
		else if (_drag_rc_type == 4 || (pt.x > rc.x - off + rc.width / 2 &&
			pt.x < rc.x + off + rc.width / 2 && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}
			_drag_rc_type = 4;
			b = true;
		}
		else if (_drag_rc_type == 5 || (pt.x > rc.x - off &&
			pt.x < rc.x + off && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (pt.x >= 0)
			{
				rc.width += rc.x - pt.x;
				rc.x = pt.x;
			}
			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}
			_drag_rc_type = 5;
			b = true;
		}
		else if (_drag_rc_type == 6 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}

			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}

			_drag_rc_type = 6;
			b = true;
		}
		else if (_drag_rc_type == 7 || (pt.x > rc.x - off + rc.width &&
			pt.x < rc.x + off + rc.width && pt.y > rc.y - off && pt.y < rc.y + off))
		{
			if (rc.width + rc.x < _dst.cols)
			{
				rc.width -= rc.width + rc.x - pt.x;
			}
			if (rc.width + rc.x > _dst.cols)
			{
				rc.width = _dst.cols - rc.x;
			}

			if (pt.y >= 0)
			{
				rc.height += rc.y - pt.y;
				rc.y = pt.y;
			}

			_drag_rc_type = 7;
			b = true;
		}
		else if (_drag_rc_type == 8 || (pt.x > rc.x - off &&
			pt.x < rc.x + off && pt.y > rc.y - off + rc.height && pt.y < rc.y + off + rc.height))
		{
			if (pt.x >= 0)
			{
				rc.width += rc.x - pt.x;
				rc.x = pt.x;
			}

			if (rc.height + rc.y < _dst.rows)
			{
				rc.height -= rc.height + rc.y - pt.y;
			}
			if (rc.height + rc.y > _dst.rows)
			{
				rc.height = _dst.rows - rc.y;
			}

			_drag_rc_type = 8;
			b = true;
		}

		if (b)
		{
			_working = act_display;
			return b;
		}

		if (_drag_rc_type == 9 || !is_out_rect(rc, pt, 20))
		{
			if (prev_pt.x == 0)
			{
				prev_pt = pt;
				_drag_rc_type = 9;
			}
			else
			{
				int offset = (_opt == btn_crop) ? 100 : 0;
				b = proc_target_rc(rc, offset, pt, prev_pt);
				if (b)
				{
					prev_pt = Point(0, 0);
				}
				else
				{
					prev_pt = pt;
				}
			}
			b = true;
		}

		if (_opt == btn_crop)
		{
			int xx = rc.x + pt.x - prev_pt.x;
			int yy = rc.y + pt.y - prev_pt.y;

			rc.x = xx;
			rc.y = yy;

			bool b1 = proc_crop_offset(rc, pt);
			if (b1)
			{
				b = true;
			}
		}
	}

	if (b)
	{
		_working = act_display;
	}
	return b;
}
