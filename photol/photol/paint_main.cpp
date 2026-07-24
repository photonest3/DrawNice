///////////////////////////////////////////////////////////////////////
/// @file paint_main.cpp
/// @brief PhotoNest 绘图模块主实现文件
/// @details 实现 CPaint 类的构造函数和核心功能:
///           - 初始化绘图参数(画笔粗细、橡皮擦大小等)
///           - 设置修复算法参数(连通性、掩码值等)
///           - 初始化线程池(10 个线程)
///           - 管理绘图状态和导航按钮
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的核心实现文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include <process.h>
#include "resource.h"
#include <fstream>
#include <shellapi.h>
//#include "MySoft2.h"
extern HINSTANCE _his;

CPaint::CPaint(void)
{
	_dst_zoom_w = 0;
	_dst_zoom_h = 0;
	_inpaint_thickness = 16;
	_draw_thickness = 6;
	_erase_thickness = 26;

	_working = act_nothing;
	_opt = 0;
	_pointer_btn = btn_move;
	_lena_w = 0;
	_lena_h = 0;

	_loDiff = 30;
	_upDiff = 30;
	_connectivity = 4;
	_newMaskVal = 255;

	_nav_btn = nav_draw;
	_tool_btn = 0;

	_currCursor = 0;
	_win_handle = NULL;

	_cur_name = L"";
	_app_type = 0;

	_drag_name = L"";
	_firstrun = true;
	_need_refresh = false;
	_angleValue = 0;
	_angleValue_change = 0;
	_pool = new ThreadPool(10);

	_hParent = NULL;

	_idx_file = 0;
	_len_files = 0;
	_first_load = false;
	_save_click = false;

	_combo_width_help = 80;
	_combo_width_draw = 5;

	PRODUCT_NAME = L"PhotoNest Cutout";
	WINDOW_NAME = "PhotoNest Cutout";

	_ov = 1;

	_draw_rgb = RGB(229, 91, 22);
	_picker_val = Scalar(22, 91, 229);

	_is_in_combo = false;
	_down_saveas_click = false;
	_down_cut_click = false;
	_down_sticker_click = false;

	_exist_coi = false;

	_hInputWnd = NULL;
	_for_font = false;
	_for_select = false;
	_input_bg_trans = 1;
	_input_bg_rgb = RGB(255, 255, 255);

	_nav_data = "";
	_cur_xy = 0;

	_hDC = CreateCompatibleDC(0);
	_lf_font = 0;
	_lf_copy = 0;
	_lf_undo = 0;
	_lf_move = 0;
	_lf_nav = 0;
	_lf_open = 0;
	_lf_save = 0;
	_lf_saveas_down = 0;

	_seamless_mode = NORMAL_CLONE;
	_show_error_times = 100;
	_curr_sticker_id = 0;

	_input_off = 20;
	TOOLBAR_BTN_STEP_X = 40;
	_closed = false;

	for (int i = 0; i < 100; i++)
	{
		threads[i].hdl = 0;
		threads[i].stop = false;
		threads[i].contours1 = 1;
		threads[i].step = 0;
		threads[i].opt = 0;
	}

	_hcursor_we = (HCURSOR)LoadImage(0, IDC_SIZEWE, IMAGE_CURSOR, 0, 0, LR_SHARED);
	_hcursor_ns = (HCURSOR)LoadImage(0, IDC_SIZENS, IMAGE_CURSOR, 0, 0, LR_SHARED);
	_hcursor_nwse = (HCURSOR)LoadImage(0, IDC_SIZENWSE, IMAGE_CURSOR, 0, 0, LR_SHARED);
	_hcursor_nesw = (HCURSOR)LoadImage(0, IDC_SIZENESW, IMAGE_CURSOR, 0, 0, LR_SHARED);

	FOOTER_HEIGHT = 30;
	TOOLBAR_TOP_HEIGHT = 40;
	_left_width = 0;
	_off_top = -20;//40;

	_language = L"en";

	InitializeCriticalSection(&_cs);
	_down_nav_click = false;
	_lenaSize = Size(1, 1);
	_lenaf = 1.0f;

	_fcoi = L"";
}

CPaint::~CPaint(void)
{
	DeleteDC(_hDC);
	delete _pool;
	DeleteCriticalSection(&_cs);
}

void CPaint::handleSizeDrag(int x, int y, int width, int height, int flags, wchar_t* pfname, char* nav, void* param)
{
	CPaint* pThis = (CPaint*)param;
	if (pThis->_firstrun)
	{
		return;
	}

	if (pThis->_close_ret >= 1 || pThis->_do_nav_btn > 0)
	{
		//return;
	}
	//pThis->WritePrivate("handleSizeDrag", flags);

	if (pThis->_app_type == FROM_EXE)
	{
		if (flags == 0)
		{
			//normal
			if ((pThis->_win_w != width - TOOLBAR_RIGHT_WIDTH || pThis->_win_h != height - pThis->FOOTER_HEIGHT) &&
				width > 0 && height > 0)
			{
				pThis->set_winsize(NULL, width, height);
				pThis->high_displayImage();
				pThis->draw_ui();

				cvui::update();
				imshow(pThis->WINDOW_NAME.c_str(), pThis->_frame);
			}
		}
		//else if (flags == 1)
		//{
		//	if (width > 0 && height > 0)
		//	{
		//		//max
		//		pThis->set_winsize(NULL, width, height);
		//		pThis->high_displayImage();
		//		pThis->draw_ui();

		//		cvui::update();
		//		imshow(pThis->WINDOW_NAME.c_str(), pThis->_frame);
		//	}
		//}
		//else if (flags == 2)
		//{
		//	//WM_SIZE
		//	if (width > 0 && height > 0)
		//	{
		//		//restore
		//		HWND pParent = ::GetParent(pThis->_win_handle);
		//		MoveWindow(pParent, x, y, width, height, TRUE);
		//	}
		//}
		//else if (flags == 3)
		//{
		//	//SC_MAXIMIZE
		//	////before max
		//	_beginthreadex(NULL, 0, max_cmd, pThis, NULL, NULL);
		//}
		else if (flags == 10)
		{

			//WM_DROPFILES
			bool b = pThis->show_unapplied();
			if (!b)
			{
				const wchar_t* p0 = (wchar_t*)pfname;
				if (p0 != NULL)
				{
					if (wcslen(p0) > 0 && wcscmp(pThis->_drag_name.c_str(), p0) != 0)
					{
						pThis->reset_frame();
						pThis->_drag_name = p0;
						pThis->load_image("first", p0);
						pThis->_curr_nav_dto.file_name = p0;

						wstring s = pThis->_drag_name + wstring(L" - ") + pThis->PRODUCT_NAME;
						HWND pParent = ::GetParent(pThis->_win_handle);
						::SetWindowText(pParent, s.c_str());

						SetForegroundWindow(pThis->_win_handle);
					}
				}
			}
		}
		else if (flags == 99)
		{
			//WM_CLOSE
			pThis->_close_ret = 1;
		}
	}
	else
	{
		if (flags == 0)
		{
			//WM_PAINT_SIZE
			pThis->on_resize_cef();
		}
	}

	if (flags == 96)
	{
		//WM_DPICHANGED
		//pThis->WritePrivate("WM_DPICHANGED", width);
		pThis->load_ico(width);
		pThis->reset_frame();
		pThis->_working = act_display;
	}
	else if (flags == 996)
	{
		//WM_PAINT_NAV
		pThis->_nav_data = nav;

	}
	else if (flags == 997)
	{
		//WM_CEF_CLOSE
		pThis->_close_ret = 100;
	}
	else if (flags == '+')
	{
		bool b = pThis->isdo_nothing();
		if (b)
		{
			return;
		}

		pThis->_tool_btn = toolbar_zoom_in;
	}
	else if (flags == '-')
	{
		bool b = pThis->isdo_nothing();
		if (b)
		{
			return;
		}

		pThis->_tool_btn = toolbar_zoom_out;
	}
	else if (flags == 'C')
	{
		Rect rc;
		pThis->copy_clipboard(rc);
	}
	else if (flags == 'V')
	{
		pThis->paste_clipboard();
		pThis->_working = act_display;
	}
	else if (flags == 'S')
	{
		pThis->paste_ok(pThis->_weightedImage);

		bool b = pThis->show_unapplied();
		if (!b)
		{
			pThis->_tool_btn = toolbar_save;
		}
	}
	else if (flags == 'Z')
	{
		pThis->paste_ok(pThis->_weightedImage);
		pThis->_tool_btn = toolbar_undo;
	}
	else if (flags == 'Y')
	{
		pThis->paste_ok(pThis->_weightedImage);
		pThis->_tool_btn = toolbar_redo;
	}
	else if (flags == '1')
	{
		//space key down, for move image
		pThis->_lastKey = 32;
	}
	else if (flags == '0')
	{
		//space key up
		pThis->_lastKey = 0;
	}
	else if (flags == 'X')
	{
		if (pThis->_rect_select.width > 0)
		{
			Rect rc;
			pThis->copy_clipboard(rc);
			pThis->trans_apply();
		}
	}
	else if (flags == VK_DELETE)
	{
		pThis->trans_apply();
	}
	else if (flags == 998)
	{
		pThis->move_input_wnd();
	}
}

int CPaint::show(HWND hParent, int w, int h, const wchar_t* fname, const wchar_t* enckey)
{
	if (wcslen(enckey) > 0)
	{
	}
	_BF.SetPassword((char*)_w2u(enckey).c_str());


	int flag = CV_WINDOW_AUTOSIZE;//in gallery
	if (_app_type == FROM_EXE)
	{
		//only exe
		flag = CV_WINDOW_NORMAL;
	}

	cvui::init(WINDOW_NAME.c_str(), 20, true, flag);
	_win_handle = (HWND)cvGetWindowHandle(WINDOW_NAME.c_str());
	HWND pParent = ::GetParent(_win_handle);

	_opt_arr.clear();
	set_winsize(hParent, w, h);

	if (_app_type == FROM_EXE)
	{
		//if (_lena2.empty())
		//{
		//	//wstring str = PRODUCT_NAME;
		//	//str += L" - ";
		//	//str += _u2w(_lang.trans("Remove unwanted objects"));
		//	::SetWindowText(pParent, PRODUCT_NAME.c_str());
		//}
		//else
		//{
			//::SetWindowText(pParent, PRODUCT_NAME.c_str());
		//}

		HICON hbig = (HICON)LoadImage(_his, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 32, 32, 0);
		HICON hsmall = (HICON)LoadImage(_his, MAKEINTRESOURCE(IDI_SMALL), IMAGE_ICON, 16, 16, 0);

		PostMessage(pParent, WM_SETICON, ICON_BIG, (LPARAM)hbig);
		PostMessage(pParent, WM_SETICON, ICON_SMALL, (LPARAM)hsmall);
	}

	load_image("", NULL);

	if (fname != NULL)
	{
		if (_app_type == FROM_EXE)
		{
			_fname = fname;
		}
	}


	cvSetSizeDragCallback(WINDOW_NAME.c_str(), handleSizeDrag, this);
	if (IsWindow(hParent))
	{
		::SetParent(pParent, hParent);
		_hParent = hParent;
		PostMessage(hParent, WM_PAINT_INIT, 0, (LPARAM)pParent);
		//::ShowWindow(pParent, SW_HIDE);
	}

	int _pre_xy = 0;
	int _hit = 0;

	while (true)
	{
		if (_app_type != FROM_EXE)
		{
			if (!_nav_data.empty())
			{
				//WritePrivate("_nav_data", _nav_data.c_str());
				if (_lstFile.empty())
				{
					vector<string> v1 = tokenize(_nav_data, "\r\n", true, "");
					int len = (int)v1.size();
					for (int i = 0; i < len; i++)
					{
						_lstFile.push_back(_u2w(v1[i]));
					}

					_len_files = (int)_lstFile.size();
					_idx_file = 0;

					list<wstring>::iterator it;
					for (it = _lstFile.begin(); it != _lstFile.end(); it++)
					{
						praser_file(*it);
						_idx_file++;
						if (fname != NULL && _curr_nav_dto.id == fname)
						{
							break;
						}
					}
				}
			}
		}

		if (_close_ret == 100)
		{
			break;
		}

		if (!IsWindow(GetParent((HWND)cvGetWindowHandle(WINDOW_NAME.c_str()))))
		{
			//_close_ret = 1;
			break;
		}

		switch (_opt_rotate)
		{
		case btn_flipv:
			myflip(btn_flipv);
			break;
		case btn_fliph:
			myflip(btn_fliph);
			break;
		default:
			break;
		}

		if (_angleValue_change != 0)
		{
			_angleValue_change = 0;
			opt_t op;
			op.opt_type = btn_rota;
			op.nav_type = 0;
			op.thickness = _angleValue;
			add_opt(op);

			_total_deg += _angleValue;
			rotate(_angleValue);
			_working = act_display;
		}

		if (_filter_changed)
		{
			if (_fVal.use_brightValue || _fVal.use_contrastValue)
			{
				filter_ok(1);
				displayImage(100, _dst);
			}
			_filter_mousedown = true;
			_filter_changed = false;
		}

		if (_filter_mousedown && _filter_mouseup)
		{
			_filter_mousedown = false;
			_filter_mouseup = false;
			filter_ok(2);
			_working = act_display;
		}

		if (_opt_rotate != 0)
		{
			_working = act_display;
			_opt_rotate = 0;
		}

		if (_working == act_display)
		{
			high_displayImage();
			_working = act_nothing;
		}
		else if (_working == act_tips_ui)
		{
			reset_frame();
			if (!_dst_tips.empty())
			{
				if (_dst_in_frame.x >= 0 && _dst_in_frame.x + _dst_tips.cols <= _frame.cols &&
					_dst_in_frame.y >= 0 && _dst_in_frame.y + _dst_tips.rows <= _frame.rows)
				{
					Mat imageROI = _frame(Rect(_dst_in_frame.x, _dst_in_frame.y, _dst_tips.cols, _dst_tips.rows));
					_dst_tips.copyTo(imageROI);
				}
			}

			if (_rect_crop.width >= 60 && _rect_crop.height >= 60)
			{
				Rect rc = _rect_crop;
				rc.x = _dst_in_frame.x + rc.x;
				rc.y = _dst_in_frame.y + rc.y;
				show_rect(_frame, rc, btn_crop);
			}

			//if (_opt == btn_change_bg)
			//{
			//	Rect rc;
			//	rc.width = _dst.cols;
			//	rc.height = _dst.rows;
			//	rc.x = _dst_in_frame.x;
			//	rc.y = _dst_in_frame.y;
			//	show_rect(_frame, rc, btn_change_bg);
			//}

			if (_pt2_sticker.x != _pt1_sticker.x && _pt2_sticker.y != _pt1_sticker.y)
			{
				drawDashRect(_frame, 5, 5, _pt1_sticker, _pt2_sticker);
			}

			_working = act_nothing;
		}

		switch (_tool_btn)
		{
		case toolbar_open:
			open();
			break;
		case toolbar_cancel:
			if (_handle_thread != INVALID_HANDLE_VALUE)
			{
				t_struct* myt = get_thread();
				myt->stop = true;

				_inpaint_click = false;
				//TerminateThread(_handle_thread, 0);
				CloseHandle(_handle_thread);
				_handle_thread = INVALID_HANDLE_VALUE;

				//myt->step = 0;
				//myt->stop = false;
				//myt->hdl = 0;
			}
			break;
		case toolbar_load:
			load_image("", _cur_name.c_str());
			break;
		case toolbar_save:
			saveas(toolbar_save, hParent);
			break;
		case toolbar_zoom_in:
			zoom(0);
			break;
		case toolbar_zoom_out:
			zoom(1);
			break;
		case toolbar_one:
			zoom(3);
			break;
		case toolbar_undo:
			undo();
			break;
		case toolbar_redraw:
			redraw(_idx);
			break;
		case toolbar_redo:
			redo();
			break;
		default:
			break;
		}
		if (_tool_btn != bth_saveas && _tool_btn != btn_font &&
			_tool_btn != btn_input_bg && _tool_btn != btn_draw_color && _tool_btn != btn_draw_trans && _tool_btn != toolbar_input)
		{
			_tool_btn = 0;
		}

		if (_inpaint_click)
		{
			if (_close_ret >= 1 || _do_nav_btn > 0)
			{
			}
			else
			{
				_inpainting = true;
				_process_nb++;
				draw_processing();
				if (_process_nb >= 10)
				{
					_process_nb = 0;
					if (_handle_thread != INVALID_HANDLE_VALUE)
					{
						t_struct* myt = get_thread();
						//if (myt->step < 90)
						{
							myt->step += 1;
						}
					}
				}
			}
		}
		else
		{
			if (_inpainting)
			{
				_inpainting = false;
				_working = act_display;
			}
		}

		if (cvui::mouse(cvui::LEFT_BUTTON, cvui::DOWN))
		{
			if (!_inpaint_click)
			{
				if (_lastKey == 32)
				{
					mouse_down_r();
				}
				else
				{
					mouse_down();
				}
			}
		}
		if (cvui::mouse(cvui::LEFT_BUTTON, cvui::IS_DOWN)) {
			if (!_inpaint_click)
			{
				if (_lastKey == 32)
				{
					mouse_isdown_r();
				}
				else
				{
					mouse_isdown();
				}
			}
		}

		if (cvui::mouse(cvui::LEFT_BUTTON, cvui::UP))
		{
			_space_key_mouse = false;
			if (!_inpaint_click)
			{
				mouse_up();
			}

		}

		if (cvui::mouse(cvui::WELL_UP)) {
			if (!_inpaint_click)
			{
				mouse_well_up();
			}
		}

		if (cvui::mouse(cvui::WELL_DOWN)) {
			if (!_inpaint_click)
			{
				mouse_well_down();
			}
		}

		if (_changed)
		{
			opt_t op;
			op.opt_type = _opt;
			op.nav_type = 0;
			op.pts = _pts;
			op.color = _picker_val;
			op.thickness = THICKNESS_VAL;

			if (_opt == btn_draw_free || _opt == btn_draw_straight || _opt == btn_draw_arrow || _opt == btn_draw_rect || _opt == btn_draw_circle)
			{
				op.thickness = _draw_thickness;
			}
			else if (_opt == btn_draw_mosaic || _opt == btn_draw_eraser)
			{
				op.thickness = _erase_thickness;
			}
			else if (_opt == btn_brush || _opt == btn_eraser || _opt == btn_free_select)
			{
				op.thickness = _inpaint_thickness;
			}
			else
			{
				op.thickness = _draw_thickness;
			}

			if (_opt == btn_brush || _opt == btn_free_select || _opt == btn_magic || _opt == btn_eraser || _opt == btn_rect)
			{
				op.nav_type = nav_inp;
			}
			else if (_opt == btn_seg_fgd || _opt == btn_seg_bgd || _opt == btn_seg_rect)
			{
				op.nav_type = nav_segment;
			}

			add_opt(op);
			_pts.clear();
			_changed = false;
		}

		draw_ui();
		show_tips(_pre_xy, _cur_xy, _hit);

		switch (_tool_btn)
		{
		case bth_saveas:
		{
			saveas(bth_saveas, hParent);
			_tool_btn = 0;
			_pointer_btn = btn_move;
		}
		break;
		case btn_font:
			set_input_font();
			_tool_btn = 0;
			break;
		case btn_input_bg:
		{
			DialogBoxParam(_his, MAKEINTRESOURCE(IDD_BACKGROUND), _win_handle, (DLGPROC)bg_wnd_proc, (LPARAM)this);
			_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));

			_tool_btn = 0;
		}
		break;
		case btn_draw_color:
			if (_opt == btn_draw_text)
			{
				set_input_font();
			}
			else
			{
				set_draw_color();
			}
			_tool_btn = 0;
			break;
		case toolbar_input:
		{
			input_command(0);
			_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
			_tool_btn = 0;
			_working = act_display;
		}
		break;
		default:
			break;
		}

		switch (_help_btn)
		{
		case 1:
		{
			_help_btn = 0;
			wstring buyURL = L"https://www.photonest.io/index.html#subscribe";
			ShellExecute(NULL, L"open", buyURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		break;
		case 2:
		{
			_help_btn = 0;
			DialogBoxParam(_his, MAKEINTRESOURCE(IDD_DIALOG_SETTINGS), _win_handle, (DLGPROC)Settings, (LPARAM)this);
			_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
			_working = act_display;
		}
		break;
		case 3:
		{
			_help_btn = 0;
			DialogBoxParam(_his, MAKEINTRESOURCE(IDD_DIALOG_REGISTER), _win_handle, (DLGPROC)Register, (LPARAM)this);
			_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
			_working = act_display;
		}
		break;
		case 4:
			_help_btn = 0;
			DialogBoxParam(_his, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), _win_handle, (DLGPROC)About, (LPARAM)this);
			_currCursor = LOWORD(reinterpret_cast<DWORD_PTR>(IDC_HAND));
			_working = act_display;
			break;
		default:
			break;
		}

		if (_help_btn >= 4)
		{
			_help_btn = _help_btn / 10;
		}

		if (_closed)
		{
			if (_app_type != FROM_EXE)
			{
				SendMessage(hParent, WM_PAINT_CLOSE, 0, 0);
			}
			break;
		}

		cvui::update();
		imshow(WINDOW_NAME.c_str(), _frame);

		if (_firstrun)
		{
			_firstrun = false;

			if (IsWindow(hParent))
			{
				int id = 0;
				if (fname != NULL && _app_type == FROM_APP_ONE)
				{
					id = ::_wtoi(fname);
				}

				PostMessage(hParent, WM_PAINT_RUN, id, (LPARAM)pParent);
				SetForegroundWindow(_win_handle);

				//import
				set_winsize(hParent, w, h);
			}
		}

		if (_fname != L"" && !_first_load)
		{
			_first_load = true;
			_working = act_display;
			_beginthreadex(NULL, 0, handle_loadimage2, this, NULL, NULL);
		}

		if (IsWindow(hParent))
		{
			if (!_lstFile.empty() && !_first_load)
			{
				_first_load = true;


				RECT rc;
				GetWindowRect(_hParent, &rc);

				int left = (rc.right - rc.left - _win_w/* - TOOLBAR_RIGHT_WIDTH*/) / 2;
				int top = (rc.bottom - rc.top - _win_h - FOOTER_HEIGHT) / 2;

				if (left < 0)
				{
					left = 0;
				}

				if (top < 30)
				{
					top = 30;
				}

				HWND pParent = ::GetParent(_win_handle);

				MoveWindow(pParent, left, top, _win_w/* + TOOLBAR_LEFT_WIDTH*/, _win_h + FOOTER_HEIGHT, TRUE);
				PostMessage(_hParent, WM_PAINT_MASK, 0, (LPARAM)pParent);

				_working = act_display;
				_beginthreadex(NULL, 0, handle_loadimage, this, NULL, NULL);
			}
		}
	}

	::cvDestroyAllWindows();

	WritePrivate("inpaint_thickness", _inpaint_thickness);
	WritePrivate("draw_thickness", _draw_thickness);
	WritePrivate("erase_thickness", _erase_thickness);
	WritePrivate("tolerance", _upDiff);

	return 0;
}
