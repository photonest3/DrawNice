///////////////////////////////////////////////////////////////////////
/// @file paint_draw.cpp
/// @brief PhotoNest 绘图模块 - 绘制功能实现文件
/// @details 实现绘图工具功能:
///           - 橡皮擦功能 (paint_eraser)
///           - 绘制笔触处理
///           - 点序列转掩码
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的核心功能文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "cvui.h"
#include "resource.h"
#include<fstream>
#include "unitil2.h"
#include <windows.h>

void CPaint::paint_eraser(int type, vector<Point> pts, int th)
{
	Mat _brushMask = Mat::zeros(_weightedImage.size(), CV_8UC1);

	int len = (int)pts.size();
	int st = 1;

	Point pt0 = pts[0];
	if (type == 1)
	{
		st = len - 2;
		pt0 = pts[st];
		st = len - 1;
	}

	int xx0 = pt0.x;
	int yy0 = pt0.y;
	int xx1 = pt0.x;
	int yy1 = pt0.y;

	for (int j = st; j < len; j++)
	{
		Point pt = pts[j];
		get_rect2(pt, xx0, yy0, xx1, yy1);
		line(_brushMask, pt0, pt, Scalar::all(255), th, 8, 0);

		/*		if (!_weighted_era.empty())
				{
					if (type == 1)
					{
						line(_weighted_era, pt0, pt, Scalar::all(255), th, 8, 0);
					}
					else
					{
						line(_eraser_mask, pt0, pt, Scalar::all(255), th, 8, 0);
					}
				}
		*/
		pt0 = pt;
	}

	get_rect1(th, _lena_w, _lena_h, xx0, yy0, xx1, yy1);
	Rect rect0(xx0, yy0, xx1 - xx0, yy1 - yy0);

	if (type == 1)
	{
		for (int i = rect0.y; i < rect0.y + rect0.height; ++i)
		{
			for (int j = rect0.x; j < rect0.x + rect0.width; ++j)
			{
				if (_brushMask.at<uchar>(i, j) == 255)
				{
					_weighted_mask.at<uchar>(i, j) = 0;
					_weighted_temp.at<Vec3b>(i, j) = _lena_bak.at<Vec3b>(i, j);
				}
			}
		}

		Mat dst_temp;// = _weighted_temp(_dst_in_lena).clone();
		resize(_weighted_temp(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());

		displayImage(100, dst_temp);
	}
	else
	{
		for (int i = rect0.y; i < rect0.y + rect0.height; ++i)
		{
			for (int j = rect0.x; j < rect0.x + rect0.width; ++j)
			{
				if (_brushMask.at<uchar>(i, j) == 255)
				{
					_inpaintMask.at<uchar>(i, j) = 0;
					_weightedImage.at<Vec3b>(i, j) = _lena_bak.at<Vec3b>(i, j);
				}
			}
		}
	}
}

void CPaint::paint_brush(int type, vector<Point> pts, int th)
{
	Mat beforeROI;
	Mat afterROI;
	int len = (int)pts.size();
	int st = 1;

	Point pt0 = pts[0];
	if (type == 1)
	{
		st = len - 2;
		pt0 = pts[st];
		st = len - 1;
	}

	int xx0 = pt0.x;
	int yy0 = pt0.y;
	int xx1 = pt0.x;
	int yy1 = pt0.y;

	for (int i = st; i < len; i++)
	{
		Point pt = pts[i];
		get_rect2(pt, xx0, yy0, xx1, yy1);
	}

	get_rect1(th, _lena_w, _lena_h, xx0, yy0, xx1, yy1);
	Rect rect(xx0, yy0, xx1 - xx0, yy1 - yy0);
	if (type == 1)
	{
		beforeROI = _weighted_mask(rect).clone();
		afterROI = _weighted_mask;
	}
	else
	{
		beforeROI = _inpaintMask(rect).clone();
		afterROI = _inpaintMask;
	}

	for (int i = st; i < len; i++)
	{
		Point pt = pts[i];
		line(afterROI, pt0, pt, Scalar::all(255), th, 8, 0);

		if (type == 1)
		{
			if (!_weighted_era.empty())
			{
				line(_weighted_era, pt0, pt, Scalar::all(255), th, 8, 0);
			}
		}
		pt0 = pt;
	}

	if (type == 1)
	{
		afterROI = afterROI(rect);

		int BChannel = 0, GChannel = 0, RChannel = 255;
		if (_opt == btn_draw_mosaic)
		{
			BChannel = _picker_val[0];
			GChannel = _picker_val[1];
			RChannel = _picker_val[2];
		}

		drawWeighted(_weighted_temp(rect), beforeROI, afterROI, BChannel, GChannel, RChannel);

		Mat dst_temp;
		resize(_weighted_temp(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());

		displayImage(100, dst_temp);

	}
	else
	{
		if (_opt == btn_free_select)
		{
			vector<vector<Point>> arr;
			arr.push_back(pts);
			fillPoly(afterROI, arr, Scalar::all(255));
		}

		afterROI = afterROI(rect);
		drawWeighted(_weightedImage(rect), beforeROI, afterROI);
	}
}

void CPaint::paint_mosaic(vector<Point> pts, int th)
{
	Point pt = pts[1];
	Point pt0 = pts[0];
	int x0 = pt0.x;
	int y0 = pt0.y;
	int x1 = pt0.x;
	int y1 = pt0.y;

	int len = (int)pts.size();
	for (int i = 0; i < len; i++)
	{
		Point pt2 = pts[i];
		get_rect2(pt2, x0, y0, x1, y1);
	}
	get_rect1(th, _lena_w, _lena_h, x0, y0, x1, y1);

	int neightbourHood = 9;
	RNG rng;

	Point ptL = Point(x0, y0);
	Point ptR = Point(x1, y1);

	Point pt9 = ptR;
	ptR.x < ptL.x ? ptR = ptL, ptL = pt9 : ptR = ptR;

	ptR.x > _weightedImage.cols - 2 * neightbourHood ? ptR.x = _weightedImage.cols - 2 * neightbourHood : ptR.x = ptR.x;
	ptR.y > _weightedImage.rows - 2 * neightbourHood ? ptR.y = _weightedImage.rows - 2 * neightbourHood : ptR.y = ptR.y;

	Mat ara = Mat::zeros(_weightedImage.size(), CV_8UC1);
	for (int i = 1; i < len; i++)
	{
		pt = pts[i];
		line(ara, pt0, pt, Scalar::all(255), th, 8, 0);
		pt0 = pt;
	}


	Mat beforeROI = _weightedImage.clone();
	//(Rect(ptL.x - neightbourHood, ptL.y - neightbourHood,
	//	ptR.x - ptL.x + neightbourHood, ptR.y - ptL.y + neightbourHood)).clone();

	for (int i = 0; i < ptR.y - ptL.y; i += neightbourHood)
	{
		for (int j = 0; j < ptR.x - ptL.x; j += neightbourHood)
		{
			int randomNum = 0;// rng.uniform(-neightbourHood / 2, neightbourHood / 2);
			//Rect rect2 = Rect(j + neightbourHood + ptL.x, i + neightbourHood + ptL.y, neightbourHood, neightbourHood);
			Rect rect2 = Rect(j + ptL.x, i + ptL.y, neightbourHood, neightbourHood);
			Mat roi = _weightedImage(rect2);
			int xx = j + randomNum + ptL.x;
			int yy = i + randomNum + ptL.y;
			Scalar sca = Scalar(
				beforeROI.at<Vec3b>(yy, xx)[0],
				beforeROI.at<Vec3b>(yy, xx)[1],
				beforeROI.at<Vec3b>(yy, xx)[2]);
			//Mat roiCopy = Mat(rect2.size(), CV_8UC3, sca);
			//roiCopy.copyTo(roi);

			if (ara.at<uchar>(yy, xx) == 255)
			{
				roi = sca;
			}
		}
	}

}

void CPaint::paint_free(int type, vector<Point> pts, int th)
{
	Point pt0 = pts[0];
	//if (type == 6)
	//{
	//	pt0.x = pt0.x * _dst_zoom_w / _lena_w + _lena_in_frame.x - _dst_in_frame.x;
	//	pt0.y = pt0.y * _dst_zoom_w / _lena_w + _lena_in_frame.y - _dst_in_frame.y;
	//}

	_weighted_temp = _weightedImage.clone();

	int len = (int)pts.size();
	for (int i = 1; i < len; i++)
	{
		Point pt = pts[i];

		if (_opt == btn_draw_free)
		{
			line(_weighted_temp, pt0, pt, _picker_val, th, 8, 0);
			//line(_lena_bak, pt0, pt, _picker_val, th, 8, 0);

			if (!_weighted_era.empty())
			{
				line(_weighted_era, pt0, pt, Scalar::all(255), th, 8, 0);
			}
		}
		else
		{

			line(_weighted_temp, pt0, pt, Scalar::all(255), th, 8, 0);
			//line(_lena_bak, pt0, pt, Scalar::all(255), th, 8, 0);
		}
		pt0 = pt;
	}

	Mat dst_temp;// = _weighted_temp(_dst_in_lena).clone();
	resize(_weighted_temp(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());

	displayImage(100, dst_temp);
}

void CPaint::drawDashRect(Mat img, int linelength, int dashlength, Point pt1, Point pt2)
{
	bool b = false;
	if (linelength == dashlength)
	{
		b = true;
	}

	int w = abs(pt1.x - pt2.x);
	int h = abs(pt1.y - pt2.y);

	int tl_x = min(pt1.x, pt2.x);
	int tl_y = min(pt1.y, pt2.y);

	//if (tl_x + w > img.cols || tl_y + h > img.rows )
	//{
	//	return;
	//}

	if (dashlength == 1)
	{
		double alphaReserve = 0.2;
		int	BChannel = 43;
		int	GChannel = 43;
		int	RChannel = 43;

		for (int i = 0; i < img.rows; i++)
		{
			Vec3b* ptr = img.ptr<Vec3b>(i);
			for (int j = 0; j < img.cols; j++)
			{
				if (i < tl_y || i > tl_y + h || j < tl_x || j > tl_x + w)
				{
				}
				else
				{
					alpha(ptr[j], BChannel, GChannel, RChannel, alphaReserve);
				}
			}
		}
	}
	else
	{
		Rect rc = Rect(0, 0, 0, 0);
		int offset = 0;
		if (dashlength == 2)
		{
			rc = Rect(_dst_in_frame.x, _dst_in_frame.y, _dst.cols, _dst.rows);
			offset = -5;
		}
		else if (dashlength == 3)
		{
			rc = Rect(0, 0, img.cols, img.rows);
			offset = 5;
		}

		if (rc.x + rc.width > img.cols)
		{
			rc.width = img.cols - rc.x;
		}
		if (rc.y + rc.height > img.rows)
		{
			rc.height = img.rows - rc.y;
		}

		if (offset != 0)
		{
			for (int i = rc.y; i < rc.height + rc.y; i++)
			{
				for (int j = rc.x; j < rc.width + rc.x; j++)
				{
					Vec3b& v = img.at<Vec3b>(i, j);
					if (i < tl_y || i > tl_y + h || j < tl_x || j > tl_x + w)
					{
						v[0] = saturate_cast<uchar>((int)(v[0] - offset));
						v[1] = saturate_cast<uchar>((int)(v[1] - offset));
						v[2] = saturate_cast<uchar>((int)(v[2] - offset));
					}
					else
					{
						v[0] = saturate_cast<uchar>((int)(v[0] + offset));
						v[1] = saturate_cast<uchar>((int)(v[1] + offset));
						v[2] = saturate_cast<uchar>((int)(v[2] + offset));
					}
				}
			}
		}
	}

	if (linelength == 0 && dashlength == 0)
	{
		return;
	}

	int thickness = 1;

	Scalar color = CV_RGB(255, 255, 255);
	Scalar color2 = CV_RGB(0, 0, 255);

	dashlength = 3;
	linelength = 4;

	int totallength = dashlength + linelength;
	int nCountX = w / totallength;
	int nCountY = h / totallength;

	CvPoint start, end;

	start.y = tl_y;
	start.x = tl_x;
	end.x = tl_x;
	end.y = tl_y;

	for (int i = 0; i < nCountX; i++)
	{
		start.x = tl_x + i * totallength;
		start.y = tl_y;
		end.x = tl_x + (i + 1) * totallength - dashlength;//draw top dash line
		end.y = tl_y;
		line(img, start, end, color, thickness);

		start.x = tl_x + i * totallength + linelength;
		end.x = tl_x + (i + 1) * totallength;//draw top dash line
		line(img, start, end, color2, thickness);

	}
	for (int i = 0; i < nCountX; i++)
	{
		start.x = tl_x + i * totallength;
		start.y = tl_y + h;
		end.x = tl_x + (i + 1) * totallength - dashlength;
		end.y = tl_y + h;
		line(img, start, end, color, thickness);

		start.x = tl_x + i * totallength + linelength;
		end.x = tl_x + (i + 1) * totallength;
		line(img, start, end, color2, thickness);

	}
	for (int i = 0; i < nCountY; i++)
	{
		start.x = tl_x;
		start.y = tl_y + i * totallength;
		end.y = tl_y + (i + 1) * totallength - dashlength;
		end.x = tl_x;
		line(img, start, end, color, thickness);

		start.y = tl_y + i * totallength + linelength;
		end.y = tl_y + (i + 1) * totallength;
		line(img, start, end, color2, thickness);


	}
	for (int i = 0; i < nCountY; i++)
	{
		start.x = tl_x + w;
		start.y = tl_y + i * totallength;
		end.y = tl_y + (i + 1) * totallength - dashlength;
		end.x = tl_x + w;
		line(img, start, end, color, thickness);

		start.y = tl_y + i * totallength + linelength;
		end.y = tl_y + (i + 1) * totallength;
		line(img, start, end, color2, thickness);
	}

	if (b)
	{
		return;
	}
	color2 = CV_RGB(0, 0, 0);

	Rect rc = Rect(tl_x - 2, tl_y - 2, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	rc = Rect(tl_x - 2 + w / 2, tl_y - 2, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	rc = Rect(tl_x - 2 + w, tl_y - 2, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	rc = Rect(tl_x - 2, tl_y - 2 + h / 2, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	rc = Rect(tl_x - 2 + w, tl_y - 2 + h / 2, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	//

	rc = Rect(tl_x - 2, tl_y - 2 + h, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);


	rc = Rect(tl_x - 2 + w / 2, tl_y - 2 + h, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);

	rc = Rect(tl_x - 2 + w, tl_y - 2 + h, 6, 6);
	rectangle(img, rc, color, -1, LINE_8, 0);
	rectangle(img, rc, color2, thickness, LINE_8, 0);
}

void CPaint::show_rect(Mat dst2, Rect rc, int opt)
{
	Point pt1;

	if (rc.width > 1)
	{
		pt1.x = rc.x + rc.width;
		pt1.y = rc.y + rc.height;

		if (opt == btn_crop/* || opt == btn_change_bg*/)
		{
			show_crop_apply(dst2, rc, pt1, opt);
		}
		else
		{
			pt1 = Point(rc.x, rc.y);
			Point pt2 = Point(pt1.x + rc.width, pt1.y + rc.height);

			drawDashRect(dst2, 1, 2, pt1, pt2);
		}
	}

}

void CPaint::show_crop_apply(Mat dst2, Rect rc, Point pt1, int opt)
{
	Point pt4;
	pt4.x = pt1.x - 50;
	pt4.y = pt1.y - 50;

	pt1 = Point(rc.x, rc.y);
	Point pt2 = Point(pt1.x + rc.width, pt1.y + rc.height);
	if (opt == btn_crop)
	{
		drawDashRect(dst2, 1, 2, pt1, pt2);
	}
	pt1 = Point(pt4.x, pt4.y);
	if (opt == btn_crop)
	{
		drawDashRect(dst2, 1, 1, pt1, pt2);
	}
	else
	{
		int w = abs(pt1.x - pt2.x);
		int h = abs(pt1.y - pt2.y);

		int tl_x = min(pt1.x, pt2.x);
		int tl_y = min(pt1.y, pt2.y);

		double alphaReserve = 0.2;
		int	BChannel = 43;
		int	GChannel = 43;
		int	RChannel = 43;

		for (int i = 0; i < dst2.rows; i++)
		{
			Vec3b* ptr = dst2.ptr<Vec3b>(i);
			for (int j = 0; j < dst2.cols; j++)
			{
				if (i > tl_y && i < tl_y + h && j > tl_x && j < tl_x + w)
				{
					alpha(ptr[j], BChannel, GChannel, RChannel, alphaReserve);
				}
			}
		}
	}

	int x1 = pt4.x + 8;
	int y1 = pt4.y + 8;
	if (x1 + apply_idle.cols > dst2.cols || y1 + apply_idle.rows > dst2.rows)
	{
		return;
	}

	Mat t = dst2(Rect(x1, y1, apply_idle.cols, apply_idle.rows));

	for (int i = 0; i < apply_idle.rows; i++)
	{
		for (int j = 0; j < apply_idle.cols; j++)
		{
			Vec3b v2 = apply_idle.at<Vec3b>(i, j);
			if (v2[0] != 43)
			{
				t.at<Vec3b>(i, j) = v2;
			}
		}
	}
}

void CPaint::seamless_10(Mat& dst, Rect& roi, Mat src, Point pt, int loDiff, int upDiff, int color)
{
	if (!_eraser_mask.empty())
	{
		for (int i = 0; i < _eraser_mask.rows; i++)
		{
			for (int j = 0; j < _eraser_mask.cols; j++)
			{
				if (_eraser_mask.at<uchar>(i, j) == 0)
				{
					src.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				}
			}
		}
	}
	else
	{
		if (color == 0)
		{
			dst = Mat(src.size(), CV_8UC1, 255);
		}
	}

	Point seed = pt;
	int lo = loDiff;
	int up = upDiff;
	int flags = _connectivity + (_newMaskVal << 8) + FLOODFILL_FIXED_RANGE;

	int b = 255;
	int g = 255;
	int r = 255;

	Mat flood = Mat(src.rows + 2, src.cols + 2, CV_8UC1);
	flood = Scalar::all(0);

	threshold(flood, flood, 1, 128, CV_THRESH_BINARY);

	Scalar newVal = Scalar(b, g, r);
	int area = floodFill(src, flood, seed, newVal, &roi, Scalar(up, up, up), Scalar(up, up, up), flags);

	//if (pt.x == 0 && pt.y == 0)
	//{
	//	for (int i = 0; i < src.rows; ++i)
	//	{
	//		for (int j = 0; j < src.cols; ++j)
	//		{
	//			//由于mask的尺寸大于原图，所以原图的坐标(x,y)对应于模板为(x+1,y+1)
	//			if (flood.at<uchar>(i + 1, j + 1) != 255)
	//			{
	//				dst.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
	//			}
	//		}
	//	}
	//}

	for (int i = roi.y; i < roi.y + roi.height; ++i)
	{
		for (int j = roi.x; j < roi.x + roi.width; ++j)
		{
			if (flood.at<uchar>(i + 1, j + 1) == 255)
			{
				dst.at<uchar>(i, j) = color;
			}
		}
	}

}

void CPaint::inpaint_proc()
{
	_inpaint_click = true;
	int inp = 0;
	for (int i = 0; i < _idx; i++)
	{
		int opt_type = _opt_arr[i].opt_type;
		if (opt_type == inpaint_ok)
		{
			inp = i;
		}
	}

	if (inp > 0 && inp == _idx - 1)
	{
		_weightedImage = _opt_arr[inp].image.clone();
		_inpaintMask = _opt_arr[inp].mask.clone();
		_lena_bak = _weightedImage.clone();
	}

	//if (!_eraser_mask.empty())
	//{
	//	for (int i = 0; i < _eraser_mask.rows; i++)
	//	{
	//		for (int j = 0; j < _eraser_mask.cols; j++)
	//		{
	//			if (_eraser_mask.at<uchar>(i, j) == 0)
	//			{
	//				_weightedImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
	//			}
	//		}
	//	}
	//}

	t_struct* myt = get_thread();

	vector<vector<Point>> contours1;
	vector<Vec4i> hierarchy1;
	findContours(_inpaintMask, contours1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	myt->contours1 = (int)contours1.size();
	Rect rcs[30] = {};

	//drawContours(temp, contours1, -1, (0, 0, 255), 3);
	for (int i = 0; i < contours1.size(); i++)
	{
		int x0 = 10000;
		int y0 = 10000;
		int x1 = 0;
		int y1 = 0;

		for (int j = 0; j < contours1[i].size(); j++)
		{
			int x = contours1[i][j].x;
			int y = contours1[i][j].y;
			if (x < x0)
			{
				x0 = x;
			}
			if (x > x1)
			{
				x1 = x;
			}
			if (y < y0)
			{
				y0 = y;
			}
			if (y > y1)
			{
				y1 = y;
			}
		}
		int w = (x1 - x0) < 150 ? 150 : x1 - x0;
		int h = (y1 - y0) < 150 ? 150 : y1 - y0;
		x0 -= w;
		if (x0 < 0) x0 = 0;

		y0 -= h;
		if (y0 < 0) y0 = 0;

		x1 += w;
		if (x1 > _inpaintMask.cols) x1 = _inpaintMask.cols;

		y1 += h;
		if (y1 > _inpaintMask.rows) y1 = _inpaintMask.rows;

		rcs[i] = Rect(x0, y0, x1 - x0, y1 - y0);
	}

	vector< future<p_struct> > results;
	for (int j = 0; j < contours1.size(); j++)
	{
		vector<Point> pts = contours1[j];
		Rect rc = rcs[j];
		results.emplace_back(_pool->enqueue([rc, pts, myt, this] {

			Mat paint_temp;
			Mat src_temp = _weightedImage(rc).clone();
			Mat mask = _inpaintMask(rc).clone();
			Mat mask_temp = mask;

			int w = rc.width;
			int h = rc.height;

			int m = max(w, h);
			double r = 600.0f / (double)(m);
			if (r < 1.0f)
			{
				w = (int)(w * r);
				h = (int)(h * r);
			}
			if (r < 1.0f)
			{
				resize(src_temp, src_temp, Size(w, h), 0, 0, resizemode());
				resize(mask, mask_temp, Size(w, h), 0, 0, resizemode());
			}

			myinpaint p;
			p.inpaint(src_temp, mask_temp, paint_temp, 0, myt);

			if (!paint_temp.empty())
			{
				if (r < 1.0f)
				{
					resize(paint_temp, paint_temp, Size(rc.width, rc.height), 0, 0, resizemode());
				}
			}

			p_struct v;
			v.rc = rc;
			v.pts = pts;
			v.image = paint_temp;
			v.mask = mask;
			return v;

		})
		);
	}

	Mat img = _weightedImage.clone();
	//Mat mask0 = _eraser_mask.clone();

	for (auto&& result : results)
	{
		if (!myt->stop)
		{
			//myt->step = 98 * myt->contours1;

			p_struct v = result.get();
			Rect rc = v.rc;
			vector<Point> pts = v.pts;
			Mat paint_temp = v.image;
			Mat mask = v.mask;

			if (!paint_temp.empty())
			{
				for (int i = 0; i < mask.rows; i++)
				{
					for (int j = 0; j < mask.cols; j++)
					{
						Point pt = Point(rc.x + j, rc.y + i);
						double f = pointPolygonTest(pts, pt, false);
						//if (mask.at<uchar>(i, j) != 0)
						if (f >= 0)
						{
							img.at<Vec3b>(rc.y + i, rc.x + j) = paint_temp.at<Vec3b>(i, j);
							//if (!_eraser_mask.empty())
							//{
							//	mask0.at<uchar>(rc.y + i, rc.x + j) = 255;
							//}
						}
					}
				}
			}
		}
	}

	if (!myt->stop)
	{
		_weightedImage = img;
		//_eraser_mask = mask0;

		myt->step = 100 * myt->contours1;

		_inpaint_click = false;

		int opt_type = 0;
		int len0 = (int)_opt_arr.size();
		for (int i = 0; i < len0; i++)
		{
			opt_type = _opt_arr[i].opt_type;
		}
		if (opt_type == inpaint_ok)
		{
			_opt_arr.erase(end(_opt_arr) - 1);
		}

		opt_t op;
		op.opt_type = inpaint_ok;
		op.nav_type = 0;
		op.mask = _inpaintMask.clone();
		op.image = _weightedImage.clone();
		//op.pts = _pts;
		op.thickness = THICKNESS_VAL;
		add_opt(op);

		_lena_bak = _weightedImage.clone();
		_inpaintMask = Mat::zeros(_weightedImage.size(), CV_8UC1);
	}

	myt->step = 0;
	myt->stop = false;
	myt->contours1 = 1;
	myt->hdl = 0;
}


Mat CPaint::SkySeparation(Mat src, Inputparama input)
{
	// 异常数值修正
	input.low_h = max(uchar(0), min(uchar(255), input.low_h));
	input.high_h = max(uchar(0), min(uchar(255), input.high_h));
	input.low_s = max(uchar(0), min(uchar(255), input.low_s));
	input.high_s = max(uchar(0), min(uchar(255), input.high_s));
	input.low_v = max(uchar(0), min(uchar(255), input.low_v));
	input.high_v = max(uchar(0), min(uchar(255), input.high_v));
	input.close_size = max(0, min(10, input.close_size));
	input.blur_size = max(0, min(10, input.blur_size));

	// 转为hsv通道
	Mat hsv, nhsv, thresh;
	cvtColor(src, hsv, COLOR_BGR2HSV);
	vector<Mat> hsvs;
	split(hsv, hsvs);
	Mat h, s, v;

	// 直方图均衡化
	equalizeHist(hsvs[1], s);
	equalizeHist(hsvs[2], v);
	hsvs[1] = s.clone();
	hsvs[2] = v.clone();
	merge(hsvs, nhsv);

	// 按天空色选出mask并反相
	Mat low = (Mat_<uchar>{ input.low_h, input.low_s, input.low_v });
	Mat high = (Mat_<uchar>{ input.high_h, input.high_s, input.high_v  });
	inRange(nhsv, low, high, thresh);
	Mat thresh_ = 255 - thresh;

	// 寻找轮廓，找出最大轮廓作为前景图
	vector<vector<Point>> contour;// , ncontour;
	vector<Vec4i> hierarchy;
	findContours(thresh_, contour, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	Mat Foreground = thresh_.clone();
	if (!contour.empty() && !hierarchy.empty())
	{
		int max = 0;
		vector<vector<Point> >::const_iterator itc = contour.begin();
		vector<vector<Point> >::const_iterator itmax;
		// 遍历所有轮廓
		int i = 1;
		while (itc != contour.end())
		{
			double area = contourArea(*itc);
			if (area > max)
			{
				itmax = itc;
				max = (int)area;
			}
			itc++;
		}
		for (auto it = contour.begin(); it != contour.end(); it++)
		{
			if (it != itmax)
			{
				Rect rect = boundingRect(Mat(*it));
				for (int i = rect.y; i < rect.y + rect.height; i++)
				{
					uchar* output_data = Foreground.ptr<uchar>(i);
					for (int j = rect.x; j < rect.x + rect.width; j++)
					{
						// 将连通区的值置0
						if (output_data[j] == 255)
						{
							output_data[j] = 0;
						}
					}
				}
			}
		}
	}

	// 闭运算
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * input.close_size + 1, 2 * input.close_size + 1));
	morphologyEx(Foreground, Foreground, MORPH_CLOSE, element);

	// 滤波
	blur(Foreground, Foreground, Size(2 * input.blur_size + 1, 2 * input.blur_size + 1));

	return Foreground;
}

Mat CPaint::ImageFusion(Mat src1, Mat src2, Mat mask)
{
	Mat sky;
	resize(src2, sky, Size(src1.cols, src1.rows));
	Mat result = src1.clone();
	int row = src1.rows;
	int col = src1.cols;
	// 改色
	for (int i = 0; i < row; ++i)
	{
		uchar* s1 = result.ptr<uchar>(i);
		uchar* s2 = sky.ptr<uchar>(i);
		uchar* m = mask.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			// 蒙版为0的区域就是标准背景区
			if (m[j] == 0)
			{
				s1[3 * j] = s2[3 * j];
				s1[3 * j + 1] = s2[3 * j + 1];
				s1[3 * j + 2] = s2[3 * j + 2];
			}
			// 不为0且不为255的区域是轮廓区域（边缘区），需要虚化处理
			else if (m[j] != 255)
			{
				// 边缘处按比例上色
				int newb = (int)((s1[3 * j] * m[j] * 0.3 + s2[3 * j] * (255 - m[j]) * 0.7) / ((255 - m[j]) * 0.7 + m[j] * 0.3));
				int newg = (int)((s1[3 * j + 1] * m[j] * 0.3 + s2[3 * j + 1] * (255 - m[j]) * 0.7) / ((255 - m[j]) * 0.7 + m[j] * 0.3));
				int newr = (int)((s1[3 * j + 2] * m[j] * 0.3 + s2[3 * j + 2] * (255 - m[j]) * 0.7) / ((255 - m[j]) * 0.7 + m[j] * 0.3));
				newb = max(0, min(255, newb));
				newg = max(0, min(255, newg));
				newr = max(0, min(255, newr));
				s1[3 * j] = newb;
				s1[3 * j + 1] = newg;
				s1[3 * j + 2] = newr;
			}
		}
	}
	return result;
}

int CPaint::change_sky()
{
	int idx = 0;
	int opt_type = 0;
	int len0 = (int)_opt_arr.size();
	for (int i = 0; i < len0; i++)
	{
		idx = i;
		opt_type = _opt_arr[i].opt_type;
	}

	Mat src;
	if (opt_type == btn_draw_sky)
	{
		src = _opt_arr[idx].bgra;
	}
	else
	{
		src = _weightedImage;
	}

	Inputparama input;
	input.low_h = 78;
	input.high_h = 124;
	input.low_s = 0;
	input.high_s = 255;
	input.low_v = 78;
	input.high_v = 255;
	input.close_size = 4;
	input.blur_size = 2;

	Mat thresh = SkySeparation(src, input);

	bool b = false;
	int row = thresh.rows;
	int col = thresh.cols;
	for (int i = 0; i < row; ++i)
	{
		uchar* m = thresh.ptr<uchar>(i);
		for (int j = 0; j < col; ++j)
		{
			if (m[j] == 0)
			{
				b = true;
				break;
			}
		}
	}

	//int z = countNonZero(thresh);
	if (!b)
	{
		_disp_nb = 1;
		_show_err_type = 3;
		return 0;
	}
	else
	{
		Mat sky = Mat::zeros(Size(0, 0), CV_8UC3);
		select_bg_file(2, sky);
		if (sky.empty())
		{
			return 0;
		}

		Mat result = ImageFusion(src, sky, thresh);
		_weightedImage = result;

		if (opt_type == btn_draw_sky)
		{
			_opt_arr.erase(end(_opt_arr) - 1);
		}

		opt_t op;
		op.opt_type = btn_draw_sky;
		op.nav_type = 0;
		op.thickness = THICKNESS_VAL;
		op.image = result.clone();
		op.bgra = src.clone();
		add_opt(op);

		_working = act_display;
		_pointer_btn = btn_move;
	}
	return 0;
}
