///////////////////////////////////////////////////////////////////////
/// @file paint_ui_tips.cpp
/// @brief 绘图模块 - 提示信息和光标处理实现
/// @details 实现工具提示显示、光标样式切换、鼠标位置跟踪、
///          状态栏信息更新等用户交互反馈功能
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件增强用户交互体验,提供实时操作反馈
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include <shellapi.h>
#include <process.h>
extern HINSTANCE _his;


void CPaint::show_tips(int& pre_xy, int& cur_xy, int& hit)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(_win_handle, &pt);

	int x = pt.x;
	int y = pt.y;

#pragma region cursor  
	UINT id_arrow = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_ARROW));
	UINT _cursor = id_arrow;

	if (x > _left_width && x < _win_w && y > TOOLBAR_TOP_HEIGHT && y < _win_h)
	{
		if (_lena2.empty())
		{
			_cursor = id_arrow;
		}
		else
		{
			switch (_opt)
			{
			case btn_draw_text:
				_cursor = IDCUR_TEXT;
				break;
			case btn_draw_mosaic:
				_cursor = IDCUR_MOSAIC;
				break;
			case btn_brush:
				_cursor = IDCUR_MAKER;
				break;
			case btn_free_select:
				_cursor = IDCUR_LASSO;
				break;
			case btn_rect:
				_cursor = IDCUR_RECT;
				break;
			case btn_magic:
				_cursor = IDCUR_MAGIC;
				break;
			case btn_eraser:
				_cursor = IDCUR_ERASER;
				break;
			case btn_sticker:
				_cursor = id_arrow;
				break;
			case btn_draw_free:
				_cursor = IDCUR_PENCIL;
				break;
			case btn_draw_picker:
				_cursor = IDCUR_PICKER;
				break;
			case btn_draw_eraser:
				_cursor = IDCUR_ERASER;
				break;
			case btn_draw_fill:
				_cursor = IDCUR_FILL;
				break;
			case btn_draw_magic:
				_cursor = IDCUR_MAGIC;
				break;
			case btn_draw_straight:
				_cursor = IDCUR_CURSOR;
				break;
			case btn_draw_arrow:
				_cursor = IDCUR_CURSOR;
				break;
			case btn_draw_rect:
				_cursor = IDCUR_RECT;
				break;
			case btn_seg_rect:
				_cursor = IDCUR_RECT;
				break;
			case btn_draw_circle:
				_cursor = IDCUR_ELLIPSE;
				break;
			case btn_crop:
				_cursor = IDCUR_CROP;
				break;
			case btn_draw_select:
				_cursor = IDCUR_SELECT;
				break;
			case btn_paste:
				_cursor = id_arrow;
				break;
			case btn_seg_fgd:
				_cursor = IDCUR_GC_FGD;
				break;
			case btn_seg_bgd:
				_cursor = IDCUR_GC_BGD;
				break;
			default:
				_cursor = id_arrow;
				break;
			}

			if (_pointer_btn == btn_move)
			{
				if (x > _dst_in_frame.x && x < _dst_in_frame.x + _dst.cols &&
					y > _dst_in_frame.y && y < _dst_in_frame.y + _dst.rows)
				{
					_cursor = IDCUR_HAND;
				}
				else
				{
					_cursor = id_arrow;
				}
			}
		}
	}

	if (!_clip_mat.empty())
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);
		if (at_rect(pt, _rect_paste))
		{
			_cursor = IDCUR_MOVE;
		}
	}

	int top3 = TOOLBAR_TOP_HEIGHT;// +_off_top + TOOLBAR_BTN_STEP_Y;
	int lf3 = get_toolbar_lf();

	if (!_down_nav_click)
	{
		if (_nav_btn == nav_filter)
		{
			if (x > lf3 && x < lf3 + _combo_width_filter && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 9 + 30)
			{
				_cursor = id_arrow;
			}
		}
		else if (_nav_btn == nav_draw)
		{
			if (x > lf3 && x < lf3 + _combo_width_draw && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 8)
			{
				_cursor = id_arrow;
			}
		}
		else if (_nav_btn == nav_sticker)
		{
			if (x > lf3 && x < lf3 + _combo_width_sticker && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 2)
			{
				_cursor = id_arrow;
			}
		}
		else if (_nav_btn == nav_segment)
		{
			if (x > lf3 && x < lf3 + _combo_width_seg && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 6)
			{
				_cursor = id_arrow;
			}
		}
		else if (_nav_btn == nav_inp)
		{
			if (x > lf3 && x < lf3 + _combo_width_inp && y > top3 && y < _win_h) //top3 + TOOLBAR_MEN_STEP_Y * 5)
			{
				_cursor = id_arrow;
			}
		}
	}

	top3 = TOOLBAR_TOP_HEIGHT;

	if (!_inpaint_click && _down_nav_click)
	{
		int step = MENU_BTN_STEP;
		if (x > _lf_nav && x < _lf_nav + _combo_width_nav && y > top3 && y < top3 + step * 5)
		{
			_cursor = id_arrow;
		}
	}

	top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;

	if (_down_saveas_click)
	{
		if (_app_type == FROM_EXE)
		{
			int lf = _lf_saveas_down - saveas_ico_idle.cols + down_over.cols;

			//	int lf = _lf_saveas_down - copy_idle.cols - 2;
			if (x > lf && x < lf + saveas_ico_idle.cols && y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + saveas_ico_idle.rows)
			{
				_cursor = id_arrow;
			}
		}
		else
		{
			//int lf = _win_w + TOOLBAR_RIGHT_WIDTH - 80;
			int lf = _lf_saveas_down - saveas_idle.cols + down_over.cols;

			if ((x > lf && x < lf + saveas_idle.cols) && (y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + saveas_idle.rows))
			{
				_cursor = id_arrow;
			}
		}
	}

	if (_down_cut_click)
	{
		int lf = _lf_copy - copy_idle.cols - 2;
		if (x > lf && x < lf + cut_idle.cols
			&& y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + cut_idle.rows)
		{
			_cursor = id_arrow;
		}
	}

	if (_opt == btn_rota_bar)
	{
		int off = 5;
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;
		int lf = 8 + _combo_width_draw;

		if (x > lf && x < lf + (crop_custom_idle.cols + off) * 5 && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
		{
			_cursor = id_arrow;
		}
	}
	else if (_opt == btn_crop_bar)
	{
		int off = 5;
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y + TOOLBAR_MEN_STEP_Y * 6;
		int lf = 8 + _combo_width_draw;

		if (x > lf && x < lf + (crop_custom_idle.cols + off) * 7 && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y)
		{
			_cursor = id_arrow;
		}
	}

	if (_help_click == menu_help)
	{
		int lf0 = _win_w + TOOLBAR_RIGHT_WIDTH - 15 - _combo_width_help;
		if (x > lf0 && x < lf0 + _combo_width_help &&
			y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + TOOLBAR_MEN_STEP_Y * 4)
		{
			_cursor = id_arrow;
		}
	}
#pragma endregion

#pragma region tips  

	//if (_opt == btn_change_bg)
	//{
	//	cur_xy = 0;
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
	//		cur_xy = btn_change_bg_ok;
	//	}
	//}
	//if (cur_xy == btn_change_bg_ok && _opt != btn_change_bg)
	//{
	//	cur_xy = 0;
	//}

	//if (cur_xy != btn_change_bg_ok)
	{
		if (x > TOOLBAR_BTN_LEFT && x <TOOLBAR_BTN_LEFT + 32 && y > _win_h + (FOOTER_HEIGHT - loc_idle.rows) / 2)
		{
			if (_fcoi != L"")
			{
				cur_xy = btn_loc;
			}
		}
		else if (y > 0 && y < TOOLBAR_TOP_HEIGHT)
		{
			cur_xy = 0;
			if (x > _lf_undo + TOOLBAR_BTN_STEP_X * 4 + 24 && x < _lf_undo + TOOLBAR_BTN_STEP_X * 4 + 24 + 32)
			{
				cur_xy = 0;
			}

			if (_rect_select.width != 0)
			{
				if (x > _lf_copy - TOOLBAR_BTN_STEP_X + 12 && x < _lf_copy - TOOLBAR_BTN_STEP_X + 12 + 32)
				{
					cur_xy = btn_copy;
				}
			}

			if (x > _lf_undo + TOOLBAR_BTN_STEP_X * 2 + 24 && x < _lf_undo + TOOLBAR_BTN_STEP_X * 2 + 24 + 32)
			{
				cur_xy = toolbar_one;
				//cur_xy = toolbar_load;
			}
			else if (x > _lf_undo + TOOLBAR_BTN_STEP_X && x < _lf_undo + TOOLBAR_BTN_STEP_X + 32)
			{
				cur_xy = toolbar_redo;
			}
			else if (x > _lf_undo && x < _lf_undo + 32)
			{
				cur_xy = toolbar_undo;
			}
			else if (x > _lf_move + TOOLBAR_BTN_STEP_X && x < _lf_move + TOOLBAR_BTN_STEP_X + 32)
			{
				cur_xy = btn_paste;
			}
			else if (x > _lf_move && x < _lf_move + 32)
			{
				cur_xy = btn_move;
			}
			else if (x > _lf_apply && x < _lf_apply + 32)
			{
				if (!_sticker_arr.empty())
				{
					cur_xy = btn_sticker_apply;
				}
			}

			if (_app_type == FROM_EXE)
			{
				if (x > _lf_save && x < _lf_save + 32)
				{
					cur_xy = toolbar_save;
				}

				if (x > _lf_open && x < _lf_open + 32)
				{
					cur_xy = toolbar_open;
				}
			}
		}
		else if (y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y * 9)
		{
			int lf3 = TOOLBAR_SUB_OFF + _left_width;
			int lf4 = lf3 + _combo_width_draw / 2;
			int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;


			if (x > lf3 && x < lf4)
			{
				if (_nav_btn == nav_draw && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 8)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 7)
					{
						if (_opt == btn_crop_bar)
						{
							cur_xy = 0;
						}
						else
						{
							cur_xy = btn_crop_bar;
						}
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 6)
					{
						cur_xy = btn_draw_magic;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 5 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = btn_draw_color;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = btn_draw_eraser;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3)
					{
						cur_xy = btn_draw_picker;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = btn_draw_rect;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					{
						cur_xy = btn_draw_straight;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_draw_free;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_inp && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						//cur_xy = btn_inpaint;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3)
					{
						cur_xy = btn_optimize;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = btn_magic;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					{
						cur_xy = btn_free_select;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_inp_tips;
					}
					else if (y > top3)
					{
						cur_xy = 0;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_segment && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 6)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = btn_change_bg;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					{
						cur_xy = btn_seg_fgd;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_seg_tips;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_sticker && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					{
						cur_xy = 0;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_sticker_tips;
					}
					else
					{
						cur_xy = 0;
					}
				}
			}
			else if (x > lf4 && x < lf4 + _combo_width_draw / 2)
			{
				if (_nav_btn == nav_draw && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 8)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 7)
					{
						if (_opt == btn_rota_bar)
						{
							cur_xy = 0;
						}
						else
						{
							cur_xy = btn_rota_bar;
						}
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 6)
					{
						cur_xy = btn_input_bg;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = btn_draw_text;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = btn_draw_mosaic;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3)
					{
						cur_xy = btn_draw_fill;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = btn_draw_circle;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					{
						cur_xy = btn_draw_arrow;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_draw_select;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_inp && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 3)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = btn_eraser;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 1 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 1)
					{
						cur_xy = btn_rect;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_brush;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_segment && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 4 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 1)
					{
						cur_xy = btn_seg_bgd;
					}
					else if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_seg_rect;
					}
					else
					{
						cur_xy = 0;
					}
				}
				else if (_nav_btn == nav_sticker && !_down_nav_click)
				{
					if (y > top3 + 32)
					{
						cur_xy = 0;
					}
					else if (y > top3)
					{
						cur_xy = btn_sticker;
					}
					else
					{
						cur_xy = 0;
					}
				}
			}
			else
			{
				cur_xy = 0;
			}

			if (x > lf3 + 5 && x < lf3 + sky_draw_idle.cols + 5)
			{
				if (_nav_btn == nav_filter && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 8 + 20 && y < top3 + TOOLBAR_MEN_STEP_Y * 8 + 20 + sky_draw_idle.rows)
					{
						cur_xy = btn_draw_sky;
					}
					else { cur_xy = 0; }
				}
			}

			if (x > lf3 && x < lf3 + _combo_width_inp)
			{
				if (_nav_btn == nav_inp && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = btn_inpaint;
					}
				}
				else if (_nav_btn == nav_segment && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y * 6)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 5)
					{
						cur_xy = btn_change_bg;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 4)
					{
						cur_xy = 0;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 3)
					{
						cur_xy = btn_sticker_ok;
					}
					else if (y > top3 + TOOLBAR_MEN_STEP_Y * 2)
					{
						cur_xy = btn_seg_trans;
					}
				}
				else if (_nav_btn == nav_sticker && !_down_nav_click)
				{
					if (y > top3 + TOOLBAR_MEN_STEP_Y + 32)
					{
						cur_xy = 0;
					}
					//else if (y > top3 + TOOLBAR_MEN_STEP_Y)
					//{
					//	cur_xy = btn_sticker_apply;
					//}
				}
			}
		}
		else
		{
			hit = 0;
		}
	}

	//if (_lena2.empty())
	//{
	//	cur_xy = 0;
	//	if (x > (_win_w - lbl_drag_idle.cols) / 2 + lbl_drag_idle.cols + 8 &&
	//		x < (_win_w - lbl_drag_idle.cols) / 2 + lbl_drag_idle.cols + 8 + 32 &&
	//		y >(_win_h - firstopen_idle.rows) / 2 &&
	//		y < (_win_h - firstopen_idle.rows) / 2 + 32)
	//	{
	//		cur_xy = btn_firstopen;
	//	}
	//	else
	//	{
	//		hit = 0;
	//	}
	//}

	if (pre_xy == cur_xy)
	{
		hit++;
	}
	else
	{
		pre_xy = cur_xy;
		hit = 0;
	}
#pragma endregion

#pragma region tips show  
	if (hit > 5)
	{
		int off = 0;
		int lf3 = TOOLBAR_BTN_LEFT + 32 + 12;
		int top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;

		string txt = "";
		_has_tips = true;

		lf3 = TOOLBAR_BTN_LEFT;
		top3 = TOOLBAR_TOP_HEIGHT + 4;
		switch (cur_xy)
		{
		case btn_loc:
		{
			lf3 = TOOLBAR_BTN_LEFT;
			top3 = _win_h - 8 - loc_idle.rows;
			wstring p = _fcoi;
			Replace(p, L"/", L"\\");
			txt = _w2u(p);
		}
		break;
		case toolbar_open:
			lf3 = _lf_open;
			txt = _lang.trans("Open...");
			break;
		case btn_firstopen:
			lf3 = (_win_w - lbl_drag_idle.cols) / 2 + lbl_drag_idle.cols + 8 + 46;
			top3 = (_win_h - firstopen_idle.rows) / 2;
			txt = _lang.trans("Open...");
			break;
		case toolbar_save:
			lf3 = _lf_save;
			txt = _lang.trans("Save");
			break;
		case btn_move:
			lf3 = _lf_move;
			txt = _lang.trans("Move");
			break;
		case btn_paste:
			lf3 = _lf_move + TOOLBAR_BTN_STEP_X;
			//BOOL b7 = IsClipboardFormatAvailable(CF_BITMAP);
			//Scalar color = !b7 ? Scalar::all(107) : Scalar::all(190);
			txt = _lang.trans("Paste");
			break;
		case btn_copy:
			lf3 = _lf_copy - TOOLBAR_BTN_STEP_X + 16;
			txt = _lang.trans("Copy");
			break;
		case toolbar_undo:
			lf3 = _lf_undo;
			txt = _lang.trans("Undo");
			break;
		case toolbar_redo:
			lf3 = _lf_undo + TOOLBAR_BTN_STEP_X;
			txt = _lang.trans("Redo");
			break;
		case toolbar_one:
			lf3 = _lf_undo + TOOLBAR_BTN_STEP_X * 2 + 12;
			txt = _lang.trans("Center");
			break;
		case btn_sticker_apply:
			lf3 = _lf_apply;
			txt = _lang.trans("Complete image fusion");
			break;
		default:
			break;
		}
		if (txt != "")
		{
			draw_tip_txt(lf3, top3 + off, txt.c_str());
		}
		txt = "";
		{
			top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
			lf3 = get_toolbar_lf();

			if (_nav_btn == nav_filter)
			{
				lf3 += _combo_width_filter;
			}
			else if (_nav_btn == nav_draw)
			{
				lf3 += _combo_width_draw;
			}
			else
			{
				lf3 += _combo_width_inp;
			}

			switch (cur_xy)
			{
			case btn_draw_free:
				txt = _lang.trans("Pencil");
				break;
			case btn_draw_straight:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Line");
				break;
			case btn_draw_rect:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans("Rectangle");
				break;
			case btn_draw_picker:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("Color Picker");
				break;
			case btn_draw_eraser:
				top3 += TOOLBAR_MEN_STEP_Y * 4;
				txt = _lang.trans("Eraser");
				break;
			case btn_draw_select:
				txt = _lang.trans("Selection");
				break;
			case btn_draw_mosaic:
				top3 += TOOLBAR_MEN_STEP_Y * 4;
				txt = _lang.trans("Mosaic");
				break;
			case btn_draw_text:
				top3 += TOOLBAR_MEN_STEP_Y * 5;
				txt = _lang.trans("Text");
				break;
			case btn_draw_arrow:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Arrow");
				break;
			case btn_draw_circle:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans("Ellipse");
				break;
			case btn_draw_fill:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("Color Fill");
				break;
			case btn_crop_bar:
				top3 += TOOLBAR_MEN_STEP_Y * 7;
				txt = _lang.trans("Crop");
				break;
			case btn_rota_bar:
				top3 += TOOLBAR_MEN_STEP_Y * 7;
				txt = _lang.trans("Rotate");
				break;
			case btn_draw_color:
			{
				int r = _picker_val[2];
				int g = _picker_val[1];
				int b = _picker_val[0];

				char buf[200] = { 0 };
				sprintf_s(buf, 200, ": RGB(%d,%d,%d)", r, g, b);

				top3 += TOOLBAR_MEN_STEP_Y * 5;
				txt = _lang.trans("Color") + buf;

				if (_opt == btn_draw_text)
				{
					memset(buf, 0, 200);
					sprintf_s(buf, 200, ": %s, %d", _w2u(_input_lf.lfFaceName).c_str(), _input_lf.lfHeight);
					txt += ", " + _lang.trans("Font") + buf;
				}
			}
			break;
			case btn_input_bg:
				top3 += TOOLBAR_MEN_STEP_Y * 6;
				txt = _lang.trans("Background");
				break;
			case btn_draw_magic:
				top3 += TOOLBAR_MEN_STEP_Y * 6;
				txt = _lang.trans("Transparent");
				break;
			case btn_draw_sky:
				top3 += TOOLBAR_MEN_STEP_Y * 8 + 20;
				txt = _lang.trans("Change Sky Background");
				break;
			case btn_seg_tips:
			{
				int w = min(seg_sample.cols, _win_w - lf3 - 2);
				Mat t = seg_sample(Rect(0, 0, w, seg_sample.rows));
				t.copyTo(_frame(Rect(lf3, top3, t.cols, t.rows)));
			}
			break;
			case btn_inp_tips:
			{
				int w = min(inp_sample.cols, _win_w - lf3 - 2);
				Mat t = inp_sample(Rect(0, 0, w, inp_sample.rows));
				t.copyTo(_frame(Rect(lf3, top3, t.cols, t.rows)));
			}
			break;
			case btn_brush:
				txt = _lang.trans("Brush");
				break;
			case btn_free_select:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Lasso");
				break;
			case btn_rect:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Rectangle");
				break;
			case btn_magic:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans("Magic Wand");
				break;
			case btn_eraser:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans("Eraser");
				break;
			case btn_optimize:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("Optimize");//Remove unnecessary Objects
				break;
			case btn_inpaint:
				top3 += TOOLBAR_MEN_STEP_Y * 4;
				txt = _lang.trans("Remove smeared objects");//Remove Unwanted Objects
				break;
			case btn_seg_apply:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("btn_seg_apply");
				break;
			case crop_0_0:
				txt = _lang.trans("Crop");
				break;
			case btn_rota270:
				txt = _lang.trans(_w2u(L"Rotate left").c_str());
				break;
			case btn_rota90:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans(_w2u(L"Rotate right").c_str());
				break;
			case btn_rota180:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans(_w2u(L"Rotate by 180°").c_str());
				break;
			case btn_fliph:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("Flip");// horizonta
				break;
			case btn_flipv:
				top3 += TOOLBAR_MEN_STEP_Y * 4;
				txt = _lang.trans("Mirror");// vertical
				break;
			case btn_seg_rect:
				txt = _lang.trans("Select a rectangular area around the object you want to segment");
				break;
			case btn_sticker:
				txt = _lang.trans("Select sticker");
				break;
			case btn_sticker_tips:
			{
				int w = min(sticker_sample.cols, _win_w - lf3 - 2);
				Mat t = sticker_sample(Rect(0, 0, w, sticker_sample.rows));
				t.copyTo(_frame(Rect(lf3, top3, t.cols, t.rows)));
			}
			break;
			//case btn_sticker_apply:
			//	top3 += TOOLBAR_MEN_STEP_Y;
			//	txt = _lang.trans("Complete image fusion");
			//	break;
			case btn_seg_fgd:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Retain");
				break;
			case btn_seg_bgd:
				top3 += TOOLBAR_MEN_STEP_Y;
				txt = _lang.trans("Remove");
				break;
			case btn_sticker_ok:
				top3 += TOOLBAR_MEN_STEP_Y * 3;
				txt = _lang.trans("Finish the cutting");
				break;
			case btn_change_bg_ok:
			{
				//txt = "";
				//lf3 = _dst.cols + _dst_in_frame.x - lbl_fusion.cols;
				//top3 = _dst.rows - 70 + _dst_in_frame.y;
				//lbl_fusion.copyTo(_frame(Rect(lf3, top3, lbl_fusion.cols, lbl_fusion.rows)));
			}
			break;
			case btn_seg_trans:
				top3 += TOOLBAR_MEN_STEP_Y * 2;
				txt = _lang.trans("Transparent");
				break;
			case btn_change_bg:
				top3 += TOOLBAR_MEN_STEP_Y * 5;
				txt = _lang.trans("Change Background");
				break;
			default:
				break;
			}
			if (txt != "")
			{
				draw_tip_txt(lf3, top3 + off, txt.c_str());
			}
		}
	}
	else
	{
		if (_has_tips)
		{
			_has_tips = false;
			if (_working != act_display)
			{
				_working = act_tips_ui;
			}
		}
	}
#pragma endregion

#pragma region SetCursor  
	if (_lena2.empty())
	{
		_cursor = id_arrow;
	}

	if (_hInputWnd != NULL)
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);

		if (pt.x > _input_win_in_dst.x && pt.x < _input_win_in_dst.x + _input_win_in_dst.width
			&& pt.y > _input_win_in_dst.y && pt.y < _input_win_in_dst.y + _input_win_in_dst.height)
		{
			_cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_IBEAM));
		}
		else
		{
			get_crop_cursor(_input_win_in_dst, _cursor);
		}
	}

	if (_rect_crop.width != 0)
	{
		get_crop_cursor(_rect_crop, _cursor);
	}

	if (_rect_select.width != 0)
	{
		get_crop_cursor(_rect_select, _cursor);
	}

	if (!_sticker_arr.empty())
	{
		_cursor = id_arrow;

		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);

		int nlen = (int)_sticker_arr.size();
		for (int i = 0; i < nlen; i++)
		{
			if (_sticker_arr[i].id == _curr_sticker_id)
			{
				Rect rc = _sticker_arr[i].rc;
				Mat paper = _sticker_arr[i].dst;

				Rect rc2 = Rect(rc.x, rc.y, paper.cols * _dst_zoom_w / _lena_w, paper.rows * _dst_zoom_w / _lena_w);

				//bool b = false;
				//if (rc2.x + rc2.width > _dst.cols)
				//{
				//	rc2.x = 10;
				//	b = true;
				//}
				//if (rc2.x + rc2.width > _dst.cols)
				//{
				//	rc2.width = _dst.cols - rc2.x - 10;
				//	b = true;
				//}
				//if (rc2.y + rc2.height > _dst.rows)
				//{
				//	rc2.y = 10;
				//	b = true;
				//}
				//if (rc2.y + rc2.height > _dst.rows)
				//{
				//	rc2.height = _dst.rows - rc2.y - 10;
				//	b = true;
				//}
				//if (b)
				//{
				//	rc2 = Rect(0, 0, rc2.width, rc2.height);
				//}

				if (!is_out_rect(rc2, pt, 5))
				{
					_cursor = IDCUR_MOVE;
				}
				break;
			}
		}


		//if (_opt == btn_change_bg)
		//{
		//	Rect rc;
		//	rc.width = _dst.cols;
		//	rc.height = _dst.rows;
		//	rc.x = _dst_in_frame.x;
		//	rc.y = _dst_in_frame.y;

		//	int x = cvui::mouse().x;
		//	int y = cvui::mouse().y;
		//	Point pt2(x, y);
		//	if (is_crop50(pt2, rc))
		//	{
		//		_cursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_ARROW));
		//	}
		//}

	}

	if (_down_sticker_click)
	{
		if (x > _lf_font && x <  _lf_font + lbl_seamless.cols &&
			y > TOOLBAR_TOP_HEIGHT && y < TOOLBAR_TOP_HEIGHT + TOOLBAR_MEN_STEP_Y * 4)
		{
			_cursor = id_arrow;
		}
	}

	if (_nav_btn == nav_sticker)
	{
		top3 = TOOLBAR_TOP_HEIGHT + _off_top + TOOLBAR_BTN_STEP_Y;
		if (x > lf3 && x < lf3 + _combo_width_sticker && y > top3 && y < top3 + TOOLBAR_MEN_STEP_Y * 2)
		{
			_cursor = id_arrow;
		}
	}

	if (_close_ret >= 1)
	{
		_cursor = id_arrow;
	}

	if (_do_nav_btn >= 1)
	{
		_cursor = id_arrow;
	}

	if (_space_key_mouse)
	{
		_cursor = id_arrow;
	}

	if (x <= 0 || y <= 0 || x >= _win_w + TOOLBAR_RIGHT_WIDTH || y >= _win_h + FOOTER_HEIGHT)
	{
		_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
		return;
	}

	if (_rect_select.width == 0 && _rect_crop.width == 0 && _hInputWnd == NULL)
	{
		CURSORINFO cursorInfo = { 0 };
		cursorInfo.cbSize = sizeof(cursorInfo);

		if (::GetCursorInfo(&cursorInfo))
		{
			if (cursorInfo.hCursor == _hcursor_we || cursorInfo.hCursor == _hcursor_ns ||
				cursorInfo.hCursor == _hcursor_nwse || cursorInfo.hCursor == _hcursor_nesw)
			{
				SetCursor((HCURSOR)LoadCursor(0, IDC_ARROW));
				return;
			}
		}
	}

	if (_inpaint_click)
	{
		Point pt;
		pt.x = TOOLBAR_LEFT_WIDTH + (_win_w - 200 - TOOLBAR_LEFT_WIDTH) / 2;
		pt.y = TOOLBAR_TOP_HEIGHT + (_win_h - 30 - TOOLBAR_TOP_HEIGHT) / 2;

		if (x <= pt.x + 300 && y <= pt.y + 30 && x >= pt.x && y >= pt.y)
		{
			_cursor = id_arrow;
		}
	}

	if (_currCursor != _cursor)
	{
		_currCursor = _cursor;
		if (_cursor >= id_arrow)
		{
			SetCursor((HCURSOR)LoadCursor(0, MAKEINTRESOURCE(_cursor)));
		}
		else
		{
			SetCursor((HCURSOR)LoadCursor(_his, MAKEINTRESOURCE(_cursor)));
		}
	}
#pragma endregion

}
