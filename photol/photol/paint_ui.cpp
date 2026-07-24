///////////////////////////////////////////////////////////////////////
/// @file paint_ui.cpp
/// @brief PhotoNest 绘图模块 - UI 绘制功能实现文件
/// @details 实现绘图界面的 UI 元素绘制:
///           - 工具栏按钮绘制 (draw_ui)
///           - UI 状态管理
///           - 按钮启用/禁用逻辑
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的核心 UI 文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
#include <process.h>


void CPaint::draw_ui()
{
	int len0 = (int)_opt_arr.size();
	bool bEmpty = _lena2.empty() ? true : false;
	bool canInp = canInpaint();
	bool b0 = !_seg_mask.empty() || !_sticker_arr.empty() || canInp;
	bool bNothing = b0 || !_clip_mat.empty() || _lena2.empty();

#pragma region open save
	int lf = TOOLBAR_BTN_LEFT;
	int lf00 = TOOLBAR_BTN_LEFT;

	_frame(Rect(0, 0, _win_w, TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

	int top0 = 8;
	int top2 = 28;

	if (_app_type == FROM_EXE)
	{
		if (_lena2.empty())
		{
			if (_toosmall == 1)
			{
				if (cvui::button(_frame, (_win_w - lbl_small.cols) / 2, (_win_h - lbl_small.rows) / 2,
					lbl_small, lbl_small, lbl_small))
				{
					open();
				}
			}
			else
			{
				if (_fname != L"")
				{
					if (!_fnameLoaded)
					{
						lbl_loading.copyTo(_frame(Rect((_win_w - lbl_loading.cols) / 2, (_win_h - lbl_loading.rows) / 2, lbl_loading.cols, lbl_loading.rows)));
					}
					else
					{
						lbl_openerr.copyTo(_frame(Rect((_win_w - lbl_openerr.cols) / 2, (_win_h - lbl_openerr.rows) / 2, lbl_openerr.cols, lbl_openerr.rows)));
					}
				}
				else
				{
					if (cvui::button(_frame, (_win_w - lbl_drag_idle.cols) / 2, (_win_h - lbl_drag_idle.rows) / 2,
						lbl_drag_idle, lbl_drag_idle, lbl_drag_idle))
					{
						open();
					}
					//if (cvui::button(_frame, (_win_w - lbl_drag_idle.cols) / 2 + lbl_drag_idle.cols + 8, (_win_h - firstopen_idle.rows) / 2,
					//	firstopen_idle, firstopen_idle, firstopen_idle))
					//{
					//	open();
					//}
				}
			}

			if (!expiration_idle.empty())
			{
				if (cvui::button(_frame, (_win_w - expiration_idle.cols) - 20, _win_h - expiration_idle.rows - 20,
					expiration_idle, expiration_idle, expiration_idle))
				{
					_help_btn = 30000;
					_show_help = 0;
					_working = act_tips_ui;
				}
			}
		}

		int lf0 = _win_w + TOOLBAR_RIGHT_WIDTH - 20 - help_idle.cols;

		if (cvui::button(_frame, lf0, TOOLBAR_BTN_TOP, help_idle, help_over, help_over))
		{
			if (!_inpaint_click)
			{
				if (_help_click == menu_help && _show_help == 1)
				{
					_show_help = -1;
				}
				else
				{
					_show_help = 1;
				}

				_help_click = menu_help;
				_working = act_tips_ui;
			}
		}

		if (_help_click == menu_help)
		{
			if (_show_help == 1)
			{
				lf0 -= _combo_width_help;
				lf0 += help_idle.cols;

				int tp = TOOLBAR_TOP_HEIGHT;
				int step = 4;

				_frame(Rect(lf0, tp, _combo_width_help, TOOLBAR_MEN_STEP_Y * step)) = Scalar::all(43);
				if (cvui::button(_frame, lf0, tp, home_idle, home_over, home_over))
				{
					_help_btn = 10000;
					_show_help = 0;
					_working = act_tips_ui;
				}

				tp += TOOLBAR_MEN_STEP_Y;
				if (cvui::button(_frame, lf0, tp, settings_idle, settings_over, settings_over))
				{
					_help_btn = 20000;
					_show_help = 0;
					_working = act_tips_ui;
				}

				tp += TOOLBAR_MEN_STEP_Y;
				if (cvui::button(_frame, lf0, tp, reg_idle, reg_over, reg_over))
				{
					_help_btn = 30000;
					_show_help = 0;
					_working = act_tips_ui;
				}

				tp += TOOLBAR_MEN_STEP_Y;
				if (cvui::button(_frame, lf0, tp, about_idle, about_over, about_over))
				{
					_help_btn = 40000;
					_show_help = 0;
					_working = act_tips_ui;
				}
			}
		}
	}
	else
	{
		int lf0 = _win_w + TOOLBAR_RIGHT_WIDTH - 80;

		int w = original_over.cols;
		int lf2 = lf0 - w - save_idle.cols - down_idle.cols - 15;

		if (cvui::button(_frame, lf2, TOOLBAR_BTN_TOP,
			(_close_ret >= 1 || !_exist_coi) ? original_gray : original_idle,
			(_close_ret >= 1 || !_exist_coi) ? original_gray : original_over,
			(_close_ret >= 1 || !_exist_coi) ? original_gray : original_over))
		{
			if (!_inpaint_click)
			{
				if (_exist_coi)
				{
					_tool_btn = toolbar_open;
				}
			}
		}

		nav_saveas_ui(lf0 - save_idle.cols - down_idle.cols - 2, b0);

		if (_lena2.empty())
		{
			if (!_fnameLoaded)
			{
				lbl_loading.copyTo(_frame(Rect((_win_w - lbl_loading.cols) / 2, (_win_h - lbl_loading.rows) / 2, lbl_loading.cols, lbl_loading.rows)));
			}
			else
			{
				if (!lbl_openerr.empty())
				{
					lbl_openerr.copyTo(_frame(Rect((_win_w - lbl_openerr.cols) / 2, (_win_h - lbl_openerr.rows) / 2, lbl_openerr.cols, lbl_openerr.rows)));
				}
			}

		}

	}

#pragma endregion





#pragma region nav_sub
	if (!_down_nav_click && _close_ret == 0 && _do_nav_btn == 0)
	{
		if (_nav_btn == nav_draw)
		{
			nav_draw_ui();
		}
		else if (_nav_btn == nav_segment)
		{
			nav_seg_ui();
		}
		else if (_nav_btn == nav_sticker)
		{
			nav_sticker_ui();
		}
		else if (_nav_btn == nav_filter)
		{
			if (_close_ret >= 1)
			{
				z_filter val = _fVal;
				nav_filter_ui(val);
			}
			else
			{
				nav_filter_ui(_fVal);
			}
		}
		else if (_nav_btn == nav_inp)
		{
			nav_inp_ui(canInp);
		}

		get_toolbar_rc(_left_bar_rc);
		if (_left_bar_rc.width > 1)
		{
			_left_bar = _frame(_left_bar_rc).clone();
		}
	}
	else
	{
		if (_left_bar_rc.width > 1 && !_left_bar.empty())
		{
			get_toolbar_lf();

			_frame(Rect(0, TOOLBAR_TOP_HEIGHT, TOOLBAR_LEFT_WIDTH, _win_h - TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);

			_left_bar.copyTo(_frame(_left_bar_rc));

			if (_navbar_state == 0)
			{
				make_gray2(_frame(_left_bar_rc));
			}
			if (_navbar_state == 1)
			{
			}
			else if (_navbar_state == 2)
			{
				_down_nav_click = false;
				_navbar_state = 0;
				_working = act_display;
			}
		}
	}

	if (_left_bar_rc.width > 1)
	{
		if (_lena2.empty())
		{
			make_gray2(_frame(_left_bar_rc));
		}
	}

	{

		if (_nav_btn == nav_inp)
		{
			_lbl_nav = _lbl_inp;
		}
		else if (_nav_btn == nav_segment)
		{
			_lbl_nav = _lbl_segment;
		}
		else if (_nav_btn == nav_sticker)
		{
			_lbl_nav = _lbl_sticker;
		}
		else if (_nav_btn == nav_filter)
		{
			_lbl_nav = _lbl_filter;
		}
		else
		{
			_nav_btn = nav_draw;
			_lbl_nav = _lbl_draw;
		}


		int top3 = TOOLBAR_BTN_TOP;


		if (_app_type != FROM_EXE)
		{
			lf += 12;
		}

		_lf_nav = lf;
		if (cvui::button(_frame, lf, (TOOLBAR_TOP_HEIGHT - _lbl_nav.rows) / 2, _lbl_nav, _lbl_nav, _lbl_nav))
		{
			if (!_inpaint_click)
			{
				_down_nav_click = !_down_nav_click;
				_pointer_btn = btn_move;
				_opt = 0;
				_working = act_tips_ui;
			}
		}

		/*
		if (cvui::button(_frame, lf + _lbl_nav.cols + 4, (TOOLBAR_TOP_HEIGHT - _lbl_nav.rows) / 2, down2_idle, down2_over, down2_over))
		{
			if (!_inpaint_click)
			{
				_down_nav_click = !_down_nav_click;
				_pointer_btn = btn_move;
				_opt = 0;
				_working = act_tips_ui;
			}
		}
*/
		if (!_inpaint_click && _down_nav_click)
		{
			top3 = TOOLBAR_TOP_HEIGHT;
			int step = MENU_BTN_STEP;

			_frame(Rect(lf, top3, _combo_width_nav, step * 5)) = Scalar::all(43);

			if (cvui::button(_frame, lf, top3, _nav_btn == nav_inp ? nav_inp_over : nav_inp_idle, nav_inp_over, nav_inp_over))
			{
				if (!_inpaint_click)
				{
					bool state = (_opt >= btn_brush && _opt <= btn_inpaint) ? true : false;
					bool b = set_working(nav_inp, state);
					TOOLBAR_LEFT_WIDTH = _combo_width_inp + 16;
					WritePrivate("nav_btn", _nav_btn);
				}
				_working = act_display;
				_down_nav_click = false;
			}

			top3 += step;
			if (cvui::button(_frame, lf, top3, _nav_btn == nav_filter ? nav_filter_over : nav_filter_idle, nav_filter_over, nav_filter_over))
			{
				if (!_inpaint_click)
				{
					if (!_opt_arr.empty() && _idx > 0 && _idx <= _opt_arr.size())
					{
						int opt_type = _opt_arr[_idx - 1].opt_type;
						if (opt_type != btn_filter)
						{
							init_fVal(_fVal);
						}
					}

					bool b = set_working(nav_filter, false);
					TOOLBAR_LEFT_WIDTH = _combo_width_filter + 16;
					WritePrivate("nav_btn", _nav_btn);
				}

				_working = act_display;
				_down_nav_click = false;
			}

			top3 += step;
			if (cvui::button(_frame, lf, top3, _nav_btn == nav_segment ? nav_segment_over : nav_segment_idle, nav_segment_over, nav_segment_over))
			{
				if (!_inpaint_click)
				{
					bool state = (_opt == btn_seg_fgd || _opt == btn_seg_bgd || _opt == btn_seg_rect) ? true : false;
					bool b = set_working(nav_segment, state);
					TOOLBAR_LEFT_WIDTH = _combo_width_inp + 16;
					WritePrivate("nav_btn", _nav_btn);
				}
				_working = act_display;

				_down_nav_click = false;
			}

			top3 += step;
			if (cvui::button(_frame, lf, top3, _nav_btn == nav_sticker ? nav_sticker_over : nav_sticker_idle, nav_sticker_over, nav_sticker_over))
			{
				if (!_inpaint_click)
				{
					_down_sticker_click = false;
					bool state = (_opt == btn_sticker) ? true : false;
					bool b = set_working(nav_sticker, state);
					TOOLBAR_LEFT_WIDTH = _combo_width_inp + 16;
					WritePrivate("nav_btn", _nav_btn);
				}
				_working = act_display;

				_down_nav_click = false;
			}

			top3 += step;
			if (cvui::button(_frame, lf, top3, _nav_btn == nav_draw ? nav_draw_over : nav_draw_idle, nav_draw_over, nav_draw_over))
			{
				if (!_inpaint_click)
				{
					bool state = (_opt >= btn_draw_free && _opt <= btn_draw_select) ? true : false;
					bool b = set_working(nav_draw, state);
					TOOLBAR_LEFT_WIDTH = _combo_width_draw + 16;
					WritePrivate("nav_btn", _nav_btn);
				}
				_working = act_display;

				_down_nav_click = false;
			}
		}

		lf += _lbl_nav.cols + 32;// + 4;
		lf += 3;
		line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
		lf += 11;


		if (_app_type == FROM_EXE)
		{
			_lf_open = lf;

			if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP, b0 ? open_ico_gray : open_ico_idle, b0 ? open_ico_gray : open_ico_over, b0 ? open_ico_gray : open_ico_over))
			{
				if (!_inpaint_click & !b0)
				{
					bool b = canExit();
					if (!b)
					{
						_close_ret = 5;
						return;
					}
					_tool_btn = toolbar_open;

				}
			}
			lf += TOOLBAR_BTN_STEP_X;

			_lf_save = lf;
			lf00 = lf;
			nav_saveas_ui(lf00, b0);

			lf += TOOLBAR_BTN_STEP_X;
			lf += 15;
			lf += 3;
			line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
			lf += 11;
		}
	}

#pragma endregion


#pragma region toolbar

	_lf_move = lf;
	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP,
		bEmpty ? hand_gray : hand_idle, bEmpty ? hand_gray : hand_over, bEmpty ? hand_gray : hand_over))
	{
		if (!_inpaint_click && !bEmpty)
		{
			bool b = is_unapplied(canInp);
			if (b)
			{
				return;
			}
			_opt = 0;
			_last_opt = 0;
			_pointer_btn = btn_move;
		}
	}

	BOOL b7 = IsClipboardFormatAvailable(CF_BITMAP);

	lf += TOOLBAR_BTN_STEP_X;
	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP, !b7 ? paste_gray : paste_idle,
		!b7 ? paste_gray : paste_over, !b7 ? paste_gray : paste_over))
	{
		if (!_inpaint_click && b7)
		{
			paste_clipboard();

			_pointer_btn = btn_move;
			_working = act_display;
		}
	}

	//lf += 12;
	lf += TOOLBAR_BTN_STEP_X;
	lf += 3;
	line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
	lf += 11;

	_lf_undo = lf;
	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP,
		(_idx > 0 && len0 != 0) ? undo_idle : undo_gray, (_idx > 0 && len0 != 0) ? undo_over : undo_gray, (_idx > 0 && len0 != 0) ? undo_over : undo_gray))
	{
		if (!_inpaint_click && _idx > 0 && len0 != 0)
		{
			_tool_btn = toolbar_undo;
			_working = act_tips_ui;
		}
	}

	lf += TOOLBAR_BTN_STEP_X;
	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP,
		(_idx < len0 && len0 != 0) ? redo_idle : redo_gray, (_idx < len0 && len0 != 0) ? redo_over : redo_gray, (_idx < len0 && len0 != 0) ? redo_over : redo_gray))
	{
		if (!_inpaint_click && _idx < len0 && len0 != 0)
		{
			_tool_btn = toolbar_redo;
			_working = act_tips_ui;
		}
	}
	lf += TOOLBAR_BTN_STEP_X;

	lf += 3;
	line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
	lf += 11;

	if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP,
		bNothing ? center_gray : center_idle, bNothing ? center_gray : center_over, bNothing ? center_gray : center_over))
	{
		if (!_inpaint_click && !bNothing)
		{
			bool b = is_unapplied(canInp);
			if (b)
			{
				return;
			}

			clear_opt();
			_tool_btn = toolbar_one;
			_working = act_tips_ui;
		}
	}

	if (_need_refresh)
	{
		if (lf + TOOLBAR_BTN_STEP_X + 300 < _win_w)
		{
			_frame(Rect(lf + TOOLBAR_BTN_STEP_X, 0, 300, TOOLBAR_TOP_HEIGHT - 2)) = Scalar::all(43);
		}
		_need_refresh = false;
	}
	_lf_font = 0;
	_lf_apply = 0;

	if (_opt == btn_magic || _opt == btn_brush || _opt == btn_eraser || _opt == btn_draw_free ||
		_opt == btn_draw_eraser || _opt == btn_draw_straight || _opt == btn_draw_arrow ||
		_opt == btn_draw_rect || _opt == btn_draw_circle || _opt == btn_free_select || _opt == btn_draw_mosaic ||
		_opt == btn_draw_magic || _opt == btn_draw_fill)
	{
		_need_refresh = true;
		if (!_lena2.empty())
		{
			lf += TOOLBAR_BTN_STEP_X;
			lf += 3;
			line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
			lf += 11;

			_lf_font = lf;
			Mat lbl;
			if (_opt == btn_magic || _opt == btn_draw_magic || _opt == btn_draw_fill)
			{
				lbl = lbl_tolerance;
			}
			else
			{
				lbl = lbl_thickness;
			}

			int off = lf + 5 + lbl.cols + 10 + 200;
			if (off < _win_w)
			{
				int tp = (TOOLBAR_TOP_HEIGHT - lbl.rows) / 2;
				lbl.copyTo(_frame(Rect(lf + 5, tp, lbl.cols, lbl.rows)));

				tp = -10;
				lf += lbl.cols;
				if (_close_ret == 0)
				{
					if (_opt == btn_magic || _opt == btn_draw_magic || _opt == btn_draw_fill)
					{
						cvui::trackbar(_frame, lf + 10, tp, 100, &_upDiff, 10, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS);
					}
					else if (_opt == btn_brush || _opt == btn_eraser || _opt == btn_free_select)
					{
						cvui::trackbar(_frame, lf + 10, tp, 100, &_inpaint_thickness, 10, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS);
					}
					else if (_opt == btn_draw_eraser || _opt == btn_draw_mosaic)
					{
						cvui::trackbar(_frame, lf + 10, tp, 100, &_erase_thickness, 10, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS);
					}
					else
					{
						cvui::trackbar(_frame, lf + 10, tp, 100, &_draw_thickness, 1, 100, 1, "%.0Lf", cvui::TRACKBAR_HIDE_LABELS);
					}
				}
				else
				{
					if (_opt == btn_magic || _opt == btn_draw_magic || _opt == btn_draw_fill)
					{
						int ff = _upDiff;
						cvui::trackbar(_frame, lf + 10, tp, 100, &ff, 1, 100, 1, "%d", cvui::TRACKBAR_HIDE_LABELS);
					}
					else if (_opt == btn_brush || _opt == btn_eraser || _opt == btn_free_select)
					{
						int ff = _inpaint_thickness;
						cvui::trackbar(_frame, lf + 10, tp, 100, &ff, 1, 100, 1, "%d", cvui::TRACKBAR_HIDE_LABELS);
					}
					else if (_opt == btn_draw_eraser || _opt == btn_draw_mosaic)
					{
						int ff = _erase_thickness;
						cvui::trackbar(_frame, lf + 10, tp, 100, &ff, 1, 100, 1, "%d", cvui::TRACKBAR_HIDE_LABELS);
					}
					else
					{
						int ff = _draw_thickness;
						cvui::trackbar(_frame, lf + 10, tp, 100, &ff, 1, 100, 1, "%d", cvui::TRACKBAR_HIDE_LABELS);
					}
				}

				lf += 140;
			}
		}
	}
	else if (!_sticker_arr.empty())
	{
		_need_refresh = true;
		lf += TOOLBAR_BTN_STEP_X;
		lf += 3;
		line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
		lf += 11;
		int top3 = TOOLBAR_BTN_TOP;

		string txt = " ";

		if (_seamless_mode == NORMAL_CLONE)
		{
			txt = "Normal clone";
		}
		else if (_seamless_mode == MIXED_CLONE)
		{
			txt = "Mixed clone";
		}
		else if (_seamless_mode == MONOCHROME_TRANSFER)
		{
			txt = "Monochrome transfer";
		}
		else if (_seamless_mode == 9)
		{
			txt = "Paste";
		}
		else if (_seamless_mode == 10)
		{
			txt = "Flood clone";
		}

		lbl_seamless = get_lbl(_u2w(_lang.trans(txt)).c_str(), Scalar::all(74));
		Mat temp = Mat::zeros(Size(_combo_width_sticker_2, lbl_seamless.rows + 4), CV_8UC3);
		change_width(temp, lbl_seamless, Scalar::all(74), 8);

		int tp = (TOOLBAR_TOP_HEIGHT - lbl_seamless.rows) / 2;
		int off = lf + 5 + _combo_width_sticker_2 + 40;
		if (off < _win_w)
		{
			if (cvui::button(_frame, lf + 5, tp, lbl_seamless, lbl_seamless, lbl_seamless))
			{
				if (!_inpaint_click)
				{
					_down_sticker_click = !_down_sticker_click;
					_working = act_tips_ui;
				}
			}

			_lf_font = lf;
			if (cvui::button(_frame, lf + lbl_seamless.cols + 10, tp, down2_idle, down2_over, down2_over))
			{
				if (!_inpaint_click)
				{
					_down_sticker_click = !_down_sticker_click;
					_working = act_tips_ui;
				}
			}

			_lf_apply = lf + lbl_seamless.cols + 20 + down2_idle.cols;
			if (cvui::button(_frame, _lf_apply, TOOLBAR_BTN_TOP, apply_idle, apply_over, apply_gray))
			{
				if (!_inpaint_click && !_sticker_arr.empty())
				{
					int opt = btn_change_bg_ok;
					if (_nav_btn == nav_segment)
					{
						_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);
					}
					else if (_nav_btn == nav_sticker)
					{
						opt = btn_sticker_ok;
					}

					int k = get_thread_idx();
					threads[k].stop = false;
					threads[k].opt = opt;
					_handle_thread = (HANDLE)_beginthreadex(NULL, 0, handle_sticker, this, NULL, NULL);
					threads[k].hdl = _handle_thread;

					_opt = 0;
					_last_opt = 0;
					_pointer_btn = btn_move;
				}
			}

			if (!_inpaint_click && _down_sticker_click)
			{
				_down_saveas_click = false;
				_down_cut_click = false;

				_need_refresh = true;

				int step = MENU_BTN_STEP;
				top3 += step;
				_frame(Rect(lf + 5, top3, _combo_width_sticker_2, step * 4)) = Scalar::all(43);

				if (cvui::button(_frame, lf + 5, top3,
					_seamless_mode == NORMAL_CLONE ? normal_clone_over : normal_clone_idle, normal_clone_over, normal_clone_over))
				{
					_seamless_mode = NORMAL_CLONE;
					WritePrivate("seamless_mode", _seamless_mode);

					_down_sticker_click = false;
					_working = act_display;
				}

				top3 += step;
				if (cvui::button(_frame, lf + 5, top3,
					_seamless_mode == MIXED_CLONE ? mixed_clone_over : mixed_clone_idle, mixed_clone_over, mixed_clone_over))
				{
					_seamless_mode = MIXED_CLONE;
					WritePrivate("seamless_mode", _seamless_mode);

					_down_sticker_click = false;
					_working = act_display;
				}

				top3 += step;
				if (cvui::button(_frame, lf + 5, top3,
					_seamless_mode == MONOCHROME_TRANSFER ? monochrome_over : monochrome_idle, monochrome_over, monochrome_over))
				{
					_seamless_mode = MONOCHROME_TRANSFER;
					WritePrivate("seamless_mode", _seamless_mode);

					_down_sticker_click = false;
					_working = act_display;
				}

				top3 += step;
				if (cvui::button(_frame, lf + 5, top3,
					_seamless_mode == 9 ? paste_only_over : paste_only_idle, paste_only_over, paste_only_over))
				{
					_seamless_mode = 9;
					WritePrivate("seamless_mode", _seamless_mode);

					_down_sticker_click = false;
					_working = act_display;
				}
			}
		}
	}
	else if (_rect_select.width != 0)
	{
		lf += TOOLBAR_BTN_STEP_X;
		lf += 3;
		line(_frame, Point(lf, top0), Point(lf, top2), Scalar::all(96), 1, 8, 0);
		lf += 11;

		if (cvui::button(_frame, lf, TOOLBAR_BTN_TOP,
			_rect_select.width == 0 ? copy_gray : copy_idle, _rect_select.width == 0 ? copy_gray : copy_over, _rect_select.width == 0 ? copy_gray : copy_over))
		{
			if (!_inpaint_click && _rect_select.width != 0)
			{
				Rect rc;
				copy_clipboard(rc);
				_working = act_tips_ui;
			}
		}

		_lf_copy = lf + copy_idle.cols + 2;

		if (cvui::button(_frame, _lf_copy, TOOLBAR_BTN_TOP,
			_rect_select.width == 0 ? down_gray : (_down_cut_click ? down_over : down_idle),
			_rect_select.width == 0 ? down_gray : down_over, _rect_select.width == 0 ? down_gray : down_over))
		{
			if (!_inpaint_click && _rect_select.width != 0)
			{
				_down_cut_click = !_down_cut_click;
				_working = act_tips_ui;
			}
		}

		if (_down_cut_click && _close_ret == 0 && _rect_select.width != 0 && !_lena2.empty())
		{
			_down_saveas_click = false;
			_down_sticker_click = false;

			int lf2 = lf;// +copy_idle.cols + 2;

			if (cvui::button(_frame, lf2, TOOLBAR_TOP_HEIGHT, cut_idle, cut_over, cut_idle))
			{
				if (!_inpaint_click && _rect_select.width != 0)
				{
					Rect rc;
					copy_clipboard(rc);
					trans_apply();

					_down_cut_click = false;
				}
				_working = act_display;
			}
		}
	}

	if (_app_type != FROM_EXE)
	{
		if (cvui::button(_frame, _win_w + TOOLBAR_RIGHT_WIDTH - close_gray.cols - 15, TOOLBAR_BTN_TOP - 2,
			close_idle, close_over, close_idle))
		{
			_close_ret = 1;
			bool b = is_unapplied(canInp);
			if (b)
			{
				return;
			}
		}
	}
#pragma endregion

#pragma region _nav_btn
	if (_opt == btn_rota_bar)
	{
		nav_rota_ui();
	}
	else if (_opt == btn_crop_bar)
	{
		nav_crop_ui();
	}

#pragma endregion

#pragma region bottom
	_frame(Rect(0, _win_h, _win_w, FOOTER_HEIGHT)) = Scalar::all(43);

	//if (_app_type == FROM_APP_NAV || _app_type == FROM_EXE)
	{
		//if (_len_files > 0)
		{
			bool b1 = b0 || _idx_file >= _len_files;
			int rf = _win_w + TOOLBAR_RIGHT_WIDTH - prev_idle.cols - 20;
			if (cvui::button(_frame, rf, _win_h + (FOOTER_HEIGHT - next_idle.rows) / 2,
				b1 ? next_gray : next_idle, b1 ? next_gray : next_over, b1 ? next_gray : next_idle))
			{
				if (!_inpaint_click && !b1)
				{
					bool b = canExit();
					if (!b)
					{
						_close_ret = 3;
					}
					else
					{
						nav(1);
					}
				}
			}

			b1 = b0 || _idx_file <= 1;
			if (cvui::button(_frame, rf - prev_idle.cols - 4, _win_h + (FOOTER_HEIGHT - next_idle.rows) / 2,
				b1 ? prev_gray : prev_idle, b1 ? prev_gray : prev_over, b1 ? prev_gray : prev_idle))
			{
				if (!_inpaint_click && !b1)
				{
					bool b = canExit();
					if (!b)
					{
						_close_ret = 2;
					}
					else
					{
						nav(0);
					}
				}
			}
		}
	}

#pragma endregion

#pragma region close

	if (_do_nav_btn > 0)
	{
		_inpaint_click = false;

		bool close = false;
		show_window(1, close);
	}

	if (_disp_nb >= 1)
	{
		_disp_nb++;
		int x = _win_w - 450;
		int y = TOOLBAR_BTN_TOP;

		string txt = "";

		if (_show_err_type != 0)
		{
			if (_show_err_type == 1)
			{
				txt = "The outside of the rectangle is the background";
				show_msg(0, y, txt);
				_show_error_times = 100;
			}
			else if (_show_err_type == 2)
			{
				txt = "File is too big";
				show_msg(0, y, txt);
				_show_error_times = 100;
			}
			else if (_show_err_type == 3)
			{
				txt = "No sky detected";
				show_msg(0, y, txt);
				_show_error_times = 100;
			}
			else if (_show_err_type == 4)
			{
				txt = "Canvas area exceeds the maximum limit (2/3)";
				show_msg(0, y, txt);
				_show_error_times = 100;
			}
		}
		else
		{
			if (_ov == 1)
			{
				txt = "30 days trial period";
				show_msg(0, y, txt);
				_show_error_times = 100;
			}
			else
			{
				if (_save_ret == 0)
				{
					txt = "Saved successfully";
					show_msg(0, y, txt, Scalar(194, 245, 194), Scalar(0x00, 0xaa, 0x00));
					_show_error_times = 100;
				}
				else if (_save_ret == -1)
				{
					txt = "An error occurred";
					show_msg(0, y, txt);
					_show_error_times = 100;
				}
				else if (_save_ret == -2)
				{
					txt = "Read-only attribute";
					show_msg(0, y, txt);
					_show_error_times = 100;
				}
			}
		}

		if (_disp_nb > _show_error_times)
		{
			_show_err_type = 0;
			_save_ret = 1;
			_disp_nb = 0;
			_working = act_display;
		}
	}

	if (_close_ret >= 1)
	{
		_closed = false;
		bool b = canExit();
		if (!b)
		{
			if (_hInputWnd != NULL)
			{
				input_command(1);
			}

			_do_nav_btn = 0;
			_do_nav_state = false;
			_inpaint_click = false;
			if (_ov == 0 && _close_ret == 1)
			{
				show_window(0, _closed);
			}
			else
			{
				_close_ret = 0;
				_closed = true;
			}
		}
		else if (_close_ret == 1)
		{
			_close_ret = 0;
			_closed = true;
		}
		else
		{
			_close_ret = 0;
		}
	}
#pragma endregion

#pragma region footer

	if (_fcoi != L"")
	{
		if (cvui::button(_frame, TOOLBAR_BTN_LEFT, _win_h + (FOOTER_HEIGHT - loc_idle.rows) / 2,
			loc_idle, loc_over, loc_idle))
		{
			if (!_inpaint_click)
			{
				wstring p = _fcoi;
				Replace(p, L"/", L"\\");

				transform(p.begin(), p.end(), p.begin(), ::tolower);

				wstring::size_type pos = p.find(L"\\photonest_app\\");
				if (pos != wstring::npos)
				{
					reset_appdata_path(p);
				}

				wstring s1 = L"/select, " + p;
				ShellExecute(NULL, L"open", L"explorer.exe", s1.c_str(), NULL, SW_SHOWNORMAL);
			}
		}
	}
	line(_frame, Point(0, _win_h), Point(_win_w, _win_h), Scalar::all(54), 1, 8, 0);

	if (!_lena2.empty() && _dst_zoom_w > 0 && _lena_w > 0)
	{

		wchar_t buf[200] = { 0 };
		int y = 0;
		if (_size_info.empty())
		{
			swprintf_s(buf, 200, L"%d x %d", _lenaSize.width, _lenaSize.height);
			_size_info = get_lbl(buf, Scalar::all(43), Scalar::all(128));

			y = (info_idle.rows - _size_info.rows) / 2;
			if (y > 0 && _size_info.rows + y < info_idle.rows)
			{
				_size_info.copyTo(info_idle(Rect(335, y, _size_info.cols, _size_info.rows)));
			}
			else
			{
				_size_info = Mat::zeros(Size(0, 0), CV_8UC3);
			}
			_working = act_display;
		}

		if (_kb_info.empty())
		{
			memset(buf, 0, sizeof(wchar_t) * 200);
			float val0 = (float)_mkb / 1024.0f / 1024.0f;
			if (val0 > 1)
			{
				swprintf_s(buf, 200, L"%.2f MB", val0);
			}
			else
			{
				swprintf_s(buf, 200, L"%.2f KB", (float)_mkb / 1024.0f);
			}

			_kb_info = get_lbl(buf, Scalar::all(43), Scalar::all(128));

			y = (info_idle.rows - _kb_info.rows) / 2;
			if (y > 0 && _kb_info.rows + y < info_idle.rows)
			{
				_kb_info.copyTo(info_idle(Rect(480, y, _kb_info.cols, _kb_info.rows)));
			}
			else
			{
				_kb_info = Mat::zeros(Size(0, 0), CV_8UC3);
			}
			_working = act_display;
		}

		Rect rt = Rect(-1, -1, 0, 0);
		if (_curr_rt.x > 0 || _curr_rt.y > 0 || _curr_rt.width > 0)
		{
			rt = _curr_rt;
			if (_opt == btn_seg_rect || _opt == btn_draw_select)
			{
				rt.x = _dst_in_lena.x + rt.x * _lena_w / _dst_zoom_w;
				rt.y = _dst_in_lena.y + rt.y * _lena_w / _dst_zoom_w;
				rt.width = rt.width * _lena_w / _dst_zoom_w;
				rt.height = rt.height * _lena_w / _dst_zoom_w;
			}
			else if (_opt == btn_crop)
			{
				crop_rt(rt);
			}
		}
		else
		{
			if (_rect_select.width != 0)
			{
				rt = _rect_select;
			}
			else if (_rect_paste.width != 0)
			{
				rt = _rect_paste;
			}
			else if (_rect_crop.width != 0)
			{
				rt = _rect_crop;
				crop_rt(rt);
			}
			else if (_rect_seg_select.width != 0)
			{
				rt = _rect_seg_select;
			}
			else if (_input_win_in_dst.width != 0)
			{
				rt = _input_win_in_dst;
			}

			if (rt.x != -1 && rt.y != -1 && _rect_crop.width == 0)
			{
				rt.x = _dst_in_lena.x + rt.x * _lena_w / _dst_zoom_w;
				rt.y = _dst_in_lena.y + rt.y * _lena_w / _dst_zoom_w;
				rt.width = rt.width * _lena_w / _dst_zoom_w;
				rt.height = rt.height * _lena_w / _dst_zoom_w;
			}
		}

		if (rt.x == -1 && rt.y == -1)
		{
			int x0 = cvui::mouse().x - _lena_in_frame.x;
			int y0 = cvui::mouse().y - _lena_in_frame.y;
			Point pt = get_lena_pt(x0, y0);
			rt = Rect(pt.x, pt.y, 0, 0);
		}

		memset(buf, 0, sizeof(wchar_t) * 200);
		if (rt.x >= 0 && rt.y >= 0 && rt.x <= _lena_w && rt.y <= _lena_h)
		{
			swprintf_s(buf, 200, L"%d, %d            ", (int)(rt.x * _lenaf), (int)(rt.y * _lenaf));

			Mat lbl = get_lbl(buf, Scalar::all(43), Scalar::all(128));
			if (!lbl.empty() && lbl.rows <= 24)
			{
				int y = (info_idle.rows - lbl.rows) / 2;
				lbl.copyTo(info_idle(Rect(50, y, lbl.cols, lbl.rows)));
			}
		}

		if (rt.width > 0 && rt.height > 0 && (_opt == btn_draw_circle || (rt.width <= _lena_w && rt.height <= _lena_h)))
		{
			memset(buf, 0, sizeof(wchar_t) * 200);
			swprintf_s(buf, 200, L"%d x %d        ", (int)(rt.width * _lenaf), (int)(rt.height * _lenaf));
			Mat lbl = get_lbl(buf, Scalar::all(43), Scalar::all(128));
			if (!lbl.empty() && lbl.rows <= 24)
			{
				int y = (info_idle.rows - lbl.rows) / 2;
				lbl.copyTo(info_idle(Rect(190, y, lbl.cols, lbl.rows)));
			}
		}
		else
		{
			info_idle(Rect(190, 0, 120, info_idle.rows)) = Scalar::all(43);
		}

		y = _win_h + (FOOTER_HEIGHT - info_idle.rows) / 2;
		int x = _win_w - info_idle.cols - 220;
		if (x > 0)
		{
			info_idle.copyTo(_frame(Rect(x, y, info_idle.cols, info_idle.rows)));
		}

		memset(buf, 0, sizeof(wchar_t) * 200);
		swprintf_s(buf, 200, L"%.0f%%        ", (double)_dst_zoom_w * 100 / (double)_lena_w);
		print_bottom_info(_win_w - 170, buf);

		if (_len_files > 0)
		{
			memset(buf, 0, sizeof(wchar_t) * 200);
			swprintf_s(buf, 200, L"%d / %d    ", _idx_file, _len_files);
			print_bottom_info(_win_w - 100, buf);
		}

		if (_close_ret >= 1 || _do_nav_btn > 0)
		{
			Mat roi = _frame(Rect(0, _win_h, _win_w, FOOTER_HEIGHT));
			if (!roi.empty())
			{
				make_gray2(roi);
			}
		}
	}

#pragma endregion


}

void CPaint::reset_appdata_path(wstring& p)
{
	wstring path = get_module_path(NULL);
	wstring::size_type pos = path.find(L"\\windowsapps\\photonest");
	if (pos != wstring::npos)
	{
		pos = path.find(L"x64__");
		if (pos != wstring::npos)
		{
			path = path.substr(pos + 5);
			Replace(p, L"\\local\\photonest_app\\", L"\\local\\packages\\photonest.photonest_" + path + L"\\localcache\\local\\photonest_app\\");
		}
	}
}


