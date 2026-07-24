///////////////////////////////////////////////////////////////////////
/// @file paint_init.cpp
/// @brief 绘图模块 - 初始化功能实现
/// @details 实现绘图应用的初始化逻辑,包括应用类型设置、
///          许可证验证、语言加载、窗口配置等
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理 PhotoNest 应用启动时的初始化流程
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
#include "unitil3.h"

extern HINSTANCE _his;

void CPaint::init2(int type, int pixelsy, const wchar_t* en, const wchar_t* user, int ov, bool bfirst)
{
	_app_type = type;
	_usr_lic = user;
	wstring langue = en;
	_pixelsy = pixelsy;
	_ov = ov;
	_nav_btn = nav_draw;

	if (_usr_lic == L"0")
	{
		PRODUCT_NAME = L"PhotoNest";
		WINDOW_NAME = "PhotoNest";
	}
	else if (_usr_lic == L"1")
	{
		PRODUCT_NAME = L"PhotoNest Encryption";
		WINDOW_NAME = "PhotoNest Encryption";
	}
	else
	{
		PRODUCT_NAME = L"PhotoNest Cutout";
		WINDOW_NAME = "PhotoNest Cutout";
	}
	_nav_btn = nav_inp;

	//if (_his == NULL)
	//{
	//	_his = ::GetModuleHandle(L"photol.dll");
	//}

	load_ini();

	_left_width = 0;

	if (_app_type == FROM_EXE)
	{
		//if (langue == L"")
		//{
		//	LCID lcd = GetUserDefaultUILanguage();
		//	switch (lcd)
		//	{
		//	case 0X0804:
		//		langue = L"zh";
		//		break;
		//	case 0x0409:
		//		langue = L"en";
		//		break;
		//	default:
		//		break;
		//	}
		//}

		if (langue == L"")
		{
			langue = _language;
		}
	}

	memset(_fn, 0, 255 * sizeof(wchar_t));
	wcscpy_s(_fn, L"Arial");

	_lang.proc_lang(langue);

	int cx = GetSystemMetrics(SM_CXICON);
	if (bfirst)
	{
		if (cx >= 56)
		{
			FOOTER_HEIGHT = 30 + 10;
			_win_h -= FOOTER_HEIGHT;
		}
		else
		{
			FOOTER_HEIGHT = 30;
			_win_h -= FOOTER_HEIGHT;
		}
		_win_w -= TOOLBAR_RIGHT_WIDTH;
	}

	load_ico(cx);
}

void CPaint::load_ico(int cx)
{
	if (cx >= 56)
	{
		FOOTER_HEIGHT = 30 + 10;

		TOOLBAR_TOP_HEIGHT = 40 + 10;
		TOOLBAR_BTN_STEP_X = 50;
	}
	else
	{
		FOOTER_HEIGHT = 30;

		TOOLBAR_TOP_HEIGHT = 40;
		TOOLBAR_BTN_STEP_X = 40;
	}

	int w = 14;
	int h = 14;
	if (cx <= 32)
	{
		w = 14;
		h = 14;
	}
	else if (cx <= 40)
	{
		w = 16;
		h = 16;
	}
	else if (cx <= 48)
	{
		w = 18;
		h = 18;
	}
	else
	{
		w = 24;
		h = 24;
	}
	set_svg(w, h, IDR_SVG_FUNC, func_ico_idle);

	set_svg(w, h, IDR_SVG_OPEN, open_ico_idle);
	make_gray(open_ico_idle, open_ico_over, open_ico_gray);

	set_svg(w, h, IDR_SVG_SAVE, save_ico_idle);
	make_gray(save_ico_idle, save_ico_over, save_ico_gray);

	//set_svg(w, h, IDR_SVG_SAVEAS, saveas_ico_idle);
	//make_gray(_lang.trans("Save As...").c_str(), saveas_ico_idle, saveas_ico_over, saveas_ico_gray);
	draw_button(_lang.trans("Save As...").c_str(), saveas_ico_idle, saveas_ico_over, saveas_ico_gray);


	set_svg(w, h, IDR_SVG_PASTEAS, pasteas_ico_idle);
	draw_ico_button(_lang.trans("Paste As New").c_str(), pasteas_ico_idle, pasteas_ico_over);

	set_svg(cx == 32 ? 20 : 24, cx == 32 ? 20 : 24, IDR_SVG_HAND, hand_idle);
	make_gray(hand_idle, hand_over, hand_gray);

	set_svg(w, h, IDR_SVG_COPY, copy_idle);
	make_gray(copy_idle, copy_over, copy_gray);

	set_svg(w, h, IDR_SVG_CUT, cut_idle);
	make_gray(_lang.trans("Cut").c_str(), cut_idle, cut_over, cut_gray);

	set_svg(w, h, IDR_SVG_PASTE, paste_idle);
	make_gray(paste_idle, paste_over, paste_gray);

	draw_button(_lang.trans("Save").c_str(), save_idle, save_over, save_gray, Scalar(0, 255, 0));
	draw_button(_lang.trans("Save As...").c_str(), saveas_idle, saveas_over, saveas_gray, Scalar(0, 255, 0));

	load_bitmap(_his, down_idle, cx == 32 ? IDB_DOWN_14 : IDB_DOWN_18);
	make_gray(down_idle, down_over, down_gray);

	Mat temp;
	draw_button(_lang.trans("Discard Changes").c_str(), discard_idle, discard_over, Scalar::all(66));
	draw_button(_lang.trans("Save").c_str(), yes_idle, yes_over, Scalar::all(66));
	draw_button(_lang.trans("Don't Save").c_str(), no_idle, no_over, Scalar::all(66));
	draw_button(_lang.trans("Cancel").c_str(), cancel_idle, cancel_over, Scalar::all(66));

	draw_button(_lang.trans("Save").c_str(), yes2_idle, yes2_over, Scalar::all(66), Scalar(16, 217, 46));
	draw_button(_lang.trans("Don't Save").c_str(), no2_idle, no2_over, Scalar::all(66), Scalar(16, 217, 46));

	draw_button(_lang.trans("Discard Changes").c_str(), discard2_idle, discard2_over, Scalar::all(66), Scalar(16, 217, 46));
	draw_button(_lang.trans("Cancel").c_str(), cancel2_idle, cancel2_over, Scalar::all(66), Scalar(16, 217, 46));

	set_svg(w, h, IDR_SVG_LOAD, load_idle);
	make_gray(load_idle, load_over, load_gray);

	set_svg(w, h, IDR_SVG_REDO, redo_idle);
	make_gray(redo_idle, redo_over, redo_gray);

	set_svg(w, h, IDR_SVG_UNDO, undo_idle);
	make_gray(undo_idle, undo_over, undo_gray);

	set_svg(w, h, IDR_SVG_ZOOMIN, zoomin_idle);
	make_gray(zoomin_idle, zoomin_over, zoomin_gray);

	set_svg(w, h, IDR_SVG_ZOOMOUT, zoomout_idle);
	make_gray(zoomout_idle, zoomout_over, zoomout_gray);

	set_svg(w, h, IDR_SVG_PREV, prev_idle, 32, 24);
	make_gray(prev_idle, prev_over, prev_gray);

	set_svg(w, h, IDR_SVG_NEXT, next_idle, 32, 24);
	make_gray(next_idle, next_over, next_gray);

	set_svg(w, h, IDR_SVG_LOC, loc_idle, 32, 24);
	make_over(loc_idle, loc_over);

	Mat t5 = get_lbl(_u2w(_lang.trans("Activation Expired")).c_str(), Scalar::all(0), Scalar::all(190), 16);
	lbl_ov = Mat(Size(t5.cols + 180, t5.rows + 180), CV_8UC3, Scalar::all(0));
	t5.copyTo(lbl_ov(Rect(90, 90, t5.cols, t5.rows)));

	float row9 = (float)lbl_ov.rows / 2;
	float col9 = (float)lbl_ov.cols / 2;
	double angle = 30;
	cv::Point2f center(col9, row9);
	double scale = 1;

	cv::Mat M = cv::getRotationMatrix2D(center, angle, scale);//计算旋转的仿射变换矩阵 
	cv::warpAffine(lbl_ov, lbl_ov, M, cv::Size(lbl_ov.cols, lbl_ov.rows));//仿射变换  


	lbl_tolerance = get_lbl(_u2w(_lang.trans("Tolerance")).c_str(), Scalar::all(43));
	lbl_thickness = get_lbl(_u2w(_lang.trans("Thickness")).c_str(), Scalar::all(43));

	lbl_small = get_lbl(_u2w(_lang.trans("Too small")).c_str(), Scalar(49, 52, 49));

	//set_svg(w, h, IDR_SVG_HOME, home_idle);
	load_bitmap(_his, home_idle, IDB_ITEM);
	draw_ico_button(_lang.trans("Home").c_str(), home_idle);
	_combo_width_help = max(_combo_width_help, home_idle.cols);

	load_bitmap(_his, reg_idle, IDB_ITEM);
	settings_idle = reg_idle.clone();
	draw_ico_button(_lang.trans("Registration...").c_str(), reg_idle);
	_combo_width_help = max(_combo_width_help, reg_idle.cols);

	//set_svg(w, h, IDR_SVG_ABOUT, about_idle);
	load_bitmap(_his, about_idle, IDB_ITEM);
	draw_ico_button(_lang.trans("About...").c_str(), about_idle);
	_combo_width_help = max(_combo_width_help, about_idle.cols);

	draw_ico_button(_lang.trans("Language").c_str(), settings_idle);
	_combo_width_help = max(_combo_width_help, settings_idle.cols);

	_combo_width_help = _combo_width_help + 5;

	temp = Mat::zeros(Size(_combo_width_help, about_idle.rows), CV_8UC3);
	change_width(temp, home_idle);
	change_width(temp, reg_idle);
	change_width(temp, about_idle);
	change_width(temp, settings_idle);

	make_over(home_idle, home_over);
	make_over(reg_idle, reg_over);
	make_over(about_idle, about_over);
	make_over(settings_idle, settings_over);

	set_svg(w, h, IDR_SVG_HELP, help_idle);
	make_gray(help_idle, help_over, help_gray);

	set_svg(w, h, IDR_SVG_CLOSE, stop_idle, 20, 20, Scalar::all(218));
	set_svg(w, h, IDR_SVG_CLOSE_OVER, stop_over, 20, 20, Scalar::all(218));


	set_svg(w, h, IDR_SVG_OPEN, firstopen_idle, 32, 32, Scalar(49, 52, 49));
	//draw_ico_button(_lang.trans("Drag files here").c_str(), firstopen_idle, firstopen_over, Scalar(49, 52, 49));
	draw_button(_lang.trans("Drag files here").c_str(), lbl_drag_idle, lbl_drag_over, Scalar(49, 52, 49));
	draw_button(_lang.trans("Original image").c_str(), original_idle, original_over, original_gray, Scalar::all(164));

	//lbl_loading = get_lbl(_u2w(_lang.trans("Loading...")).c_str(), Scalar(49, 52, 49), Scalar::all(76));
	load_bitmap(_his, lbl_loading, IDB_LOADER);
	lbl_openerr = get_lbl(_u2w(_lang.trans("An error occurred")).c_str(), Scalar(49, 52, 49));

	if (_app_type == FROM_EXE)
	{
		/*
		CMySoft soft;
		string code = "";
		lic_header lic;
		soft.get_verify(code, lic, _ov);

		if (_ov == 1)
		{
			Scalar c = Scalar(0, 255, 0);
			string exp = _lang.trans("Activation Expired");

			draw_button(exp.c_str(), expiration_idle, expiration_over, Scalar(49, 52, 49), c);
		}
		else if (lic.year == 0)
		{
			time_t current_time;
			time(&current_time);
			int day = static_cast<int>(current_time - lic.date) / (24 * 60 * 60);
			if (day > 23)
			{
				time_t t0 = lic.date;
				t0 += 30 * EXP_SPAN;

				struct tm t;
				localtime_s(&t, &t0);

				char stamped[MAX_PATH] = { 0 };
				strftime(stamped, MAX_PATH, "%Y/%m/%d", &t);

				string exp = _lang.trans("Expiration Date:");
				exp += " ";
				exp += stamped;

				Scalar c = Scalar(0, 255, 0);
				draw_button(exp.c_str(), expiration_idle, expiration_over, Scalar(49, 52, 49), c);
			}
		}
		*/
	}

	set_svg(w, h, IDR_SVG_CENTER, center_idle);
	make_gray(center_idle, center_over, center_gray);

	set_svg(w, h, IDR_SVG_CLOSE, close_idle);
	make_gray(close_idle, close_over, close_gray);

	set_svg(w, h, IDR_SVG_OPAQUE, input_opaque_model);
	Scalar c0 = Scalar(GetBValue(_input_bg_rgb), GetGValue(_input_bg_rgb), GetRValue(_input_bg_rgb));

	//input_opaque_idle = input_opaque_model.clone();
	//make_draw_ico(input_opaque_idle, 0xbe, c0);
	//make_gray(input_opaque_idle, input_opaque_over, input_opaque_gray);

	set_svg(w, h, IDR_SVG_TRANS, input_trans_idle);
	make_gray(input_trans_idle, input_trans_over, input_trans_gray);

	set_svg(w, h, IDR_SVG_TIPS, tips_idle);
	make_over(tips_idle, tips_over);

	set_svg(w, h, IDR_SVG_APPLY, apply_idle);
	make_gray(apply_idle, apply_over, apply_gray);



	set_svg(w, h, IDR_SVG_DONE, done_idle);
	done_inp_idle = done_idle.clone();

	draw_ico_button(_lang.trans("Apply").c_str(), done_idle);
	make_gray(done_idle, done_over, done_gray);

	draw_ico_button(_lang.trans("Apply").c_str(), done_inp_idle);//Erase
	make_gray(done_inp_idle, done_inp_over, done_inp_gray);


	set_svg(w, h, IDR_SVG_WARNING, warning_idle, 24, 24, Scalar::all(49));

	init_inp(cx, w, h);
	init_seg(cx, w, h);
	init_sticker(cx, w, h);
	init_filter(cx, w, h);
	init_more(cx, w, h);
	init_draw(cx, w, h);

	//load_bitmap(nav_inp_idle, IDB_ITEM);
	nav_inp_idle = Mat(32, 32, CV_8UC3);
	nav_inp_idle = Scalar::all(43);

	nav_segment_idle = nav_inp_idle.clone();
	nav_sticker_idle = nav_inp_idle.clone();
	nav_filter_idle = nav_inp_idle.clone();
	nav_draw_idle = nav_inp_idle.clone();

	draw_button(_lang.trans("Remove unnecessary Objects").c_str(), nav_inp_idle, nav_inp_over, nav_inp_gray);
	_combo_width_nav = nav_inp_idle.cols;

	draw_button(_lang.trans("Background Matting").c_str(), nav_segment_idle, nav_segment_over, nav_segment_gray);
	_combo_width_nav = max(_combo_width_nav, nav_segment_idle.cols);

	draw_button(_lang.trans("Image Stitching").c_str(), nav_sticker_idle, nav_sticker_over, nav_sticker_gray);
	_combo_width_nav = max(_combo_width_nav, nav_sticker_idle.cols);

	draw_button(_lang.trans("Adjust").c_str(), nav_filter_idle, nav_filter_over, nav_filter_gray);
	_combo_width_nav = max(_combo_width_nav, nav_filter_idle.cols);

	draw_button(_lang.trans("Drawing Tool").c_str(), nav_draw_idle, nav_draw_over, nav_draw_gray);
	_combo_width_nav = max(_combo_width_nav, nav_draw_idle.cols);

	_combo_width_nav += 16;
	int off = 12;

	temp = Mat::zeros(Size(_combo_width_nav, nav_inp_idle.rows), CV_8UC3);
	change_width(temp, nav_inp_idle, Scalar::all(43), off);
	change_width(temp, nav_segment_idle, Scalar::all(43), off);
	change_width(temp, nav_sticker_idle, Scalar::all(43), off);
	change_width(temp, nav_filter_idle, Scalar::all(43), off);
	change_width(temp, nav_draw_idle, Scalar::all(43), off);

	make_gray(nav_inp_idle, nav_inp_over, nav_inp_gray);
	make_gray(nav_segment_idle, nav_segment_over, nav_segment_gray);
	make_gray(nav_sticker_idle, nav_sticker_over, nav_sticker_gray);
	make_gray(nav_filter_idle, nav_filter_over, nav_filter_gray);
	make_gray(nav_draw_idle, nav_draw_over, nav_draw_gray);
	_lbl_inp = get_lbl(_u2w(_lang.trans("Remove unnecessary Objects")).c_str(), Scalar::all(43), Scalar::all(190));
	int width2 = _lbl_inp.cols;

	_lbl_segment = get_lbl(_u2w(_lang.trans("Background Matting")).c_str(), Scalar::all(43), Scalar::all(190));
	width2 = max(width2, _lbl_segment.cols);

	_lbl_sticker = get_lbl(_u2w(_lang.trans("Image Stitching")).c_str(), Scalar::all(43), Scalar::all(190));
	width2 = max(width2, _lbl_sticker.cols);

	_lbl_filter = get_lbl(_u2w(_lang.trans("Adjust")).c_str(), Scalar::all(43), Scalar::all(190));
	width2 = max(width2, _lbl_filter.cols);

	_lbl_draw = get_lbl(_u2w(_lang.trans("Drawing Tool")).c_str(), Scalar::all(43), Scalar::all(190));
	width2 = max(width2, _lbl_draw.cols);
	width2 += 16;

	off = 8;
	temp = Mat::zeros(Size(width2 + down_over.cols + 8, _lbl_inp.rows + 4), CV_8UC3);
	change_width(temp, _lbl_inp, Scalar::all(43), off);
	change_width(temp, _lbl_segment, Scalar::all(43), off);
	change_width(temp, _lbl_sticker, Scalar::all(43), off);
	change_width(temp, _lbl_filter, Scalar::all(43), off);
	change_width(temp, _lbl_draw, Scalar::all(43), off);

	make_over(_lbl_inp, _lbl_inp);
	make_over(_lbl_segment, _lbl_segment);
	make_over(_lbl_sticker, _lbl_sticker);
	make_over(_lbl_filter, _lbl_filter);
	make_over(_lbl_draw, _lbl_draw);


	down2_idle = down_idle(Rect(0, (down_idle.rows - _lbl_inp.rows) / 2, down_idle.cols, _lbl_inp.rows));
	down2_over = down_over(Rect(0, (down_over.rows - _lbl_inp.rows) / 2, down_idle.cols, _lbl_inp.rows));
	down2_gray = down_gray(Rect(0, (down_gray.rows - _lbl_inp.rows) / 2, down_idle.cols, _lbl_inp.rows));

	Rect rc(width2 + 8, (temp.rows - down2_idle.rows) / 2, down2_idle.cols, down2_idle.rows);
	down2_idle.copyTo(_lbl_inp(rc));
	down2_idle.copyTo(_lbl_segment(rc));
	down2_idle.copyTo(_lbl_sticker(rc));
	down2_idle.copyTo(_lbl_filter(rc));
	down2_idle.copyTo(_lbl_draw(rc));


	load_bitmap(_his, info_idle, IDB_INFO);

}

void CPaint::load_image(string type0, const wchar_t* fname, bool binit)
{
	_down_saveas_click = false;
	_down_cut_click = false;
	_down_sticker_click = false;
	_dst_tips = Mat::zeros(Size(0, 0), CV_8UC1);

	_toosmall = 0;

	reset(binit);
	wstring fname3 = (fname == NULL) ? L"" : fname;
	Replace(fname3, L"\\", L"/");

	if (fname3 != L"")
	{
		_fcoi = fname3;

		ifstream file(fname3, ios::in | ios::binary | ios::ate);
		if (!file.is_open())
		{
			_lena2 = Mat::zeros(Size(0, 0), CV_8UC3);
		}
		else
		{
			streampos size = file.tellg();
			file.seekg(0, ios::beg);
			string buffer(size, 0);
			file.read(&buffer[0], size);
			file.close();

			_mkb = (int)size;
			if (_mkb != 0)
			{
				wstring::size_type  pos = fname3.find(L".krf");
				if (pos == wstring::npos)
				{
					vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
					_lena2 = imdecode(vec_data, IMREAD_UNCHANGED);

					//test
					{
						//int i, j;
						//Mat dimg(64, 64, CV_64FC2, Scalar(0, 1.1));

						//for (i = 0; i < dimg.rows; i++)
						//{
						//	for (j = 0; j < dimg.cols; j++)
						//	{
						//		dimg.at<Vec2d>(i, j)[0] = 9.9;
						//		dimg.at<Vec2d>(i, j)[1] = 100.0;
						//	}
						//}

						//_lena2 = dimg;
					}

					if (_lena2.data != NULL)
					{
						int u = _lena2.depth();
						if (u != 0)
						{
							Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
							temp.convertTo(_lena2, CV_8U);
						}
					}
				}
				else
				{
					//_fcoi = L"";
					//if (_enckey == "")
					//{
					//	_enckey = "1";
					//}

					//WritePrivate("_enckey", _enckey.c_str());

					// Retrive file size
					DWORD lengthActual = (DWORD)size;

					DE_HEADER* de_hdr = (DE_HEADER*)buffer.c_str();
					int offset = 8 + de_hdr->offset;

					_BF.Decrypt((void*)(buffer.c_str() + 8), lengthActual - 8);

					//HANDLE hWrite = CreateFileW(L"e:\\ttt.webp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
					//	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					//if (hWrite != INVALID_HANDLE_VALUE)
					//{
					//	DWORD dwWritten = 0;
					//	WriteFile(hWrite, (void*)(buffer.c_str() + offset), lengthActual - offset, &dwWritten, NULL);
					//	CloseHandle(hWrite);
					//}

					vector<uchar> vec_data(&buffer[offset], &buffer[offset] + lengthActual - offset);
					_lena2 = imdecode(vec_data, IMREAD_UNCHANGED);//IMREAD_COLOR);// 
					int u = _lena2.depth();
					if (_lena2.data != NULL && u != 0)
					{
						Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
						temp.convertTo(_lena2, CV_8U);
					}

				}

				if (_lena2.data == NULL)
				{
					//WritePrivate("_lena2.data", "NULL");
					_lena2 = Mat::zeros(Size(0, 0), CV_8UC3);
				}
			}
		}

		_cur_name = fname3;
	}
	else
	{
		_lena2 = Mat::zeros(Size(0, 0), CV_8UC3);
	}

	if (type0 == "first")
	{
		if (_app_type == FROM_EXE)
		{
			if (!_lena2.empty())
			{
				_lstFile.clear();
				GetFolderPics(fname3, _lstFile);

				_len_files = (int)_lstFile.size();
				_idx_file = 0;

				list<wstring>::iterator it;
				for (it = _lstFile.begin(); it != _lstFile.end(); it++)
				{
					_idx_file++;
					if (*it == fname3)
					{
						break;
					}
				}
			}
		}
	}

	if (!_lena2.empty())
	{
		int c = _lena2.channels();
		if (c == 2)
		{
			_lena2 = Mat::zeros(Size(0, 0), CV_8UC3);
		}

		if (_lena2.cols <= 16 || _lena2.rows <= 16)
		{
			_toosmall = 1;
			_lena2 = Mat::zeros(Size(0, 0), CV_8UC3);
		}
	}

	if (!_lena2.empty())
	{
		_lenaSize = _lena2.size();
		//3840 2160
		//	7680 4320
		//	6000 3000
		//	6000 4000
		//	4096 2160
		//	6250 3515
		//	5000 2800
		//	------ - 1920 2048 2000
		_lenaf = 1.0f;

		int val = max(_lenaSize.width, _lenaSize.height);
		if (val > 5000)
		{
			float f = 1.0f;
			div_t div_result = div(val, 7);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 7.0f;
			}
			div_result = div(val, 6);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 6.0f;
			}
			div_result = div(val, 5);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 5.0f;
			}
			div_result = div(val, 4);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 4.0f;
			}
			div_result = div(val, 3);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 3.0f;
			}
			div_result = div(val, 2);
			if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
			{
				f = 2.0f;
			}

			if (f < 2.0f)
			{
				if (val > 10000)
				{
					f = 4.0f;
				}
				else
				{
					f = 2.0f;
				}
			}
			_lenaf = f;
			resize(_lena2, _lena2, Size((int)(_lenaSize.width / f), (int)(_lenaSize.height / f)), 0, 0, resizemode());
		}

		int _channels = _lena2.channels();
		//if (_channels == 2)
		//{
		//	Mat tmp;
		//	Mat dst8 = Mat::zeros(_lena2.size(), CV_8U);
		//	normalize(_lena2, tmp, 0, 255, NORM_MINMAX);
		//	convertScaleAbs(tmp, dst8);
		//	_lena2 = dst8;
		//	_channels = _lena2.channels();
		//}

		if (_channels == 1)
		{
			vector<Mat> channels;
			for (int i = 0; i < 3; i++)
			{
				channels.push_back(_lena2);
			}
			merge(channels, _lena2);
		}


		int angle = 0;
		if (_curr_nav_dto.rotation == L"0")
		{
		}
		else if (_curr_nav_dto.rotation == L"6")
		{
			angle = 90;
		}
		else if (_curr_nav_dto.rotation == L"8")
		{
			angle = -90;
		}
		else if (_curr_nav_dto.rotation == L"3")
		{
			angle = 180;
		}

		if (angle != 0)
		{
			RotateImage(_lena2, angle, true);
		}

		_channels = _lena2.channels();

		_size_info = Mat::zeros(Size(0, 0), CV_8UC3);
		_kb_info = Mat::zeros(Size(0, 0), CV_8UC3);

		if (_channels == 4)
		{
			_bgra_mask = Mat::zeros(_lena2.size(), CV_8UC1);

			for (int i = 0; i < _lena2.rows; i++)
			{
				Vec4b* ptr = _lena2.ptr<Vec4b>(i);
				for (int j = 0; j < _lena2.cols; j++)
				{
					Vec4b& v = ptr[j];
					if (v[3] != 255)
					{
						float alphaReserve = (float)v[3] / 255.0;
						v[0] = saturate_cast<uchar>((int)(v[0] * alphaReserve + 255 * (1 - alphaReserve)));
						v[1] = saturate_cast<uchar>((int)(v[1] * alphaReserve + 255 * (1 - alphaReserve)));
						v[2] = saturate_cast<uchar>((int)(v[2] * alphaReserve + 255 * (1 - alphaReserve)));
					}
				}
			}

			vector<int> fromto = { 3, 0 };
			mixChannels(_lena2, _bgra_mask, fromto);

			cvtColor(_lena2, _lena2, CV_BGRA2BGR);
			_eraser_mask = _bgra_mask.clone();
		}
		else
		{
			_bgra_mask = Mat::zeros(Size(0, 0), CV_8UC1);
			_eraser_mask = Mat::zeros(Size(0, 0), CV_8UC1);
		}

		if (_ov == 1)
		{
			watermark(_lena2);
		}
	}

	_weightedImage = _lena2.clone();
	_lena_bak = _weightedImage.clone();
	_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
	_clip_mat = Mat::zeros(Size(0, 0), CV_8UC3);
	_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
	_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);
	_rect_seg_select = Rect(0, 0, 0, 0);

	_lena_w = _lena2.cols;
	_lena_h = _lena2.rows;

	_weighted_befoe_input = Mat::zeros(Size(0, 0), CV_8UC3);
	_result = Mat::zeros(Size(0, 0), CV_8UC1);
	_eraser_mask_tmp = Mat::zeros(Size(0, 0), CV_8UC3);
	_curr_rt = Rect(0, 0, 0, 0);

	if (type0 != "1")
	{
		oneone();
	}
}

bool CPaint::reset(bool binit)
{
	_changed = false;
	_inpainting = false;
	_inpaint_click = false;
	_has_tips = false;
	_save_click = false;

	_total_deg = 0;

	if (binit)
	{
		_opt_arr.clear();
		_idx = 0;
	}
	_mask_idx = -997;

	_working = act_nothing;
	_crop_opt = 0;
	_opt_rotate = 0;
	_crop_mouseup = false;

	init_fVal(_fVal);
	_last_t = 0;

	_save_ret = 1;
	_disp_nb = 0;
	_process_nb = 0;

	_close_ret = 0;
	_do_nav_btn = 0;
	_do_nav_state = false;
	_help_btn = 0;

	_hInputWnd = NULL;
	_for_font = false;
	_for_select = false;
	_drag_rc_type = 0;

	_help_click = 0;
	_show_help = 0;

	_pt_prev = Point(0, 0);

	_rect_select = Rect(0, 0, 0, 0);
	_rect_crop = Rect(0, 0, 0, 0);
	memset(_input_buf, 0, 2048 * sizeof(wchar_t));

	_rect_paste = Rect(0, 0, 0, 0);
	_show_err_type = 0;
	_space_key_mouse = false;
	_last_draw_opt = 0;
	_last_opt = 0;
	_opt = 0;
	_pointer_btn = btn_move;
	_tip_btn = 0;

	_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
	_seg_bg = Mat::zeros(Size(0, 0), CV_8UC1);
	_result = Mat::zeros(Size(0, 0), CV_8UC1);

	_sticker_arr.clear();
	_curr_sticker_id = 0;
	_in_rect_seg_select = false;

	_filter_changed = false;
	_filter_mousedown = false;
	_filter_mouseup = false;
	_is_sticker_move = false;

	_pt1_sticker = Point(0, 0);
	_pt2_sticker = Point(0, 0);

	_down_nav_click = false;
	_navbar_state = 0;
	return true;
}

void CPaint::load_ini()
{
	string alb = "private";
	if (PRODUCT_NAME == L"PhotoNest")
	{
		alb = "public";
	}
	_file_temp = get_appdata_path(alb);
	_file_temp += L"\\temp.png";

	string val = "";
	ReadPrivate("draw_rgb", val);
	if (val != "")
	{
		_draw_rgb = atol(val.c_str());
		int r = GetRValue(_draw_rgb);
		int g = GetGValue(_draw_rgb);
		int b = GetBValue(_draw_rgb);
		_picker_val = Scalar(b, g, r);
	}

	ReadPrivate("lfHeight", val);
	if (val != "")
	{
		_input_lf.lfHeight = atol(val.c_str());
	}
	else
	{
		_input_lf.lfHeight = -16;
	}

	ReadPrivate("lfFaceName", val);
	if (val == "")
	{
		val = "Arial";
	}
	wcscpy_s(_input_lf.lfFaceName, _u2w(val).c_str());

	ReadPrivate("input_bg_trans", val);
	if (val != "")
	{
		_input_bg_trans = atol(val.c_str());
	}

	ReadPrivate("input_bg_rgb", val);
	if (val != "")
	{
		_input_bg_rgb = atol(val.c_str());
	}

	ReadPrivate("seamless_mode", val);
	if (val != "")
	{
		_seamless_mode = atol(val.c_str());
	}

	ReadPrivate("inpaint_thickness", val);
	if (val != "")
	{
		_inpaint_thickness = atol(val.c_str());
	}

	ReadPrivate("draw_thickness", val);
	if (val != "")
	{
		_draw_thickness = atol(val.c_str());
	}

	ReadPrivate("erase_thickness", val);
	if (val != "")
	{
		_erase_thickness = atol(val.c_str());
	}

	ReadPrivate("tolerance", val);
	if (val != "")
	{
		_upDiff = atol(val.c_str());
	}

	ReadPrivate("language", val);
	if (val != "")
	{
		_language = _u2w(val);
	}
	//else
	//{
	//	HKEY hkey;
	//	long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PhotoNest Cutout_is1", 0,
	//		KEY_QUERY_VALUE, &hkey);
	//	if (ret == ERROR_SUCCESS)
	//	{
	//		wchar_t buf[100] = { 0 };// | KEY_WOW64_64KEY

	//		DWORD dwType = REG_SZ;
	//		DWORD dwSize = sizeof(buf);

	//		RegQueryValueEx(hkey, L"Inno Setup: Language", NULL, &dwType, (BYTE*)buf, &dwSize);
	//		_language = buf;

	//		RegCloseKey(hkey);
	//	}
	//}

	ReadPrivate("nav_btn", val);
	if (val != "")
	{
		_nav_btn = atol(val.c_str());
	}

	get_toolbar_lf();
}

void CPaint::set_winsize(HWND hWnd, int w, int h)
{
	if (::IsWindow(hWnd))
	{
		int left = 0;
		int top = 0;
		get_frame(hWnd, left, top);
	}
	else
	{
		_win_w = w;
		_win_h = h;

		if (_firstrun)
		{
			CvRect rect;
			int zm = 0;
			icvLoadWindowPos(WINDOW_NAME.c_str(), rect, zm);

			RECT rw, rmw;

			HWND pParent = ::GetParent(_win_handle);
			GetClientRect(pParent, &rw);
			GetWindowRect(pParent, &rmw);

			_win_w = rmw.right - rmw.left;
			_win_h = rmw.bottom - rmw.top;

			_win_w -= (rmw.right - rmw.left) - (rw.right - rw.left);
			_win_h -= (rmw.bottom - rmw.top) - (rw.bottom - rw.top);
		}
	}

	_frame = Mat(_win_h, _win_w, CV_8UC3);
	_win_h -= FOOTER_HEIGHT;
	_win_w -= TOOLBAR_RIGHT_WIDTH;
}

void CPaint::on_resize_cef()
{
	if (!IsWindow(_hParent))
	{
		return;
	}

	int left = 0;
	int top = 0;
	get_frame(_hParent, left, top);

	_frame = Mat(_win_h, _win_w, CV_8UC3);
	_win_h -= FOOTER_HEIGHT;
	//_win_w -= TOOLBAR_RIGHT_WIDTH;

	HWND pParent = ::GetParent(_win_handle);
	MoveWindow(pParent, left, top, _win_w/* + TOOLBAR_LEFT_WIDTH*/, _win_h + FOOTER_HEIGHT, FALSE);

	high_displayImage();
	draw_ui();

	move_input_wnd();
}

void CPaint::get_frame(HWND hWnd, int& left, int& top)
{
	if (::IsWindow(hWnd))
	{
		RECT rc;
		GetWindowRect(hWnd, &rc);
		_win_w = (int)((rc.right - rc.left) * 0.9f);
		_win_h = (int)((rc.bottom - rc.top) * 0.9f);

		if (_win_w < 1152)
		{
			_win_w = 1152;
		}
		if (_win_h < 600)
		{
			_win_h = 600;
		}
		if (rc.right - rc.left >= 1920)
		{
			_win_w = (int)(1920 * 0.9f);
		}
		if (rc.bottom - rc.top >= 1280)
		{
			_win_h = (int)(1280 * 0.9f);
		}

		left = (rc.right - rc.left - _win_w/* - TOOLBAR_RIGHT_WIDTH*/) / 2;
		top = (rc.bottom - rc.top - _win_h - FOOTER_HEIGHT) / 2;

		if (left < 0)
		{
			left = 0;
		}

		if (top < 30)
		{
			top = 30;
		}
	}
}

void CPaint::reset_frame()
{
	_frame = Scalar(49, 52, 49);
	_frame(Rect(0, 0, _win_w, TOOLBAR_TOP_HEIGHT)) = Scalar::all(43);
	_frame(Rect(0, _win_h, _win_w, FOOTER_HEIGHT)) = Scalar::all(43);

	//Rect rc;
	//get_toolbar_rc(rc);
	get_toolbar_lf();
	line(_frame, Point(0, TOOLBAR_TOP_HEIGHT - 1), Point(_win_w + TOOLBAR_RIGHT_WIDTH, TOOLBAR_TOP_HEIGHT - 1), Scalar::all(54), 1, 8, 0);
	line(_frame, Point(0, _win_h), Point(_win_w, _win_h), Scalar::all(54), 1, 8, 0);

	if (!_lena2.empty())
	{
		if (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE)
		{
			wchar_t* p = wcsrchr((wchar_t*)_curr_nav_dto.ori_name.c_str(), L'/');
			if (p != NULL)
			{
				wstring tmp = p + 1;
				//if (!_curr_nav_dto.file_time.empty())
				//{
				//	tmp += L" (" + _curr_nav_dto.file_time + L")";
				//}

				Mat lbl = get_lbl(tmp.c_str(), Scalar::all(43), Scalar::all(128));
				if (!lbl.empty() && lbl.rows <= 32)
				{
					Rect rc;
					rc.x = 5;
					rc.y = _win_h + (FOOTER_HEIGHT - lbl.rows) / 2 + 2;
					rc.width = lbl.cols < _win_w ? lbl.cols : _win_w / 2;
					rc.height = lbl.rows;

					lbl(Rect(0, 0, rc.width, rc.height)).copyTo(_frame(rc));
				}
			}
		}
	}
}

void CPaint::init_fVal(z_filter& fv)
{
	fv.contrastValue = 0;
	fv.brightValue = 0;
	fv.bkLightValue = 1;

	fv.use_contrastValue = false;
	fv.use_brightValue = false;
	fv.use_sketch = false;
	fv.use_decolor = false;
	fv.use_comic_strip = false;
	fv.use_vintage = false;
	fv.use_feather = false;

	fv.curr_use_contrastValue = false;
	fv.curr_use_brightValue = false;
	fv.curr_use_decolor = false;
	fv.curr_use_sketch = false;
	fv.curr_use_comic_strip = false;
	fv.curr_use_vintage = false;
	fv.curr_use_feather = false;
}