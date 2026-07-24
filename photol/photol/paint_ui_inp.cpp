///////////////////////////////////////////////////////////////////////
/// @file paint_ui_inp.cpp
/// @brief 绘图模块 - 输入界面处理实现
/// @details 实现保存另存为对话框、输入界面按钮渲染、
///          用户交互处理等 UI 输入相关功能
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理绘图应用的文件保存和输入界面逻辑
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

void CPaint::nav_saveas_ui(int lf, bool b0)
{
	Mat idle = save_idle, over = save_over, gray = save_gray;
	Mat as_idle = saveas_idle, as_over = saveas_over;

	int top3 = TOOLBAR_BTN_TOP - 2;
	if (_app_type == FROM_EXE)
	{
		idle = save_ico_idle;
		over = save_ico_over;
		gray = save_ico_gray;

		as_idle = saveas_ico_idle;
		as_over = saveas_ico_over;
		top3 = TOOLBAR_BTN_TOP;
	}

	bool b1 = canSave() && _close_ret == 0 && !b0;
	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP, b1 ? idle : gray, b1 ? over : gray, b1 ? over : gray))
	{
		if (!_inpaint_click && b1)
		{
			bool b = show_unapplied();
			if (!b)
			{
				_tool_btn = toolbar_save;
			}
		}
	}
	_lf_saveas_down = lf + idle.cols + 2;

	if (cvui::button(_frame, _lf_saveas_down, top3,
		(_lena2.empty() || _close_ret >= 1 || b0) ? down_gray : (_down_saveas_click ? down_over : down_idle),
		(_lena2.empty() || _close_ret >= 1 || b0) ? down_gray : down_over, (_lena2.empty() || _close_ret >= 1 || b0) ? down_gray : down_over))
	{
		if (!_inpaint_click && !_lena2.empty() && !b0)
		{
			_down_saveas_click = !_down_saveas_click;
			_working = act_tips_ui;
		}
	}

	if (_down_saveas_click && _close_ret == 0)
	{
		_down_cut_click = false;
		_down_sticker_click = false;

		int lf2 = lf - saveas_idle.cols;		if (_app_type != FROM_EXE) {
			int lf0 = _win_w + TOOLBAR_RIGHT_WIDTH - 80;
			lf2 = lf0 - saveas_idle.cols;
		}
		lf2 = _lf_saveas_down - saveas_idle.cols + down_over.cols;
		if (cvui::button(_frame, lf2, TOOLBAR_TOP_HEIGHT, as_idle, as_over, as_over))
		{
			if (!_inpaint_click && !_lena2.empty())
			{
				_down_saveas_click = false;

				bool b = show_unapplied();
				if (!b)
				{
					_tool_btn = bth_saveas;
				}

			}
			_working = act_display;
		}
	}
}

void CPaint::navbar_state_ui(int x, int y, string mouse)
{
	if (_down_nav_click)
	{
		int top3 = TOOLBAR_TOP_HEIGHT;
		int step = MENU_BTN_STEP;

		if (x > _lf_nav && x< _lf_nav + _combo_width_nav && y>top3 && y < top3 + step * 5)
		{
			return;
		}

		int ty = 1;
		if (mouse == "up")
		{
			ty = 2;
		}

		_navbar_state = 0;
		if (y < TOOLBAR_TOP_HEIGHT)
		{
			if (x > _lf_nav && x < _lf_nav + _lbl_nav.cols + 32)
			{
			}
			else
			{
				if (mouse == "down")
				{
					_down_nav_click = false;
				}
			}
		}
		else
		{
			int step = MENU_BTN_STEP;

			int lf3 = TOOLBAR_SUB_OFF + _left_width;
			int top3 = TOOLBAR_TOP_HEIGHT + _off_top;

			if (y > top3 && y < _win_h)
			{
				if (_nav_btn == nav_draw && x < lf3 + _combo_width_inp)
				{
					_navbar_state = ty;
				}
				else if (_nav_btn == nav_segment && x < lf3 + _combo_width_seg)
				{
					_navbar_state = ty;
				}
				else if (_nav_btn == nav_sticker && x < lf3 + _combo_width_sticker)
				{
					_navbar_state = ty;
				}
				else if (_nav_btn == nav_filter && x < lf3 + _combo_width_filter)
				{
					_navbar_state = ty;
				}
				else if (_nav_btn == nav_inp && x < lf3 + _combo_width_inp)
				{
					_navbar_state = ty;
				}
				else
				{
					if (mouse == "down")
					{
						_down_nav_click = false;
					}
				}
			}
			else
			{
				if (mouse == "down")
				{
					_down_nav_click = false;
				}
			}
		}
	}
}

void CPaint::nav_saveas_is_in_combo(int x, int y, bool& cbo)
{
	if (_app_type != FROM_EXE)
	{
		if (x > _win_w + TOOLBAR_RIGHT_WIDTH - close_gray.cols - 15 && x < _win_w && y > 5 && y < 5 + close_gray.rows)
		{
			cbo = true;
		}
	}

	if (_app_type == FROM_EXE)
	{
		int lf = _lf_saveas_down;
		if (x > lf && x < lf + down_idle.cols && y > TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
		{
			cbo = true;
		}
		if (_down_saveas_click)
		{
			lf = _lf_saveas_down - saveas_ico_idle.cols + down_over.cols;
			//	lf = _lf_saveas_down - copy_idle.cols - 2;
			if (x > lf && x < lf + saveas_ico_idle.cols && y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + saveas_ico_idle.rows)
			{
				cbo = true;
			}

			lf = _lf_saveas_down;
			if (x > lf && x < lf + down_idle.cols && y > TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
			{
			}
			else
			{
				if (!cbo)
				{
					_down_saveas_click = false;
				}
				_working = act_display;
			}
		}
		/*
				lf = _lf_saveas_down;
				if (x > lf && x < lf + down_idle.cols && y > TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
				{
					cbo = true;
				}
				*/
	}
	else
	{
		int lf = _win_w + TOOLBAR_RIGHT_WIDTH - 80;

		if (x > lf - save_idle.cols - down_idle.cols - 2 && x < lf - down_idle.cols - 2 && y> TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + save_idle.rows)
		{
			cbo = true;
		}

		if (x > lf - down_idle.cols && x < lf && y> TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
		{
			cbo = true;
		}
		lf = _lf_saveas_down - saveas_idle.cols + down_over.cols;

		if (x > lf && x < lf + saveas_idle.cols && y> TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + saveas_idle.rows)
		{
			cbo = true;
		}

		if (_down_saveas_click)
		{
			if (x > lf - down_idle.cols && x < lf && y> TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
			{
			}
			else
			{
				if (!cbo)
				{
					_down_saveas_click = false;
				}
				_working = act_display;
			}
		}
	}

	if (_down_cut_click)
	{
		int lf1 = _lf_copy - +copy_idle.cols - 2;
		if (x > lf1 && x < lf1 + cut_idle.cols && y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + 32 && _down_cut_click)
		{
			cbo = true;
		}

		lf1 = _lf_copy;
		if (x > lf1 && x < lf1 + down_idle.cols && y > TOOLBAR_BTN_TOP && y < TOOLBAR_BTN_TOP + down_idle.rows)
		{
		}
		else
		{
			if (!cbo)
			{
				_down_cut_click = false;
			}
			_working = act_display;
		}
	}
}

void CPaint::nav_draw_ui()
{
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
	int off = 5;

	int lf3 = get_toolbar_lf();

	_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	if (cvui::button(_frame, lf3, top3, _opt == btn_draw_free ? free_draw_over : free_draw_idle, free_draw_over, free_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_free);
		}
	}

	if (cvui::button(_frame, lf3 + free_draw_idle.cols + off, top3, _opt == btn_draw_select ? draw_select_over : draw_select_idle, draw_select_over, draw_select_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_select);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_draw_straight ? line_draw_over : line_draw_idle, line_draw_over, line_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_straight);
		}
	}

	if (cvui::button(_frame, lf3 + line_draw_idle.cols + off, top3, _opt == btn_draw_arrow ? arrow_draw_over : arrow_draw_idle, arrow_draw_over, arrow_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_arrow);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_draw_rect ? rect_draw_over : rect_draw_idle, rect_draw_over, rect_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_rect);
		}
	}

	if (cvui::button(_frame, lf3 + rect_draw_idle.cols + off, top3, _opt == btn_draw_circle ? circle_draw_over : circle_draw_idle, circle_draw_over, circle_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_circle);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, (_opt == btn_draw_picker) ? picker_draw_over : picker_draw_idle, picker_draw_over, picker_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			if (_opt >= btn_draw_free && _opt <= btn_draw_select)
			{
				_last_draw_opt = _opt;
			}
			else
			{
				_last_draw_opt = btn_draw_picker;
			}
			select_btn2(btn_draw_picker);
		}
	}

	if (cvui::button(_frame, lf3 + free_draw_idle.cols + off, top3, _opt == btn_draw_fill ? fill_draw_over : fill_draw_idle, fill_draw_over, fill_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_fill);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_draw_eraser ? draw_eraser_over : draw_eraser_idle, draw_eraser_over, draw_eraser_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_eraser);
		}
	}

	if (cvui::button(_frame, lf3 + free_draw_idle.cols + off, top3, _opt == btn_draw_mosaic ? mosaic_draw_over : mosaic_draw_idle, mosaic_draw_over, mosaic_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_mosaic);
		}
	}

	Mat tt = color_draw_idle.clone();
	make_draw_ico(tt, _picker_val);
	make_over(tt, color_draw_over);

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, tt, color_draw_over, tt))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			if (_opt == btn_draw_text)
			{
				_tool_btn = btn_font;
			}
			else
			{
				_tool_btn = btn_draw_color;
			}
			_working = act_display;
		}
	}

	if (cvui::button(_frame, lf3 + color_draw_idle.cols + off, top3, _opt == btn_draw_text ? text_draw_over : text_draw_idle, text_draw_over, text_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_text);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_draw_magic ? inp_magic_over : inp_magic_idle, inp_magic_over, inp_magic_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_magic);
		}
	}

	if (_opt == btn_draw_text)
	{
		//if (cvui::button(_frame, lf3 + inp_magic_idle.cols + off, top3,
		//	(_input_bg_trans != 0) ? input_trans_idle : input_opaque_idle,
		//	(_input_bg_trans != 0) ? input_trans_over : input_opaque_over, (_input_bg_trans != 0) ? input_trans_over : input_opaque_over))
		//{
		//	if (!_inpaint_click && !_lena2.empty())
		//	{
		//		_tool_btn = btn_input_bg;
		//		_working = act_display;
		//	}
		//}
		if (cvui::button(_frame, lf3 + inp_magic_idle.cols + off, top3, input_trans_idle, input_trans_over, input_trans_over))
		{
			if (!_inpaint_click && !_lena2.empty())
			{
				_tool_btn = btn_input_bg;
				_working = act_display;
			}
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, (_opt == btn_crop_bar || _opt == btn_crop) ? crop_custom_over : crop_custom_idle, crop_custom_over, crop_custom_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			if (_opt == btn_crop_bar)
			{
				_opt = 0;
				_working = act_tips_ui;
			}
			else
			{
				select_btn2(btn_crop_bar);
			}
			_pointer_btn = btn_move;
		}
	}

	if (cvui::button(_frame, lf3 + crop_custom_idle.cols + off, top3, (_opt == btn_rota_bar) ? rota270_over : rota270_idle, rota270_over, rota270_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			if (_opt == btn_rota_bar)
			{
				_opt = 0;
				_working = act_tips_ui;
			}
			else
			{
				select_btn2(btn_rota_bar);
			}

			_pointer_btn = btn_move;
		}
	}
}

void CPaint::nav_sticker_ui()
{
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;// * 3;
	int off = 5;

	int lf3 = get_toolbar_lf();

	_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	if (cvui::button(_frame, lf3 + tips_idle.cols + off, top3, _lena2.empty() ? select_sticker_gray : select_sticker_idle,
		_lena2.empty() ? select_sticker_gray : select_sticker_over, _lena2.empty() ? select_sticker_gray : select_sticker_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			open_sticker();
		}
	}
	if (cvui::button(_frame, lf3, top3, tips_idle, tips_over, tips_over))
	{
	}

	//top3 += TOOLBAR_MEN_STEP_Y;
	//if (cvui::button(_frame, lf3, top3, _inpaint_click || _sticker_arr.empty() ? done_gray : done_idle,
	//	_inpaint_click || _sticker_arr.empty() ? done_gray : done_over, _inpaint_click || _sticker_arr.empty() ? done_gray : done_over))
	//{
	//	if (!_inpaint_click && !_sticker_arr.empty())
	//	{
	//		int k = get_thread_idx();
	//		threads[k].stop = false;
	//		threads[k].opt = btn_sticker_ok;
	//		_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_sticker, this, NULL, NULL);
	//		threads[k].hdl = _handle_thread;

	//		_opt = 0;
	//		_last_opt = 0;
	//		_pointer_btn = btn_move;
	//	}
	//}
}

void CPaint::nav_filter_ui(z_filter& fVal)
{
	int lf3 = TOOLBAR_SUB_OFF + _left_width;
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;

	lf3 = get_toolbar_lf();

	_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	int	top = top3;// + 20;
	int left = lf3 + 5;
	int off1 = 0;
	int off2 = 27;

	unsigned int theColor = 0xCECECE;
	if (_lena2.empty())
	{
		theColor = 0;
	}

	int hoff = 30;//TOOLBAR_MEN_STEP_Y
	//3
	//top += 10;
	cvui::checkbox(_frame, left, top, "", &fVal.use_decolor, theColor);
	if (fVal.curr_use_decolor != fVal.use_decolor)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_decolor = fVal.use_decolor;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_decolor, lbl_decolor, lbl_decolor))
	{
		if (!_lena2.empty())
		{
			fVal.use_decolor = !fVal.use_decolor;
			_filter_changed = true;
		}
	}

	//3
	top += hoff;
	cvui::checkbox(_frame, left, top, "", &fVal.use_comic_strip, theColor);
	if (fVal.curr_use_comic_strip != fVal.use_comic_strip)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_comic_strip = fVal.use_comic_strip;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_comic_strip, lbl_comic_strip, lbl_comic_strip))
	{
		if (!_lena2.empty())
		{
			fVal.use_comic_strip = !fVal.use_comic_strip;
			_filter_changed = true;
		}
	}

	//--------------
	//4
	top += hoff;
	cvui::checkbox(_frame, left, top, "", &fVal.use_vintage, theColor);
	if (fVal.curr_use_vintage != fVal.use_vintage)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_vintage = fVal.use_vintage;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_vintage, lbl_vintage, lbl_vintage))
	{
		if (!_lena2.empty())
		{
			fVal.use_vintage = !fVal.use_vintage;
			_filter_changed = true;
		}
	}

	//5
	top += hoff;
	cvui::checkbox(_frame, left, top, "", &fVal.use_feather, theColor);
	if (fVal.curr_use_feather != fVal.use_feather)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_feather = fVal.use_feather;
			_filter_changed = true;
		}
	}
	if (cvui::button(_frame, left + 29, top - off1, lbl_feather, lbl_feather, lbl_feather))
	{
		if (!_lena2.empty())
		{
			fVal.use_feather = !fVal.use_feather;
			_filter_changed = true;
		}
	}

	//6
	top += hoff;
	cvui::checkbox(_frame, left, top, "", &fVal.use_sketch, theColor);
	if (fVal.curr_use_sketch != fVal.use_sketch)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_sketch = fVal.use_sketch;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_sketch, lbl_sketch, lbl_sketch))
	{
		if (!_lena2.empty())
		{
			fVal.use_sketch = !fVal.use_sketch;
			_filter_changed = true;
		}
	}
	//if (cvui::trackbar(_frame, left + _combo_width_filter, top - off2, 100, &fVal.bkLightValue, 1, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS))
	//{
	//	if (fVal.use_sketch)
	//	{
	//		_filter_changed = true;
	//	}
	//}
	//1
	top += hoff;
	cvui::checkbox(_frame, left, top, "", &fVal.use_brightValue, theColor);
	if (fVal.curr_use_brightValue != fVal.use_brightValue)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_brightValue = fVal.use_brightValue;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_brightness, lbl_brightness, lbl_brightness))
	{
		if (!_lena2.empty())
		{
			fVal.use_brightValue = !fVal.use_brightValue;
			_filter_changed = true;
		}
	}
	top += hoff + 20;
	if (cvui::trackbar(_frame, left, top - off2, 100, &fVal.brightValue, -100, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS))
	{
		if (!_lena2.empty())
		{
			if (fVal.use_brightValue)
			{
				_filter_changed = true;
			}
		}
	}

	//2
	top += hoff + 10;
	cvui::checkbox(_frame, left, top, "", &fVal.use_contrastValue, theColor);
	if (fVal.curr_use_contrastValue != fVal.use_contrastValue)
	{
		if (!_lena2.empty())
		{
			fVal.curr_use_contrastValue = fVal.use_contrastValue;
			_filter_changed = true;
		}
	}

	if (cvui::button(_frame, left + 29, top - off1, lbl_contrast, lbl_contrast, lbl_contrast))
	{
		if (!_lena2.empty())
		{
			fVal.use_contrastValue = !fVal.use_contrastValue;
			_filter_changed = true;
		}
	}

	top += hoff + 20;
	if (cvui::trackbar(_frame, left, top - off2, 100, &fVal.contrastValue, -100, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS))
	{
		if (!_lena2.empty())
		{
			if (fVal.use_contrastValue)
			{
				_filter_changed = true;
			}
		}
	}

	top += hoff + 10;
	if (cvui::button(_frame, left - 5, top, sky_draw_idle, sky_draw_over, sky_draw_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			change_sky();
		}
	}
}

void CPaint::nav_inp_ui(bool canInp)
{
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;;
	int off = 5;

	int opt_type = 0;
	if (!_opt_arr.empty())
	{
		int len = (int)_opt_arr.size();
		for (int i = 0; i < len; i++)
		{
			opt_type = _opt_arr[i].opt_type;
		}
	}

	int lf3 = get_toolbar_lf();

	_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	if (cvui::button(_frame, lf3 + tips_idle.cols + off, top3, _opt == btn_brush ? inp_brush_over : inp_brush_idle, inp_brush_over, inp_brush_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn(btn_brush);
		}
	}

	if (cvui::button(_frame, lf3, top3, tips_idle, tips_over, tips_over))
	{
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_free_select ? inp_select_over : inp_select_idle, inp_select_over, inp_select_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn(btn_free_select);
		}
	}

	if (cvui::button(_frame, lf3 + inp_select_idle.cols + off, top3, _opt == btn_rect ? inp_rect_over : inp_rect_idle, inp_rect_over, inp_rect_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn(btn_rect);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, _opt == btn_magic ? inp_magic_over : inp_magic_idle, inp_magic_over, inp_magic_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn(btn_magic);
		}
	}

	if (cvui::button(_frame, lf3 + inp_magic_idle.cols + off, top3, canInp ? (_opt == btn_eraser ? inp_eraser_over : inp_eraser_idle) : inp_eraser_gray,
		canInp ? inp_eraser_over : inp_eraser_gray, canInp ? inp_eraser_over : inp_eraser_gray))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn(btn_eraser);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, opt_type == inpaint_ok ? inp_optimize_idle : inp_optimize_gray,
		opt_type == inpaint_ok ? inp_optimize_over : inp_optimize_gray, opt_type == inpaint_ok ? inp_optimize_over : inp_optimize_gray))
	{
		if (opt_type == inpaint_ok && !_inpaint_click)
		{
			_pointer_btn = btn_move;
			_opt = btn_optimize;
			_last_opt = _opt;

			int k = get_thread_idx();
			threads[k].stop = false;
			_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_inpaint, this, NULL, NULL);
			threads[k].hdl = _handle_thread;
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, (_inpaint_click || !canInp) ? done_inp_gray : done_inp_idle, (_inpaint_click || !canInp) ? done_inp_gray : done_inp_over, (_inpaint_click || !canInp) ? done_inp_gray : done_inp_over))
	{
		if (_inpaint_click || !canInp)
		{

		}
		else
		{
			int rows = _inpaintMask.rows;
			int cols = _inpaintMask.cols;
			int z = countNonZero(_inpaintMask);
			if (z == 0)
			{

			}
			else if (z * 3 > rows * cols)
			{
				_show_err_type = 4;
				_disp_nb = 1;
			}
			else
			{
				_pointer_btn = btn_move;
				_opt = btn_inpaint;
				_last_opt = _opt;

				int k = get_thread_idx();
				threads[k].stop = false;
				_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_inpaint, this, NULL, NULL);
				threads[k].hdl = _handle_thread;
			}
		}
	}
}

void CPaint::nav_crop_ui()
{
	int off = 5;
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;

	int lf3 = get_toolbar_lf() / 2 + _combo_width_draw;

	Mat roi = _frame(Rect(lf3, top3, (crop_custom_idle.cols + off) * 7, crop_custom_idle.rows + 4));
	roi = Scalar(43, 43, 43);

	if (cvui::button(_frame, lf3, top3, (_crop_opt == crop_0_0) ? crop_custom_over : crop_custom_idle, crop_custom_over, crop_custom_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_0_0;
		}
	}

	if (cvui::button(_frame, lf3 + crop_custom_idle.cols + off, top3, (_crop_opt == crop_1_1) ? crop1_1_over : crop1_1, crop1_1_over, crop1_1_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_1_1;
		}
	}

	if (cvui::button(_frame, lf3 + (crop_custom_idle.cols + off) * 2, top3, (_crop_opt == crop_3_2) ? crop3_2_over : crop3_2, crop3_2_over, crop3_2_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_3_2;
		}
	}

	if (cvui::button(_frame, lf3 + (crop_custom_idle.cols + off) * 3, top3, (_crop_opt == crop_4_3) ? crop4_3_over : crop4_3, crop4_3_over, crop4_3_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_4_3;
		}
	}

	if (cvui::button(_frame, lf3 + (crop_custom_idle.cols + off) * 4, top3, (_crop_opt == crop_5_4) ? crop5_4_over : crop5_4, crop5_4_over, crop5_4_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_5_4;
		}
	}

	if (cvui::button(_frame, lf3 + (crop_custom_idle.cols + off) * 5, top3, (_crop_opt == crop_7_5) ? crop7_5_over : crop7_5, crop7_5_over, crop7_5_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_7_5;
		}
	}

	if (cvui::button(_frame, lf3 + (crop_custom_idle.cols + off) * 6, top3, (_crop_opt == crop_16_9) ? crop16_9_over : crop16_9, crop16_9_over, crop16_9_over))
	{
		if (!_inpaint_click)
		{
			select_btn2(btn_crop);
			_crop_opt = crop_16_9;
		}
	}
}

void CPaint::nav_rota_ui()
{
	int off = 5;
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;

	int lf3 = get_toolbar_lf() / 2 + _combo_width_draw;

	Mat roi = _frame(Rect(lf3, top3, (crop_custom_idle.cols + off) * 5, rota270_idle.rows + 4));
	roi = Scalar(43, 43, 43);

	if (cvui::button(_frame, lf3, top3, rota270_idle, rota270_over, rota270_over))
	{
		if (!_inpaint_click)
		{
			_angleValue = -90;
			_angleValue_change = 1;
		}
	}

	if (cvui::button(_frame, lf3 + rota270_idle.cols + off, top3, rota90_idle, rota90_over, rota90_over))
	{
		if (!_inpaint_click)
		{
			_angleValue = 90;
			_angleValue_change = 1;
		}
	}

	if (cvui::button(_frame, lf3 + (rota270_idle.cols + off) * 2, top3, rota180_idle, rota180_over, rota180_over))
	{
		if (!_inpaint_click)
		{
			_angleValue = 180;
			_angleValue_change = 1;
		}
	}

	if (cvui::button(_frame, lf3 + (rota270_idle.cols + off) * 3, top3, fliph_idle, fliph_over, fliph_over))
	{
		if (!_inpaint_click)
		{
			_opt_rotate = btn_fliph;
		}
	}

	if (cvui::button(_frame, lf3 + (rota270_idle.cols + off) * 4, top3, flipv_idle, flipv_over, flipv_over))
	{
		if (!_inpaint_click)
		{
			_opt_rotate = btn_flipv;
		}
	}
}

void CPaint::nav_seg_ui()
{
	int last_opt_type = 0;
	if (!_opt_arr.empty())
	{
		int len = (int)_opt_arr.size();
		if (_idx > 0 && _idx <= len)
		{
			last_opt_type = _opt_arr[_idx - 1].opt_type;
		}
	}

	int off = 5;
	int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
	int lf3 = get_toolbar_lf();

	_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	bool b0 = false;
	b0 = (last_opt_type == btn_change_bg ? true : b0);
	b0 = (_inpaint_click ? true : b0);
	if (cvui::button(_frame, lf3 + tips_idle.cols + off, top3, b0 ? seg_rect_gray : (_opt == btn_seg_rect ? seg_rect_over : seg_rect_idle),
		b0 ? seg_rect_gray : seg_rect_over, b0 ? seg_rect_gray : seg_rect_over))
	{
		if (!b0 && !_lena2.empty())
		{
			select_btn(btn_seg_rect);
		}
	}

	if (cvui::button(_frame, lf3, top3, tips_idle, tips_over, tips_over))
	{
	}

	bool b2 = (_result.empty());
	b2 = (last_opt_type == btn_change_bg ? true : b2);
	b2 = (_inpaint_click ? true : b2);

	top3 += TOOLBAR_MEN_STEP_Y;
	if (cvui::button(_frame, lf3, top3, b2 ? gc_fgd_gray : (_opt == btn_seg_fgd ? gc_fgd_over : gc_fgd_idle),
		b2 ? gc_fgd_gray : gc_fgd_over, b2 ? gc_fgd_gray : gc_fgd_over))
	{
		if (!b2 && !_lena2.empty())
		{
			select_btn(btn_seg_fgd);
		}
	}

	if (cvui::button(_frame, lf3 + gc_fgd_idle.cols + off, top3, b2 ? gc_bgd_gray : (_opt == btn_seg_bgd ? gc_bgd_over : gc_bgd_idle),
		b2 ? gc_bgd_gray : gc_bgd_over, b2 ? gc_bgd_gray : gc_bgd_over))
	{
		if (!b2 && !_lena2.empty())
		{
			select_btn(btn_seg_bgd);
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y;

	if (cvui::button(_frame, lf3, top3, /*_opt == btn_draw_magic ? inp_magic_over :*/ inp_magic_idle, inp_magic_over, inp_magic_over))
	{
		if (!_inpaint_click && !_lena2.empty())
		{
			select_btn2(btn_draw_magic);
		}
	}

	//seg_start_gray
	top3 += TOOLBAR_MEN_STEP_Y;
	bool b3 = (_inpaint_click || (_rect_seg_select.width < 10 || _rect_seg_select.height < 10));

	if (cvui::button(_frame, lf3, top3, b3 ? done_gray : done_idle, b3 ? done_gray : done_over, b3 ? done_gray : done_over))
	{
		if (!b3)
		{
			if (!_seg_mask.empty())
			{
				grabcut_apply();
			}
			_working = act_display;

			_opt = 0;
			_last_opt = 0;
			_pointer_btn = btn_move;
		}
	}

	top3 += TOOLBAR_MEN_STEP_Y + TOOLBAR_MEN_STEP_Y / 2 - 5;
	line(_frame, Point(lf3 + 4, top3), Point(_combo_width_draw, top3), Scalar::all(54), 1, 8, 0);

	top3 += TOOLBAR_MEN_STEP_Y / 2 + 5;

	bool b5 = true;

	b5 = (!_eraser_mask.empty() ? false : b5);
	b5 = (last_opt_type == btn_seg_apply ? false : b5);
	b5 = (last_opt_type == btn_change_bg ? false : b5);
	b5 = (_inpaint_click ? true : b5);
	b5 = (!b2 ? true : b5);

	if (cvui::button(_frame, lf3, top3, b5 ? select_bg_gray : select_bg_idle, b5 ? select_bg_gray : select_bg_over,
		b5 ? select_bg_gray : select_bg_over))
	{
		if (!b5)
		{
			change_bg();
		}
	}

}

void CPaint::get_toolbar_rc(Rect& rc)
{
	rc = Rect(0, 0, 1, 1);
	if (!_down_nav_click)
	{
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
		int lf3 = get_toolbar_lf();

		if (_nav_btn == nav_inp)
		{
			rc = Rect(lf3, top3, TOOLBAR_LEFT_WIDTH - lf3, TOOLBAR_MEN_STEP_Y * 5);
		}
		else if (_nav_btn == nav_segment)
		{
			rc = Rect(lf3, top3, TOOLBAR_LEFT_WIDTH - lf3, TOOLBAR_MEN_STEP_Y * 6);
		}
		else if (_nav_btn == nav_sticker)
		{
			rc = Rect(lf3, top3, TOOLBAR_LEFT_WIDTH - lf3, TOOLBAR_MEN_STEP_Y * 2);
		}
		else if (_nav_btn == nav_filter)
		{
			rc = Rect(lf3, top3, TOOLBAR_LEFT_WIDTH - lf3, TOOLBAR_MEN_STEP_Y * 10 + 30);
		}
		else if (_nav_btn == nav_draw)
		{
			rc = Rect(lf3, top3, TOOLBAR_LEFT_WIDTH - lf3, TOOLBAR_MEN_STEP_Y * 8);
		}
	}
}

int CPaint::get_toolbar_lf()
{
	int lf3 = 0;
	if (_nav_btn == nav_filter)
	{
		TOOLBAR_LEFT_WIDTH = _combo_width_filter + 16;
		lf3 = (TOOLBAR_LEFT_WIDTH - _combo_width_filter) / 2;
	}
	else if (_nav_btn == nav_draw)
	{
		TOOLBAR_LEFT_WIDTH = _combo_width_draw + 16;
		lf3 = (TOOLBAR_LEFT_WIDTH - _combo_width_draw) / 2;
	}
	else {
		TOOLBAR_LEFT_WIDTH = _combo_width_inp + 16;
		lf3 = (TOOLBAR_LEFT_WIDTH - _combo_width_inp) / 2;
	}

	return lf3;
}