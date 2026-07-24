///////////////////////////////////////////////////////////////////////
/// @file paint_sticker.cpp
/// @brief PhotoNest 绘图模块 - 贴纸功能实现文件
/// @details 实现贴纸/叠加功能:
///           - 打开贴纸文件 (open_sticker)
///           - 贴纸对话框管理
///           - 图片文件选择
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的贴纸功能文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "myinpaint.h"
#include "cvui.h"
#include "resource.h"
#include<fstream>
#include "unitil2.h"
#include <windows.h>
#include<commdlg.h>

void CPaint::open_sticker()
{
	wchar_t szFileName[MAX_PATH] = { 0 };

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
#ifdef OPENFILENAME_SIZE_VERSION_400
	// we are not going to use new fields any way
	ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
	ofn.lStructSize = sizeof(ofn);
#endif
	ofn.hwndOwner = _win_handle;
	ofn.lpstrFilter =
		L"All Picture files\0*.jpeg;*.jpg;*.jpe;*.png;*.bmp;*.webp\0"
		L"JPEG files (*.jpeg;*.jpg;*.jpe)\0*.jpeg;*.jpg;*.jpe\0"
		L"PNG files (*.png)\0*.png\0"
		L"Windows bitmap (*.bmp;*.dib)\0*.bmp;*.dib\0"
		L"WebP files (*.webp)\0*.webp\0"
		L"All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	//ofn.lpstrFile[0] = 0;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;// | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = L"*";

	//wstring iniDir = get_module_path(NULL) + L"\\extension\\sticker";

	wstring iniDir = get_module_path(NULL);
	wstring::size_type pos = iniDir.find(L"\\windowsapps\\photonest");
	if (pos != wstring::npos)
	{
		iniDir = get_extension_dir();//get_appdata_path(alb);
	}

	iniDir += L"\\extension\\sticker";


	ofn.lpstrInitialDir = iniDir.c_str();


	if (GetOpenFileName(&ofn))
	{
		wstring fname2 = szFileName;

		ifstream file(fname2, ios::in | ios::binary | ios::ate);
		if (file.is_open())
		{
			streampos size = file.tellg();
			file.seekg(0, ios::beg);
			string buffer(size, 0);
			file.read(&buffer[0], size);
			file.close();

			if ((int)size != 0)
			{
				vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
				Mat paper = imdecode(vec_data, IMREAD_UNCHANGED);
				int u = paper.depth();
				if (paper.data != NULL && u != 0)
				{
					Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
					temp.convertTo(paper, CV_8U);
				}

				if (paper.data == NULL)
				{
					paper = Mat::zeros(Size(0, 0), CV_8UC3);
				}
				else
				{
					int iW = 800;
					if (paper.cols > iW || paper.rows > iW)
					{
						int w = min(paper.cols, iW);
						int h = min(paper.rows, iW);
						int x = (paper.cols - w) / 2;
						int y = (paper.rows - h) / 2;

						paper = paper(Rect(x, y, w, h));
					}

					int channels = paper.channels();
					if (channels == 4)
					{
						Mat tmp = Mat::zeros(paper.size(), CV_8U);
						vector<int> fromto = { 3, 0 };
						mixChannels(paper, tmp, fromto);

						int z = countNonZero(tmp);
						if (z == 0)
						{
							cvtColor(paper, paper, CV_BGRA2BGR);
						}
						else
						{
							Rect rc = get_rc255(tmp);
							paper = paper(rc);
						}
					}
					else if (channels == 1)
					{
						vector<Mat> v;
						v.push_back(paper);
						v.push_back(paper);
						v.push_back(paper);
						merge(v, paper);
					}
					else if (channels == 2)
					{
						paper = Mat::zeros(Size(0, 0), CV_8UC3);
					}

					if (paper.data != NULL)
					{
						int max_idx = 0;
						int len = (int)_sticker_arr.size();
						for (int i = 0; i < len; i++)
						{
							if (_sticker_arr[i].idx > max_idx)
							{
								max_idx = _sticker_arr[i].idx;
							}
						}

						z_sticker z;
						z.idx = max_idx + 1;
						z.id = max_idx + 1;

						Rect rc2 = Rect(70 + z.idx * 10, 20 + z.idx * 10, paper.cols * _dst_zoom_w / _lena_w, paper.rows * _dst_zoom_w / _lena_w);
						rc2.width = min(rc2.width, _dst.cols);
						rc2.height = min(rc2.height, _dst.rows);
						if (rc2.x + rc2.width > _dst.cols)
						{
							rc2.x = 0;
						}

						if (rc2.y + rc2.height > _dst.rows)
						{
							rc2.y = 0;
						}

						z.dst = paper.clone();
						_curr_sticker_id = z.id;

						//if (paper.cols < _lena_w && paper.rows < _lena_h)
						{
							z.rc = Rect(rc2.x, rc2.y, paper.cols, paper.rows);

							_sticker_arr.push_back(z);

							opt_t op;
							op.opt_type = btn_sticker;
							op.nav_type = nav_sticker;
							op.thickness = THICKNESS_VAL;
							op.sticker_arr = _sticker_arr;
							add_opt(op);
						}
						//else
						//{
						//	_disp_nb = 1;
						//	_show_err_type = 2;
						//}

						_opt = btn_sticker;
						_last_opt = btn_sticker;
						_working = act_display;

					}
				}
			}
		}
	}
}

bool CPaint::mouse_down_sticker()
{
	int x0 = cvui::mouse().x - _dst_in_frame.x;
	int y0 = cvui::mouse().y - _dst_in_frame.y;
	Point pt(x0, y0);

	int k = 0;
	int max_idx = 0;
	int len = (int)_sticker_arr.size();
	for (int i = 0; i < len; i++)
	{
		if (_sticker_arr[i].idx > max_idx)
		{
			max_idx = _sticker_arr[i].idx;
		}
	}

	bool b = false;
	for (int i = 0; i < len; i++)
	{
		Rect rc = _sticker_arr[i].rc;
		Rect rc2 = Rect(rc.x, rc.y, rc.width * _dst_zoom_w / _lena_w, rc.height * _dst_zoom_w / _lena_w);
		if (at_rect(pt, rc2))
		{
			k = i;
			b = true;
		}
	}

	if (b)
	{
		_sticker_arr[k].idx = max_idx + 1;
		sort(_sticker_arr.begin(), _sticker_arr.end(), less<z_sticker>());
	}

	if (!_sticker_arr.empty())
	{
		max_idx = 0;
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
				_curr_rt = _sticker_arr[i].rc;
				_curr_rt.x = _dst_in_lena.x + _curr_rt.x * _lena_w / _dst_zoom_w;
				_curr_rt.y = _dst_in_lena.y + _curr_rt.y * _lena_w / _dst_zoom_w;
				break;
			}
		}
	}

	return b;
}

bool CPaint::mouse_isdown_sticker(Rect& rc, Point& prev_pt, int id)
{
	bool b = false;
	if (rc.width != 0)
	{
		int x0 = cvui::mouse().x - _dst_in_frame.x;
		int y0 = cvui::mouse().y - _dst_in_frame.y;
		Point pt(x0, y0);

		Rect rc2 = Rect(rc.x, rc.y, rc.width * _dst_zoom_w / _lena_w, rc.height * _dst_zoom_w / _lena_w);

		if (_drag_rc_type == 9 || at_rect(pt, rc2))
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

			int len0 = (int)_opt_arr.size();
			for (int k = 0; k < len0; k++)
			{
				if (_opt_arr[k].opt_type == btn_sticker || _opt_arr[k].opt_type == btn_change_bg)
				{
					int len1 = (int)(_opt_arr[k].sticker_arr.size());
					for (int i = 0; i < len1; i++)
					{
						if (_opt_arr[k].sticker_arr[i].id == id)
						{
							_opt_arr[k].sticker_arr[i].rc = rc;
						}
					}
				}
			}
			return true;
		}
	}
	return false;
}

void CPaint::sticker_ok(Mat& dst2, Mat& imageROI)
{
	int cols = _weightedImage.cols;
	int rows = _weightedImage.rows;

	int nlen = (int)_sticker_arr.size();
	for (int i = 0; i < nlen; i++)
	{
		Mat paper = _sticker_arr[i].dst;
		Rect sr = _sticker_arr[i].rc;
		//int_xy(sr);

		sr.width = (int)(paper.cols * _zoom_r + .5);
		sr.height = (int)(paper.rows * _zoom_r + .5);


		Rect rc = Rect(0, 0, sr.width, sr.height);
		proc_paper_rc(rc, sr, _dst);

		if (rc.width <= 0 || rc.height <= 0 || rc.width > sr.width || rc.height > sr.height)
		{
		}
		else
		{
			Rect rc2 = sr;
			rc2.width = rc.width;
			rc2.height = rc.height;
			if (rc2.x < 0)
			{
				rc2.x = 0;
			}
			if (rc2.y < 0)
			{
				rc2.y = 0;
			}

			Mat tmp;
			resize(paper, tmp, Size(sr.width, sr.height), 0, 0, resizemode());
			tmp = tmp(rc);

			int mode = _seamless_mode;
			if (_is_sticker_move)
			{
				mode = 9;
			}
			int channels = tmp.channels();
			if (mode == 9)
			{
				if (channels == 4)
				{
					Mat d2 = dst2(rc2);
					set_dst_by_v3(d2, tmp);
				}
				else
				{
					tmp.copyTo(dst2(rc2));
				}
			}
			else if (mode == 10)
			{
			}
			else
			{
				Point center(rc2.x + rc2.width / 2, rc2.y + rc2.height / 2);

				Mat mask(tmp.size(), CV_8U, Scalar::all(255));

				bool b = false;
				if (channels == 4)
				{
					mask.setTo(0);
					b = set_mask_by_v3(mask, tmp);
					cvtColor(tmp, tmp, CV_BGRA2BGR);
				}

				if (!b)
				{
					mask.setTo(255);
				}

				if (mask.rows > 10 && mask.cols > 10)
				{
					seamlessClone(tmp, dst2, mask, center, dst2, mode);
				}
			}

			dst2.copyTo(imageROI);
		}

		_pt1_sticker = Point(_dst_in_frame.x + sr.x, _dst_in_frame.y + sr.y);
		_pt2_sticker = Point(_pt1_sticker.x + sr.width, _pt1_sticker.y + sr.height);
		drawDashRect(_frame, 5, 5, _pt1_sticker, _pt2_sticker);
	}
}

void CPaint::sticker_ok()
{
	_inpaint_click = true;

	bool ok = true;

	t_struct* myt = get_thread();
	myt->step = 1;
	myt->contours1 = 1;
	int btn = myt->opt;

	int cols = _weightedImage.cols;
	int rows = _weightedImage.rows;

	int nlen = (int)_sticker_arr.size();
	for (int i = 0; i < nlen; i++)
	{
		myt->step += 10;
		Mat paper = _sticker_arr[i].dst;
		Rect sr = _sticker_arr[i].rc;

		sr.x = (int)(sr.x / _zoom_r + .5);
		sr.y = (int)(sr.y / _zoom_r + .5);


		sr.width = paper.cols;
		sr.height = paper.rows;

		Rect rc = Rect(0, 0, sr.width, sr.height);
		proc_paper_rc(rc, sr, _weightedImage);
		if (rc.width <= 0 || rc.height <= 0 || rc.width > sr.width || rc.height > sr.height)
		{
			ok = false;
		}
		else
		{
			Rect rt = Rect(_dst_in_lena.x + sr.x, _dst_in_lena.y + sr.y, rc.width, rc.height);
			if (rt.x < 0)
			{
				rt.x = 0;
			}
			if (rt.y < 0)
			{
				rt.y = 0;
			}
			if (rt.x + rt.width > cols)
			{
				rt.width = cols - rt.x;
			}
			if (rt.y + rt.height > rows)
			{
				rt.height = rows - rt.y;
			}

			rc.width = rt.width;
			rc.height = rt.height;
			Mat tmp = paper(rc).clone();

			Mat d2 = _weightedImage(rt);
			Mat mask0 = Mat::zeros(Size(0, 0), CV_8U);

			int channels = tmp.channels();
			if (_seamless_mode == 9)
			{
				if (channels == 4)
				{
					set_dst_by_v3(d2, tmp);

					mask0 = Mat::zeros(tmp.size(), CV_8U);
					set_mask_by_v3(mask0, tmp);
				}
				else
				{
					tmp.copyTo(d2);
					if (!_eraser_mask.empty())
					{
						_eraser_mask(rt).setTo(255);
					}
				}
			}
			else if (_seamless_mode == 10)
			{
			}
			else
			{
				Mat mask = Mat(tmp.size(), CV_8U, Scalar::all(255));

				if (channels == 4)
				{
					mask.setTo(0);
					set_mask_by_v3(mask, tmp);
					cvtColor(tmp, tmp, CV_BGRA2BGR);

					mask0 = mask.clone();
				}
				else
				{
					mask.setTo(255);
					if (!_eraser_mask.empty())
					{
						_eraser_mask(rt).setTo(255);
					}
				}

				if (mask.rows > 10 && mask.cols > 10)
				{
					Point center2(rt.x + rt.width / 2, rt.y + rt.height / 2);
					center2.x = rt.width / 2;
					center2.y = rt.height / 2;
					Mat roi = _weightedImage(rt);
					seamlessClone(tmp, roi, mask, center2, roi, _seamless_mode);
				}
			}

			if (!_eraser_mask.empty() && !mask0.empty())
			{
				Mat tm = _eraser_mask(rt);

				for (int i = 0; i < mask0.rows; i++)
				{
					uchar* p1 = mask0.ptr<uchar>(i);
					for (int j = 0; j < mask0.cols; j++)
					{
						if (p1[j] == 255)
						{
							tm.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}

		myt->step += 10 * nlen;
		if (myt->step > 90)
		{
			myt->step = 90;
		}
	}

	if (!myt->stop)
	{
		myt->step = 100;

		_sticker_arr.clear();
		_pt_prev = Point(0, 0);
		_pt_down = Point(0, 0);

		int opt_type = 0;
		int thickness = 0;
		int len0 = (int)_opt_arr.size();
		for (int i = 0; i < len0; i++)
		{
			opt_type = _opt_arr[i].opt_type;
			thickness = _opt_arr[i].thickness;
		}
		if (opt_type == btn_sticker || opt_type == btn_change_bg)
		{
			_opt_arr.erase(end(_opt_arr) - 1);
		}

		if (ok)
		{
			opt_t op;
			op.opt_type = btn;
			op.nav_type = 0;
			op.thickness = 0;
			op.image = _weightedImage.clone();
			op.mask = _eraser_mask.clone();
			add_opt(op);

			_lena_bak = _weightedImage.clone();
		}

		_working = act_display;
		_inpaint_click = false;
	}
	_pt1_sticker = Point(0, 0);
	_pt2_sticker = Point(0, 0);
}

void CPaint::proc_paper_rc(Rect& rc, Rect sr, Mat dst)
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
			//rc.x = 0;
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
			//rc.y = 0;
		}
	}
	/*
		rc.x = rc.x * _lena_w / _dst_zoom_w;
		rc.y = rc.y * _lena_w / _dst_zoom_w;
		rc.width = rc.width * _lena_w / _dst_zoom_w;
		rc.height = rc.height * _lena_w / _dst_zoom_w;
*/
	if (rc.x + rc.width > sr.width)
	{
		rc.width = sr.width - rc.x;
	}
	if (rc.y + rc.height > sr.height)
	{
		rc.height = sr.height - rc.y;
	}

}

void CPaint::grabcut()
{
	_inpaint_click = true;

	t_struct* myt = get_thread();
	myt->step = 1;
	myt->contours1 = 1;

	Mat bgModel, fgModel;
	Mat image, mask;
	Rect rc_init;

	image = _dst.clone();
	mask = _seg_mask.clone();

	if (_lena_w >= _dst_zoom_w)
	{
	}
	else
	{
		if (!mask.empty())
		{
			//resize_mask(image, mask, _dst_in_lena.size());
			resize(mask, mask, _dst_in_lena.size(), 0, 0, resizemode());
		}
		image = _weightedImage(_dst_in_lena).clone();
	}

	if (!_eraser_mask.empty())
	{
		Mat tmp = _eraser_mask(_dst_in_lena).clone();
		if (_lena_w >= _dst_zoom_w)
		{
			//Mat image2 = _weightedImage(_dst_in_lena).clone();
			//resize_mask(image2, tmp, _dst.size());
			resize(tmp, tmp, _dst.size(), 0, 0, resizemode());
		}

		for (int i = 0; i < tmp.rows; i++)
		{
			for (int j = 0; j < tmp.cols; j++)
			{
				if (tmp.at<uchar>(i, j) == 0)
				{
					image.at< Vec3b>(i, j) = Vec3b(255, 255, 255);
				}
			}
		}
	}

	Vec3b c = image.at<Vec3b>(0, 0);
	vector<COLORREF> fgd_val;
	vector<COLORREF> bgd_val;

	bool b = false;
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (c != image.at<Vec3b>(i, j))
			{
				b = true;
			}

			if (!mask.empty())
			{
				if (mask.at<uchar>(i, j) == GC_FGD)
				{
					Vec3b v = image.at<Vec3b>(i, j);
					COLORREF t = RGB(v[2], v[1], v[0]);
					fgd_val.push_back(t);
				}
				else if (mask.at<uchar>(i, j) == GC_BGD)
				{
					Vec3b v = image.at<Vec3b>(i, j);
					COLORREF t = RGB(v[2], v[1], v[0]);
					bgd_val.push_back(t);
				}
			}
		}
	}

	if (!fgd_val.empty() && !bgd_val.empty())
	{
		sort(fgd_val.begin(), fgd_val.end());
		fgd_val.erase(unique(fgd_val.begin(), fgd_val.end()), fgd_val.end());

		sort(bgd_val.begin(), bgd_val.end());
		bgd_val.erase(unique(bgd_val.begin(), bgd_val.end()), bgd_val.end());

		if (fgd_val == bgd_val)
		{
			int c = GC_BGD;
			int c1 = GC_PR_FGD;
			int c2 = GC_FGD;
			int c3 = GC_PR_BGD;
			if (_opt == btn_seg_bgd)
			{
				c = GC_FGD;
				c1 = GC_PR_BGD;
				c2 = GC_BGD;
				c3 = GC_PR_FGD;
			}

			for (int i = 0; i < mask.rows; i++)
			{
				for (int j = 0; j < mask.cols; j++)
				{
					if (mask.at<uchar>(i, j) == c)
					{
						mask.at<uchar>(i, j) = c2;
					}
					else if (mask.at<uchar>(i, j) == c1)
					{
						mask.at<uchar>(i, j) = c3;
					}
				}
			}
		}
	}

	Mat result(image.size(), CV_8U, Scalar(GC_PR_BGD));

	if (!b)
	{
		if (_opt == btn_seg_bgd)
		{
			result.setTo(GC_BGD);
		}
		else if (_opt == btn_seg_fgd)
		{
			result.setTo(GC_FGD);
		}
	}
	else
	{
		result = mask.clone();

		if (!mask.empty())
		{
			rc_init = Rect(0, 0, 1, 1);
			grabCut(image, result, rc_init, bgModel, fgModel, 1, GC_INIT_WITH_MASK);
		}

		myt->step += 10;

		//compare(result, GC_PR_FGD, result, CMP_EQ);

		for (int i = 0; i < result.rows; i++)
		{
			for (int j = 0; j < result.cols; j++)
			{
				if (result.at<uchar>(i, j) == GC_PR_FGD)
				{
					result.at<uchar>(i, j) = 255;
				}
				else
				{
					result.at<uchar>(i, j) = 0;
				}

				if (!mask.empty())
				{
					if (mask.at<uchar>(i, j) == 1)
					{
						result.at<uchar>(i, j) = 1;
					}
					else if (mask.at<uchar>(i, j) == 0)
					{
						result.at<uchar>(i, j) = 0;
					}
				}
			}
		}

	}

	_inpaint_click = false;
	_working = act_display;

	if (!myt->stop)
	{
		myt->step = 100;
		_result = result;
		//save_to_file(bg, L"d:\\_weightedImage2.bmp");
		opt_t op;
		op.opt_type = _opt;
		op.nav_type = nav_segment;
		op.mask = _seg_mask.clone();
		op.image = _result.clone();
		op.pts = _pts;
		op.thickness = THICKNESS_VAL;
		add_opt(op);
	}
	myt->step = 0;
	myt->stop = false;
	myt->contours1 = 1;
}

void CPaint::grabcut_apply()
{
	if (_result.empty())
	{
		return;
	}

	if (_lena_w >= _dst_zoom_w)
	{
		//Mat image = _dst.clone();
		//resize_mask(image, _result, _dst_in_lena.size());
		resize(_result, _result, _dst_in_lena.size(), 0, 0, resizemode());
	}

	_eraser_mask = Mat::zeros(_weightedImage.size(), CV_8UC1);

	Mat tmp = _eraser_mask(_dst_in_lena);
	Mat ori = _weightedImage(_dst_in_lena);

	for (int i = 0; i < _result.rows; i++)
	{
		for (int j = 0; j < _result.cols; j++)
		{
			if (_result.at<uchar>(i, j) == GC_BGD)
			{
				tmp.at<uchar>(i, j) = 0;
				ori.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
			else
			{
				tmp.at<uchar>(i, j) = 255;
			}
		}
	}

	_seg_mask = Mat::zeros(Size(0, 0), CV_8UC1);
	_result = Mat::zeros(Size(0, 0), CV_8UC1);
	_rect_seg_select = Rect(0, 0, 0, 0);

	erase_lastopt(9, 9);

	opt_t op;
	op.opt_type = btn_seg_apply;
	op.nav_type = 0;
	op.mask = _eraser_mask.clone();
	op.image = _weightedImage.clone();
	op.pts = _pts;
	op.thickness = THICKNESS_VAL;
	add_opt(op);
}

void CPaint::change_bg()
{
	Mat paper = Mat::zeros(Size(0, 0), CV_8UC3);
	Rect rc0 = Rect(0, 0, 0, 0);
	bool b = false;
	if (!_opt_arr.empty() && _idx > 0 && _idx <= _opt_arr.size())
	{
		int opt_type = _opt_arr[_idx - 1].opt_type;
		if (opt_type == btn_change_bg)
		{
			if (!_opt_arr[_idx - 1].sticker_arr.empty())
			{
				b = true;
				paper = _opt_arr[_idx - 1].sticker_arr[0].dst;
				rc0 = _opt_arr[_idx - 1].sticker_arr[0].rc;
			}
		}
	}

	if (_eraser_mask.empty() && !b)
	{
		return;
	}

	if (!b)
	{
		Rect rc = get_rc255(_eraser_mask);

		Mat tmp;
		if (rc.width == _eraser_mask.cols && rc.height == _eraser_mask.rows)
		{
			paper = _weightedImage.clone();
			tmp = _eraser_mask;
		}
		else
		{
			paper = _weightedImage(rc).clone();
			tmp = _eraser_mask(rc);
		}

		cvtColor(paper, paper, CV_BGR2BGRA);
		vector<int> fromto = { 0, 3 };
		mixChannels(tmp, paper, fromto);
	}

	_seg_bg = Mat::zeros(Size(0, 0), CV_8UC3);
	select_bg_file(1, _seg_bg);

	if (_seg_bg.empty())
	{
		return;
	}
	_brfore_change_bg = Rect(_lena_in_frame.x, _lena_in_frame.y, _dst_zoom_w, _dst_zoom_h);

	opt_t op;
	op.opt_type = btn_change_bg;
	op.nav_type = 0;
	op.image = _seg_bg.clone();
	op.mask = _eraser_mask.clone();
	op.thickness = THICKNESS_VAL;

	_weightedImage = _seg_bg.clone();
	_lena_bak = _weightedImage.clone();
	_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);

	_lena_w = _weightedImage.cols;
	_lena_h = _weightedImage.rows;
	oneone();
	//_dst_zoom_w = (int)(_lena_w * _zoom_r);
	//_dst_zoom_h = (int)(_lena_h * _zoom_r);

	erase_lastopt(btn_change_bg, 9);

	z_sticker z;
	z.dst = paper.clone();
	z.idx = 1;
	z.id = 1;
	//resize(paper, z.dst, Size(paper.cols * _zoom_r, paper.rows * _zoom_r), 0, 0, resizemode());

	//	if (rc0.width != 0)
	//	{
	//		z.rc = rc0;
	//	}
	//	else
	{
		z.rc = Rect(100 + z.idx * 20, 30 + z.idx * 30, z.dst.cols, z.dst.rows);
	}

	_curr_sticker_id = z.id;

	_sticker_arr.clear();
	_sticker_arr.push_back(z);
	op.sticker_arr = _sticker_arr;
	add_opt(op);

	_opt = btn_change_bg;
	_last_opt = 0;
	_working = act_display;
}

//
//Mat CPaint::checkMask(InputArray _mask, Size size)
//{
//	Mat mask = _mask.getMat();
//	Mat gray;
//	if (mask.channels() > 1)
//		cvtColor(mask, gray, COLOR_BGRA2GRAY);
//	else
//	{
//		if (mask.empty())
//			gray = Mat(size.height, size.width, CV_8UC1, Scalar(255));
//		else
//			mask.copyTo(gray);
//	}
//
//	return gray;
//}
//
//
//void CPaint::myseamlessClone(InputArray _src, InputArray _dst, InputArray _mask, Point p, OutputArray _blend, int flags)
//{
//	//CV_INSTRUMENT_REGION();
//	CV_Assert(!_src.empty());
//
//	const Mat src = _src.getMat();
//	const Mat dest = _dst.getMat();
//	Mat mask = checkMask(_mask, src.size());
//	dest.copyTo(_blend);
//	Mat blend = _blend.getMat();
//
//	Mat mask_inner = mask(Rect(1, 1, mask.cols - 2, mask.rows - 2));
//	copyMakeBorder(mask_inner, mask, 1, 1, 1, 1, BORDER_ISOLATED | BORDER_CONSTANT, Scalar(0));
//
//	Rect roi_s = boundingRect(mask);
//	if (roi_s.empty()) return;
//	Rect roi_d(p.x - roi_s.width / 2, p.y - roi_s.height / 2, roi_s.width, roi_s.height);
//
//	Mat destinationROI = dest(roi_d).clone();
//
//	Mat sourceROI = Mat::zeros(roi_s.height, roi_s.width, src.type());
//	src(roi_s).copyTo(sourceROI, mask(roi_s));
//
//	Mat maskROI = mask(roi_s);
//	Mat recoveredROI = blend(roi_d);
//
//	//Cloning obj;
//	//obj.normalClone(destinationROI, sourceROI, maskROI, recoveredROI, flags);
//	seamlessClone(_src, _dst, _mask, p, _blend, flags);
//
//}
