///////////////////////////////////////////////////////////////////////
/// @file paint_utils2.cpp
/// @brief 绘图模块 - 高级工具和实用功能实现
/// @details 实现图像修复(Inpaint)、文件系统操作、Shell 集成、
///          配置文件的读取和保存等高级功能
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件提供更高级的图像处理功能和系统交互能力
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
#include "myinpaint.h"
#include <process.h>

bool CPaint::canInpaint()
{
	if (_lena2.empty())
	{
		return false;
	}

	if (_opt_arr.empty())
	{
		return false;
	}
	if (_idx > _opt_arr.size())
	{
		_idx = (int)_opt_arr.size();
	}

	int inp = 0;
	for (int i = 0; i < _idx; i++)
	{
		int opt_type = _opt_arr[i].opt_type;
		if (opt_type == inpaint_ok)
		{
			inp = i;
		}
	}

	if (inp > 0)
	{
		inp = inp + 1;
	}

	bool b = false;
	for (int i = inp; i < _idx; i++)
	{
		int opt_type = _opt_arr[i].opt_type;
		if (opt_type == btn_brush || opt_type == btn_free_select || opt_type == btn_magic || opt_type == btn_rect)
		{
			b = true;
			break;
		}
	}

	return b;
}

bool CPaint::canSave()
{
	if (_lena2.empty())
	{
		return false;
	}

	if (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE)
	{
		if (_exist_coi && _cur_name == _curr_nav_dto.ori_name)
		{
			return true;
		}
	}

	if (!_opt_arr.empty())
	{
		return true;
	}

	return false;
}

bool CPaint::canExit()
{
	if (_lena2.empty())
	{
		return true;
	}

	if (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE)
	{
		if (_exist_coi && _cur_name == _curr_nav_dto.ori_name)
		{
			return false;
		}
	}

	time_t t = 0;
	if (!_opt_arr.empty())
	{
		for (int i = 0; i < _idx; i++)
		{
			t = _opt_arr[i].t;
		}
	}

	bool b = false;
	if (_last_t == t)
	{
		b = true;
	}

	return b;
}

bool CPaint::set_working(int btn, bool state)
{
	if (!_clip_mat.empty())
	{
		paste_ok();
	}

	bool b = show_unapplied(btn, state);
	if (b)
	{
		_working = act_tips_ui;
		return false;
	}

	if (_nav_btn == nav_segment || _nav_btn == nav_sticker)
	{
		clear_opt();

	}

	return set_working2(btn, state, false);
}

bool CPaint::set_working2(int btn, bool state, bool falg)
{
	_pointer_btn = 0;

	if (_nav_btn == btn)
	{
		_pointer_btn = btn_move;
	}
	else
	{
		_opt = _last_opt;
		if (_opt == 0)
		{
			_pointer_btn = btn_move;
		}
	}

	if (falg)
	{
		clear_opt();
	}

	if (state || falg || _nav_btn != btn)
	{
		_opt = 0;
		_last_opt = _opt;
		_pointer_btn = btn_move;
	}
	_rect_crop = Rect(0, 0, 0, 0);
	_rect_select = Rect(0, 0, 0, 0);

	if (_working != act_display)
	{
		_working = act_tips_ui;
	}
	_nav_btn = btn;

	return true;
}

bool CPaint::clear_opt()
{
	_rect_select = Rect(0, 0, 0, 0);
	_rect_crop = Rect(0, 0, 0, 0);
	_rect_paste = Rect(0, 0, 0, 0);
	_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
	_result = Mat::zeros(Size(0, 0), CV_8UC1);
	_rect_seg_select = Rect(0, 0, 0, 0);

	_sticker_arr.clear();
	_pt_prev = Point(0, 0);

	_opt = 0;
	_last_opt = 0;
	_pointer_btn = btn_move;

	if (!_opt_arr.empty() && _opt_arr.size() == 1 && _opt_arr[0].opt_type == btn_init)
	{
		return true;
	}

	if (_nav_btn == nav_inp && !_opt_arr.empty())
	{
		int lastopt = 0;
		int inp = 0;
		for (int i = 0; i < _idx; i++)
		{
			int opt_type = _opt_arr[i].opt_type;
			if (opt_type == inpaint_ok)
			{
				lastopt = opt_type;
				inp = i;
			}
		}

		if (inp > 0)
		{
			inp = inp + 1;
		}

		int idx = _idx;
		for (int i = inp; i < _idx; i++)
		{
			lastopt = _opt_arr[i].opt_type;
			if (lastopt == btn_brush || lastopt == btn_free_select || lastopt == btn_magic || lastopt == btn_eraser || lastopt == btn_rect)
			{
				idx = i;
				break;
			}
		}

		if (idx != _idx)
		{
			erase_lastopt(0, nav_inp);

			_idx = idx;
			_tool_btn = toolbar_redraw;
		}
	}
	else if (_nav_btn != 0 && !_opt_arr.empty())
	{
		bool b = erase_lastopt(0, _nav_btn);
		if (b)
		{
			if (!_opt_arr.empty())
			{
				_idx = (int)_opt_arr.size();
			}
			else
			{
				_idx = 0;
			}
		}
		_mask_idx = -997;
	}
	return true;
}

void CPaint::draw_processing()
{
	t_struct* myt = get_thread();

	if (!_seg_mask.empty())
	{
		if (_process_nb == 10)
		{
			//if (myt->step < 90)
			{
				myt->step += 1;
			}
		}
	}

	int x = TOOLBAR_LEFT_WIDTH + (_win_w - 200 - TOOLBAR_LEFT_WIDTH) / 2;
	int y = TOOLBAR_TOP_HEIGHT + (_win_h - 30 - TOOLBAR_TOP_HEIGHT) / 2;


	Rect rect(x, y, 230, 30);
	rectangle(_frame, rect, Scalar::all(218), -1, LINE_8, 0);

	if (cvui::button(_frame, x + 206, y + 5, stop_idle, stop_over, stop_idle))
	{
		_tool_btn = toolbar_cancel;
	}
	int w = myt->step / myt->contours1;
	if (w > 97)
	{
		w = 97;
	}

	Rect rect2(x, y, w * 2, 30);
	rectangle(_frame, rect2, Scalar(16, 217, 46), -1, LINE_8, 0);

	cvui::printf(_frame, x + 100, y + 10, 0.4, 0x000000, "%d%%", w);

}
void CPaint::show_window(int type, bool& closed)
{

	string txt = "Changes have not been saved";
	if (type == 0 && _close_ret == 1)
	{
		txt = "Do you want to save changes to aa.jpg?";
	}
	else if (type == 1)
	{
		txt = "Not yet completed";
	}
	wstring	tt2 = _u2w(_lang.trans(txt.c_str()));

	if (type == 0 && _close_ret == 1)
	{
		wstring name = L"";
		if (_fcoi != L"")
		{
			wstring::size_type 	n1 = _fcoi.rfind(L"/");
			if (n1 != wstring::npos)
			{
				name = _fcoi.substr(n1 + 1);
			}
		}

		wstring::size_type nFound = wstring::npos;
		nFound = tt2.find(L"aa.jpg", 0);
		if (nFound != wstring::npos)
		{
			tt2.replace(nFound, 6, name);
		}
	}

	Mat lbl = get_lbl(tt2.c_str(), Scalar::all(49), Scalar::all(190), 10);
	int w = max(370, lbl.cols + 180);

	if (!_inpaint_click)
	{
		_pt_close.x = TOOLBAR_LEFT_WIDTH + (_win_w - w - TOOLBAR_LEFT_WIDTH) / 2;
		_pt_close.y = TOOLBAR_TOP_HEIGHT + (_win_h - 180 - TOOLBAR_TOP_HEIGHT) / 2;
	}
	int x = _pt_close.x;
	int y = _pt_close.y;

	_inpaint_click = true;

	Rect rect(x, y, w, 180);
	rectangle(_frame, rect, Scalar::all(49), -1, LINE_8, 0);

	txt = "Close";
	if (type == 1)
	{
		if (_nav_btn == nav_inp)
		{
			txt = "Remove unnecessary Objects";
		}
		else if (_nav_btn == nav_segment)
		{
			txt = "Remove Background from Photo";
		}
		else if (_nav_btn == nav_inp)
		{
			txt = "Image Stitching";
		}
		txt = " ";
	}
	if (_close_ret == 2 || _close_ret == 3)
	{
		txt = " ";
	}

	Mat title = get_lbl(_u2w(_lang.trans(txt.c_str())).c_str(), Scalar::all(33), Scalar::all(128), 10);

	Rect rc(x, y, w, title.rows + 2);
	rectangle(_frame, rc, Scalar::all(33), -1, LINE_8, 0);

	title.copyTo(_frame(Rect(x + 3, y + 1, title.cols, title.rows)));

	line(_frame, Point(x, rc.y + title.rows + 1), Point(x + w - 1, rc.y + title.rows + 1), Scalar::all(74), 1, 8, 0);
	rectangle(_frame, Point(x, y), Point(x + w - 1, rc.y + 180 - 1), Scalar::all(74), 1, LINE_8, 0);

	warning_idle.copyTo(_frame(Rect(x + (w - lbl.cols - 50) / 2, y + 50 - (warning_idle.rows - lbl.rows) / 2, warning_idle.cols, warning_idle.rows)));
	lbl.copyTo(_frame(Rect(x + (w - lbl.cols - 50) / 2 + 50, y + 50, lbl.cols, lbl.rows)));

	if (type == 0 && _close_ret == 1)
	{
		int lf1 = x + (w - yes_idle.cols - no_idle.cols - cancel_idle.cols - 40) / 2;
		int lf2 = lf1 + yes_idle.cols + 20;
		int lf3 = lf1 + yes_idle.cols + no_idle.cols + 40;

		if (cvui::button(_frame, lf1, y + 120, yes2_idle, yes2_over, yes_idle))
		{
			if (!_save_click)
			{
				_save_click = true;
				_beginthreadex(NULL, 0, handle_save, this, NULL, NULL);
			}
		}

		if (cvui::button(_frame, lf2, y + 120, no_idle, no2_over, no_idle))
		{
			_inpaint_click = false;

			closed = true;
			_close_ret = 0;
		}

		if (cvui::button(_frame, lf3, y + 120, cancel_idle, cancel2_over, cancel_idle))
		{
			_inpaint_click = false;

			_close_ret = 0;
			_working = act_display;

		}

		return;
	}


	{
		int lf1 = x + (w - discard_idle.cols - cancel_idle.cols - 30) / 2;
		int lf2 = lf1 + discard_idle.cols + 30;

		if (cvui::button(_frame, lf1, y + 120, discard_idle, discard2_over, discard_idle))
		{
			_inpaint_click = false;
			if (type == 0)
			{
				if (_close_ret == 1)
				{
					closed = true;
				}
				else if (_close_ret == 2)
				{
					nav(0);
				}
				else if (_close_ret == 3)
				{
					nav(1);
				}
				else if (_close_ret == 5)
				{
					_tool_btn = toolbar_open;
					_working = act_display;

				}
				_close_ret = 0;
			}
			else
			{
				if (_sel_nav_btn == _nav_btn)
				{
					set_working2(_do_nav_btn, _do_nav_state);
				}
				else
				{
					set_working2(_sel_nav_btn, _sel_nav_state);
				}

				_do_nav_btn = 0;
				_do_nav_state = false;
				_working = act_display;
			}
		}

		if (cvui::button(_frame, lf2, y + 120, cancel2_idle, cancel2_over, cancel_idle))
		{
			_inpaint_click = false;
			if (type == 0)
			{
				_close_ret = 0;
				_working = act_display;
			}
			else
			{
				_do_nav_btn = 0;
				_do_nav_state = false;
				_working = act_display;
			}
		}
	}
}

bool CPaint::erase_lastopt(int opt_type, int nav_type)
{
	bool b = false;
	EnterCriticalSection(&_cs);
	{
		for (;;)
		{
			if (_opt_arr.empty())
			{
				break;
			}

			int opt_type1 = 0;
			int nav_type1 = 0;
			int len0 = (int)_opt_arr.size();
			for (int i = 0; i < len0; i++)
			{
				opt_type1 = _opt_arr[i].opt_type;
				nav_type1 = _opt_arr[i].nav_type;
			}

			if ((opt_type == 9) && (opt_type1 == btn_seg_fgd || opt_type1 == btn_seg_bgd || opt_type1 == btn_seg_rect))
			{
				_opt_arr.erase(end(_opt_arr) - 1);
				b = true;
			}
			else if (opt_type1 == opt_type || nav_type1 == nav_type)
			{
				_opt_arr.erase(end(_opt_arr) - 1);
				b = true;
			}
			else
			{
				break;
			}
		}

		if (_opt_arr.empty())
		{
			_idx = 0;
		}
		else if (_idx > _opt_arr.size())
		{
			_idx = (int)_opt_arr.size();
		}

	}
	LeaveCriticalSection(&_cs);

	return b;
}
//
//void CPaint::resize_mask(Mat image, Mat& mask, Size size)
//{
//	Mat tmp;
//	cvtColor(image, tmp, CV_BGR2BGRA);
//	vector<int> fromto = { 0, 3 };
//	mixChannels(mask, tmp, fromto);
//
//	resize(tmp, tmp, size, 0, 0, resizemode());
//	mask = Mat::zeros(size, CV_8UC1);
//
//	vector<int> fromto2 = { 3, 0 };
//	mixChannels(tmp, mask, fromto2);
//}

void CPaint::trans_apply(bool btrans)
{
	if (_rect_select.width > 0)
	{
		btrans = false;
		if (!_eraser_mask.empty())
		{
			btrans = countNonZero(_eraser_mask) > 0 ? true : false;
		}

		if (_rect_select.width > 0)
		{
			Rect rc = _rect_select;
			dst_rc_2_lena_rc(_rect_select, rc);
			if (rc.x < 0 || rc.y < 0 || rc.width <= 0 || rc.height <= 0 ||
				rc.x + rc.width > _weightedImage.cols || rc.y + rc.height > _weightedImage.rows)
			{
				return;
			}

			if (btrans)
			{
				_eraser_mask(rc) = 0;
				_weightedImage(rc) = Scalar::all(255);
			}
			else
			{
				_weightedImage(rc) = Scalar::all(255);
			}
			_rect_select = Rect(0, 0, 0, 0);
		}
		_lena_bak = _weightedImage.clone();

		opt_t op;
		op.opt_type = btn_draw_trans;
		op.nav_type = 0;
		op.mask = _eraser_mask.clone();
		op.image = _weightedImage.clone();
		op.pts = _pts;
		op.thickness = THICKNESS_VAL;
		add_opt(op);
		_working = act_display;
	}
}

void CPaint::alpha(Vec3b& v, int BChannel, int GChannel, int RChannel, double alphaReserve)
{
	v[0] = saturate_cast<uchar>((int)(v[0] * alphaReserve + BChannel * (1 - alphaReserve)));
	v[1] = saturate_cast<uchar>((int)(v[1] * alphaReserve + GChannel * (1 - alphaReserve)));
	v[2] = saturate_cast<uchar>((int)(v[2] * alphaReserve + RChannel * (1 - alphaReserve)));
}

void CPaint::sc(wstring path, wstring appDataDir, wstring subfolder)
{
	wstring szPath = path + subfolder + L"\\1.jpg";
	Replace(szPath, L"\\", L"/");

	list<wstring> lstFile;
	GetFolderPics(szPath, lstFile);

	list<wstring>::iterator it;
	for (it = lstFile.begin(); it != lstFile.end(); it++)
	{
		wstring src = *it;

		wstring fname = L"";
		wstring::size_type pos = src.rfind(L"/");
		if (pos != wstring::npos)
		{
			fname = src.substr(pos);

			wstring dst = appDataDir + subfolder + L"\\" + fname;
			CopyFile(src.c_str(), dst.c_str(), TRUE);
		}
	}
}

wstring CPaint::get_extension_dir()
{
	std::wstring dst = L"";

	wchar_t* szDocument;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &szDocument)))
	{
		dst = szDocument;
		dst += L"\\PhotoNest";
		CoTaskMemFree(szDocument);
	}

	DWORD dwAttr = 0;
	wstring dst2 = dst + L"\\extension\\sky\\QR0001.jpg";
	dwAttr = GetFileAttributesW(dst2.c_str());
	if (dwAttr == -1)
	{
		wstring s5 = dst;
		CreateDirectory(s5.c_str(), NULL);

		s5 = dst + L"\\extension";
		CreateDirectory(s5.c_str(), NULL);

		s5 = dst + L"\\extension\\sky";
		CreateDirectory(s5.c_str(), NULL);

		s5 = dst + L"\\extension\\sticker";
		CreateDirectory(s5.c_str(), NULL);

		wstring szPath = get_module_path(NULL);

		sc(szPath, dst, L"\\extension\\sky");
		sc(szPath, dst, L"\\extension\\sticker");
	}
	return dst;
}
































