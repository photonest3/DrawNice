///////////////////////////////////////////////////////////////////////
/// @file paint_btn_pool.cpp
/// @brief 绘图模块 - 按钮池和功能实现
/// @details 实现绘图工具栏按钮的资源管理、图像缩放适配、
///          以及 oneone() 等图像处理功能的入口
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件管理绘图工具的按钮状态和图像处理功能调用
///////////////////////////////////////////////////////////////////////
// ui.cpp : Defines the entry point for the console application.
//




#include "stdafx.h"
#include "paint.h"
#include "myinpaint.h"

#include "cvui.h"
#include "resource.h"
#include<fstream>
//#include <ShlObj.h>
#include <windows.h>
#include<commdlg.h>
#include "unitil.h"

extern int g_process;

void CPaint::oneone()
{
	int w0 = _lena_bak.cols;
	int h0 = _lena_bak.rows;

	if (w0 < _win_w - TOOLBAR_LEFT_WIDTH && h0 < _win_h - TOOLBAR_TOP_HEIGHT)
	{
		_dst_h = _lena_bak.rows;
		_dst_w = _lena_bak.cols;
		_dst = _weightedImage.clone();
	}
	else
	{
		float radio1 = (float)w0 / (float)h0;
		float radio2 = (float)(_win_w - TOOLBAR_LEFT_WIDTH) / (float)(_win_h - TOOLBAR_TOP_HEIGHT);
		if (radio1 > radio2)
		{
			_dst_w = _win_w * 0.9 - TOOLBAR_LEFT_WIDTH;
			_dst_h = _dst_w / radio1;
		}
		else
		{
			_dst_h = _win_h * 0.9 - TOOLBAR_TOP_HEIGHT;
			_dst_w = _dst_h * radio1;
		}


		resize(_weightedImage, _dst, Size(_dst_w, _dst_h), 0, 0, INTER_NEAREST);


	}
	xrect_img = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_w - TOOLBAR_LEFT_WIDTH) / 2;
	yrect_img = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_h - TOOLBAR_TOP_HEIGHT) / 2;

	rect_img.x = xrect_img;
	rect_img.y = yrect_img;

	reset_frame();

	_dst_rect = Rect(0, 0, _dst_w, _dst_h);

	Mat imageROI = frame(Rect(rect_img.x, rect_img.y, _dst_w, _dst_h));
	_dst.copyTo(imageROI);
}

void CPaint::undo()
{
	int len = _opt_arr.size();
	if (_idx == -100) {
		_idx = len;
	}

	_idx -= 1;
	if (_idx < 0)
	{
		_idx = 0;
	}

	redraw(_idx);

}

void CPaint::redo()
{
	int len = _opt_arr.size();
	if (_idx == -100) {
		_idx = len;
	}

	_idx += 1;
	if (_idx > len)
	{
		_idx = len;
	}

	redraw(_idx);

}

void CPaint::zoom(int type)
{
	float f = (float)_dst_h * 100 / (float)_lena_h;
	if (type == 0)
	{
		float v = 1.1;
		float temp = _dst_h * v;
		if (f >= 800)
		{
			v = (float)_lena_h * 8.0 / (float)_dst_h;
		}

		_dst_h = _dst_h * v;
		_dst_w = _dst_w * v;
		_ratio = (float)_lena_w / (float)_dst_w;

		rect_img.x -= (_dst_w * v - _dst_w) / 2;
		rect_img.y -= (_dst_h * v - _dst_h) / 2;
	}
	else if (type == 1)
	{
		float v = 0.9;
		float temp = _dst_h * v;
		if (f <= 12.5)
		{
			v = (float)_lena_h / 8.0 / (float)_dst_h;
		}

		_dst_h = _dst_h * v;
		_dst_w = _dst_w * v;
		_ratio = (float)_lena_w / (float)_dst_w;

		rect_img.x += (_dst_w - _dst_w * v) / 2;
		rect_img.y += (_dst_h - _dst_h * v) / 2;
	}
	else if (type == 3)
	{
		oneone();
	}

	_crop_opt = 0;
	_working = act_resize;
}

void CPaint::load()
{
	if (!_from_exe)
	{
		load_image(2, L"");
	}
	else
	{
		wchar_t szPathName[MAX_PATH] = { 0 };


		OPENFILENAME  open;// �����Ի���ṹ��
		char file[MAX_PATH];// ���������ȡ�ļ����ƵĻ������� 
		ZeroMemory(&open, sizeof(OPENFILENAME)); // ��ʼ��ѡ���ļ��Ի���
		open.lStructSize = sizeof(OPENFILENAME);//ָ������ṹ�Ĵ�С�����ֽ�Ϊ��λ��
		open.lpstrFile = szPathName;// file;//�򿪵��ļ���ȫ·��
		//open.lpstrFile[0] = '\0'; //��һ���ַ����ǹ�������������ʾ�ַ���
		open.nMaxFile = MAX_PATH;  //ָ��lpstrFile����Ĵ�С����TCHARsΪ��λ
		open.lpstrFilter = L"image(*.jpg)\0*.png\0�����ļ�(*.*)\0*.*\0\0";  //���ļ�����
		open.nFilterIndex = 1;  //ָ�����ļ����Ϳؼ��е�ǰѡ��Ĺ�����������
		open.lpstrFileTitle = NULL; // ָ�����ѡ����ļ����ļ�������չ���Ļ��壨����·����Ϣ���������Ա������NULL��
		open.nMaxFileTitle = 0;  //ָ��lpstrFileTitle����Ĵ�С����TCHARsΪ��λ
		open.lpstrInitialDir = NULL;  //ָ���Կ��ַ��������ַ���������������ַ�����ָ����ʼĿ¼��
		open.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//λ��ǵ����ã������ʹ������ʼ���Ի���
																		  //GetOpenFileName (&open) ;//���ļ��Ի���
																		  //GetSaveFileName(&open);//�����ļ��Ի���
		if (GetOpenFileName(&open))  // ��ʾ��ѡ���ļ��Ի���  
		{
			load_image(1, open.lpstrFile);
		}
	}
}

int CPaint::save()
{
	//GetSaveFileName
	Mat dst;
	if (_inpainted)
	{
		dst = _inpaintedImage;
	}
	else if (_croped)
	{
		dst = _src;
	}

	if (_inpainted || _croped)
	{
		vector<uchar> buf;
		cv::imencode(".jpg", dst, buf);

		wstring szDst2 = L"d:/data/szDst";
		szDst2 += L"_2.jpg";
		ofstream file2(szDst2.c_str(), ios::out | ios::binary);
		if (!file2)
		{
			return -1;
		}
		file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
		file2.close();
	}

	return 0;
}

void CPaint::rotate(int deg)
{
	opt_t op;
	op.opt_type = _opt;
	//op.pts = _pts;
	op.thickness = _thickness;

	if (deg == 180)
	{
		op.opt_type = btn_rota90;

		flip(_src, _src, -1);
		flip(_weightedImage, _weightedImage, -1);
		flip(_inpaintMask, _inpaintMask, -1);
		flip(_inpaintedImage, _inpaintedImage, -1);
		flip(_lena_bak, _lena_bak, -1);
		_total_deg += 180;
	}
	else
	{
		Mat temp_src;
		Mat temp_weightedImage;
		Mat temp_inpaintMask;
		Mat temp_inpaintedImage;
		Mat temp_lena_bak;

		transpose(_src, temp_src);
		transpose(_weightedImage, temp_weightedImage);
		transpose(_inpaintMask, temp_inpaintMask);
		transpose(_inpaintedImage, temp_inpaintedImage);
		transpose(_lena_bak, temp_lena_bak);

		if (deg == 270)
		{
			op.opt_type = btn_rota270;

			flip(temp_src, _src, 0);
			flip(temp_weightedImage, _weightedImage, 0);
			flip(temp_inpaintMask, _inpaintMask, 0);
			flip(temp_inpaintedImage, _inpaintedImage, 0);
			flip(temp_lena_bak, _lena_bak, 0);

			_total_deg -= 90;
		}
		else if (deg == 90)
		{
			op.opt_type = btn_rota90;

			flip(temp_src, _src, 1);
			flip(temp_weightedImage, _weightedImage, 1);
			flip(temp_inpaintMask, _inpaintMask, 1);
			flip(temp_inpaintedImage, _inpaintedImage, 1);
			flip(temp_lena_bak, _lena_bak, 1);
			_total_deg += 90;
		}

		int temp = _dst_h;
		_dst_h = _dst_w;
		_dst_w = temp;

		temp = _lena_h;
		_lena_h = _lena_w;
		_lena_w = temp;
		oneone();
	}

	_working = act_resize;

	add_opt(op);

	_idx = _opt_arr.size();
	_pts.clear();

}

void CPaint::crop()
{
	float f = (float)_dst_h * 100 / (float)_lena_h;
	if (f <= 12.5 || f >= 800)
	{
		return;
	}

	_src = _src(_rect_crop);
	_weightedImage = _weightedImage(_rect_crop);
	_inpaintMask = _inpaintMask(_rect_crop);
	_lena_bak = _lena_bak(_rect_crop);

	if (_inpainted)
	{
		_inpaintedImage = _inpaintedImage(_rect_crop);
	}
	_lena_w = _src.cols;
	_lena_h = _src.rows;
	oneone();

	_croped = true;
	_working = act_resize;

	opt_t op;
	op.opt_type = _opt;
	op.pts.push_back(Point(_rect_crop.x, _rect_crop.y));
	op.pts.push_back(Point(_rect_crop.x + _rect_crop.width, _rect_crop.y + _rect_crop.height));


	op.thickness = lasso_thickness;
	add_opt(op);

	_idx = _opt_arr.size();
	_pts.clear();

	_rect_crop = Rect(0, 0, 0, 0);
	_ratio = (float)_lena_w / (float)_dst_w;
	_crop_opt = 0;

	_pt_for_mouseup.x = 0;
	_pt_for_mouseup.y = 0;
}

UINT __stdcall CPaint::handle_inpaint(LPVOID pParam) {
	CPaint * pThis = (CPaint*)pParam;
	pThis->inpaint_proc();

	return 0;
}

void CPaint::draw_processing()
{
	int x = TOOLBAR_LEFT_WIDTH + (_win_w - 200 - TOOLBAR_LEFT_WIDTH) / 2;
	int y = TOOLBAR_TOP_HEIGHT + (_win_h - 30 - TOOLBAR_TOP_HEIGHT) / 2;

	Rect rect(x, y, 200, 30);
	cv::rectangle(frame, rect, Scalar(218, 218, 218), -1, LINE_8, 0);

	Rect rect2(x, y, g_process * 2, 30);
	cv::rectangle(frame, rect2, Scalar(16, 217, 46), -1, LINE_8, 0);

	cvui::printf(frame, x + 100, y + 10, 0.4, 0x000000, "%d%%", g_process);
}

void CPaint::inpaint_proc()
{
	int val255 = 0;
	int x0 = 10000;
	int y0 = 10000;
	int x1 = 0;
	int y1 = 0;

	for (int i = 0; i < _inpaintMask.rows; i++)
	{
		for (int j = 0; j < _inpaintMask.cols; j++)
		{
			if (_inpaintMask.at<uchar>(i, j) == 255)
			{
				val255++;
				if (j < x0)
				{
					x0 = j;
				}
				if (j > x1)
				{
					x1 = j;
				}
				if (i < y0)
				{
					y0 = i;
				}
				if (i > y1)
				{
					y1 = i;
				}
			}
		}
	}

	if (val255 == 0)
	{
		return;
	}

	x0 -= 100;
	if (x0 < 0) x0 = 0;

	y0 -= 100;
	if (y0 < 0) y0 = 0;

	x1 += 100;
	if (x1 > _inpaintMask.cols) x1 = _inpaintMask.cols;

	y1 += 100;
	if (y1 > _inpaintMask.rows) y1 = _inpaintMask.rows;


	_inpaintedImage = _src.clone();


	Rect rc = Rect(x0, y0, x1 - x0, y1 - y0);
	//Mat paint_temp;
	Mat src_temp = _src(rc).clone();
	Mat mask = _inpaintMask(rc).clone();
	Mat mask_temp;

	int r = 2;
	//if (rc.width > 1000 && rc.height > 1000)
	//{
	//	r = 4;
	//}
	//else if (rc.width > 500 && rc.height > 500)
	//{
	//	r = 2;
	//}
	if (r != 1)
	{
		resize(src_temp, src_temp, Size(rc.width / r, rc.height / r), 0, 0, INTER_NEAREST);
		resize(mask, mask_temp, Size(rc.width / r, rc.height / r), 0, 0, INTER_NEAREST);
	}



	std::vector< std::future<generate_struct> > results;
	ThreadPool pool(4);

	for (int j = 0; j < 4; j++)
	{
		//Mat mask2 = Mat::zeros(Size(rc.width / 2, rc.height / 2), CV_8UC1);
		Mat src0, mask0;
		if (j == 0)
		{
			src0 = src_temp(Rect(0, 0, rc.width / 4, rc.height / 4));
			mask0 = mask_temp(Rect(0, 0, rc.width / 4, rc.height / 4));
		}
		else if (j == 1)
		{
			src0 = src_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4));
			mask0 = mask_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4));
		}
		else if (j == 2)
		{
			src0 = src_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4));
			mask0 = mask_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4));
		}
		else if (j == 3)
		{
			src0 = src_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4));
			mask0 = mask_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4));
		}

		results.emplace_back(pool.enqueue([src0, mask0, j] {
			myinpaint p;
			Mat paint_temp;
			p.inpaint(src0, mask0, paint_temp, j);

			generate_struct v;
			v.idx = j;
			v.val = paint_temp;
			return v;
		})
		);
	}

	Mat paint_temp = Mat(cv::Size(rc.width / 2, rc.height / 2), CV_8UC3);
	for (auto && result : results)
	{
		generate_struct v = result.get();
		if (v.idx == 0)
		{
			v.val.copyTo(paint_temp(Rect(0, 0, rc.width / 4, rc.height / 4)));
		}
		else if (v.idx == 1)
		{
			v.val.copyTo(paint_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4)));
		}
		else if (v.idx == 2)
		{
			v.val.copyTo(paint_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4)));
		}
		else if (v.idx == 3)
		{
			v.val.copyTo(paint_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4)));
		}
	}

	/*

	for (int j = 0; j < 4; j++)
	{
	Mat mask2 = Mat(Size(rc.width / 2, rc.height / 2), CV_8UC1);
	mask2 = Scalar::all(100);

	Mat src0, mask0;
	if (j == 0)
	{
	//src0 = src_temp(Rect(0, 0, rc.width / 4, rc.height / 4));
	mask0 = mask_temp(Rect(0, 0, rc.width / 4, rc.height / 4));
	mask0.copyTo(mask2(Rect(0, 0, rc.width / 4, rc.height / 4)));
	}
	else if (j == 1)
	{
	//src0 = src_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4));
	mask0 = mask_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4));
	mask0.copyTo(mask2(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4)));
	}
	else if (j == 2)
	{
	//src0 = src_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4));
	mask0 = mask_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4));
	mask0.copyTo(mask2(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4)));
	}
	else if (j == 3)
	{
	//src0 = src_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4));
	mask0 = mask_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4));
	mask0.copyTo(mask2(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4)));
	}

	results.emplace_back(pool.enqueue([src_temp, mask2, j] {
	myinpaint p;
	Mat paint_temp;
	p.inpaint(src_temp, mask2, paint_temp, j);

	generate_struct v;
	v.idx = j;
	v.val = paint_temp;
	return v;
	})
	);
	}

	Mat paint_temp = Mat(cv::Size(rc.width / 2, rc.height / 2), CV_8UC3);
	for (auto && result : results)
	{
	generate_struct v = result.get();
	if (v.idx == 0)
	{
	v.val(Rect(0, 0, rc.width / 4, rc.height / 4)).copyTo(paint_temp(Rect(0, 0, rc.width / 4, rc.height / 4)));
	}
	else if (v.idx == 1)
	{
	v.val(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4)).copyTo(paint_temp(Rect(0, rc.height / 4, rc.width / 4, rc.height / 4)));
	}
	else if (v.idx == 2)
	{
	v.val(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4)).copyTo(paint_temp(Rect(rc.width / 4, 0, rc.width / 4, rc.height / 4)));
	}
	else if (v.idx == 3)
	{
	v.val(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4)).copyTo(paint_temp(Rect(rc.width / 4, rc.height / 4, rc.width / 4, rc.height / 4)));
	}
	}


	*/

	if (r != 1)
	{
		resize(paint_temp, paint_temp, Size(rc.width, rc.height), 0, 0, INTER_NEAREST);
	}

	//paint_temp.copyTo(_inpaintedImage(rc));


	int w = mask.cols;
	int h = mask.rows;

	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			if (mask.at<uchar>(i, j) == 255)
			{
				_inpaintedImage.at<Vec3b>(rc.y + i, rc.x + j) = paint_temp.at<Vec3b>(i, j);
			}
		}
	}


	reset_frame();
	_src = _inpaintedImage.clone();
	_weightedImage = _inpaintedImage.clone();
	_inpaintMask = Mat::zeros(_src.size(), CV_8UC1);


	//displayImage(2, _inpaintedImage);
	_inpainted = true;
	//_opt = 0;
	_inpaint_click = false;

	opt_t op;
	op.opt_type = 100;
	op.inpaintedImage = _inpaintedImage.clone();
	//op.pts = _pts;
	op.thickness = _thickness;
	add_opt(op);

	_idx = _opt_arr.size();

	//_opt
}

void CPaint::add_opt(opt_t op)
{
	op.ratio = _ratio;
	int len0 = _opt_arr.size();
	if (_curr_idx != -1 && len0 > _curr_idx)
	{
		_opt_arr.erase(std::begin(_opt_arr) + _curr_idx, std::end(_opt_arr));
	}
	_curr_idx = -1;

	_opt_arr.push_back(op);
}

void CPaint::before_crop(int sz)
{
	Point pt1;
	Point pt;
	if (sz == crop_0_0)
	{
		pt1.x = _rect_crop.x;
		pt1.y = _rect_crop.y;

		pt.x = _rect_crop.x + _rect_crop.width;
		pt.y = _rect_crop.y + _rect_crop.height;
	}
	else
	{

		int w = _dst_rect.width; // _dst.cols;
		int h = _dst_rect.height; // _dst.rows;
		float f = (float)w / (float)h;

		float fw = 1.0;
		float fh = 1.0;

		switch (sz)
		{
		case crop_1_1:
			break;
		case crop_4_3:
		{
			fw = 4.0;
			fh = 3.0;
		}
		break;
		case crop_3_2:
		{
			fw = 3.0;
			fh = 2.0;
		}
		break;
		case crop_5_4:
		{
			fw = 5.0;
			fh = 4.0;
		}
		break;
		case crop_7_5:
		{
			fw = 7.0;
			fh = 5.0;
		}
		break;
		case crop_16_9:
		{
			fw = 16.0;
			fh = 9.0;
		}
		break;
		}

		int ow = min(w, h);
		int oh = min(w, h);

		if (sz != crop_1_1)
		{
			if (f >= fw / fh)
			{
				ow = h * fw / fh;
				oh = h;
			}
			else
			{
				ow = w;
				oh = w * fh / fw;
			}
		}

		pt1.x = (w - ow) / 2 + _dst_rect.x;
		pt1.y = (h - oh) / 2 + _dst_rect.y;

		float f1 = (float)_weightedImage.cols / (float)_dst_w;

		pt1.x = pt1.x * f1 + _thickness * _ratio;
		pt1.y = pt1.y * f1 + _thickness * _ratio;

		pt.x = pt1.x + ow * f1 - 2 * _thickness * _ratio;
		pt.y = pt1.y + oh * f1 - 2 * _thickness * _ratio;

		_rect_crop.width = abs(pt1.x - pt.x);
		_rect_crop.height = abs(pt1.y - pt.y);
		_rect_crop.x = min(pt1.x, pt.x);
		_rect_crop.y = min(pt1.y, pt.y);
	}

	if (_rect_crop.width <= 0 || _rect_crop.height <= 0)
	{
		return;
	}

	reset_frame();

	Mat weighted;
	if (_inpainted)
	{
		weighted = _inpaintedImage.clone();
	}
	else
	{
		weighted = _weightedImage.clone();
	}

	drawDashRect(weighted, 1, 2, pt, pt1, CV_RGB(255, 255, 255), lasso_thickness * _ratio);

	Point pt4;
	pt4.x = pt.x - 50 * _ratio;
	pt4.y = pt.y - 50 * _ratio;

	drawDashRect(weighted, 1, 2, pt, pt4, CV_RGB(255, 255, 255), lasso_thickness * _ratio);

	Mat dst_temp;
	resize(weighted, dst_temp, Size(_dst_w, _dst_h), 0, 0, INTER_NEAREST);
	displayImage(1, dst_temp);

}
