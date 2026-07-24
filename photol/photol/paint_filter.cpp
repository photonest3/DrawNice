///////////////////////////////////////////////////////////////////////
/// @file paint_filter.cpp
/// @brief PhotoNest 绘图模块 - 滤镜功能实现文件
/// @details 实现图像滤镜和修复功能:
///           - 滤镜应用 (filter_ok)
///           - 图像修复 (myinpaint)
///           - 图像质量评估
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件依赖 OpenCV 和 myinpaint 模块
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "myinpaint.h"

#include "cvui.h"
#include "resource.h"
#include<fstream>
#include <windows.h>
#include<commdlg.h>
#include "unitil2.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include "algorithm"
#include "vector"
#include "stdio.h"
#include "map"
#include "unordered_map"
#include <math.h>
//#include "imageSkyDetector.h"

using namespace std;
using namespace cv;

void CPaint::filter_ok(int ty)
{
	if (_lena2.empty())
	{
		return;
	}

	if (ty == 1)
	{
		Mat dst_temp;
		resize(_lena_bak(_dst_in_lena), dst_temp, _dst.size(), 0, 0, resizemode());
		filter(dst_temp, _dst);
	}
	else
	{
		filter(_lena_bak, _weightedImage);

		opt_t op;
		op.opt_type = btn_filter;
		op.nav_type = nav_filter;
		op.fVal = _fVal;
		op.mask = _lena_bak.clone();
		op.image = _weightedImage.clone();
		op.thickness = THICKNESS_VAL;
		add_opt(op);
	}
}

void CPaint::filter(Mat src, Mat& dst)
{
	if (_fVal.use_brightValue || _fVal.use_contrastValue || _fVal.use_comic_strip || _fVal.use_vintage || _fVal.use_feather || _fVal.use_sketch || _fVal.use_decolor)
	{
		Bright(src, dst, _fVal);
	}
	else
	{
		dst = src.clone();
	}
}

void CPaint::Bright(const Mat src, Mat& dst, z_filter fVal)
{
	dst = Mat(src.size(), CV_8UC3);

	int cb_ = fVal.brightValue;
	int degree = fVal.contrastValue;

	int rows = src.rows;
	int cols = src.cols;
	if (cb_ < -255)
		cb_ = -255;
	if (cb_ > 255)
		cb_ = 255;

	if (degree < -100) degree = -100;
	if (degree > 100) degree = 100;
	double contrast = (100.0 + degree) / 100.0;
	contrast *= contrast;

	int kk = 8;

	int w = cols / kk;
	int h = rows / kk;

	// s2
	int center_x = src.cols >> 1;
	int center_y = src.rows >> 1;
	int s2 = center_x * center_x + center_y * center_y;

	// 宽长比例 ratio
	double ratio = src.cols > src.rows ? static_cast<double>(src.rows) / src.cols : static_cast<double>(src.cols) / src.rows;

	// mSize
	// 2、通过控制V值的大小实现范围控制。
	double mSize = 0.5;


	vector< future<z_struct> > results;

	for (int i = 0; i < kk; i++)
	{
		for (int j = 0; j < kk; j++)
		{
			int w1 = (i == kk - 1) ? cols - (kk - 1) * w : w;
			int h1 = (j == kk - 1) ? rows - (kk - 1) * h : h;
			Rect rc = Rect(i * w, j * h, w1, h1);

			results.emplace_back(_pool->enqueue([src, dst, cb_, rc, contrast, fVal, center_x, center_y, ratio, s2, mSize]
			{
				Mat s = src(rc);
				Mat d = dst(rc);
				int row = s.rows;
				int col = s.cols;

				//if (fVal.use_sketch)
			//{
				Mat gray(s.size(), CV_8UC3);
				Mat gray_revesal(s.size(), CV_8UC3);
				//}

				for (int i = 0; i < row; i++)
				{
					Vec3b* ptr = s.ptr<Vec3b>(i);
					Vec3b* ptrD = d.ptr<Vec3b>(i);
					for (int j = 0; j < col; j++)
					{
						if (fVal.use_decolor)
						{
							int max = std::max(std::max(ptr[j][0], ptr[j][1]), ptr[j][2]);
							int min = std::min(std::min(ptr[j][0], ptr[j][1]), ptr[j][2]);
							for (int k = 0; k < 3; k++)
							{
								ptrD[j][k] = (max + min) / 2;
							}
						}
						else
						{
							for (int k = 0; k < 3; k++)
							{
								ptrD[j][k] = ptr[j][k];
							}
						}

						//3连环画comic strip
						if (fVal.use_comic_strip)
						{
							int b = ptrD[j][0];
							int g = ptrD[j][1];
							int r = ptrD[j][2];

							ptrD[j][0] = saturate_cast<uchar>(std::abs(b - g + b + r) * g / 256);
							ptrD[j][1] = saturate_cast<uchar>(std::abs(b - g + b + r) * r / 256);
							ptrD[j][2] = saturate_cast<uchar>(std::abs(g - b + g + r) * r / 256);
						}

						//4vintage怀旧
						if (fVal.use_vintage)
						{
							int B = ptrD[j][0];
							int G = ptrD[j][1];
							int R = ptrD[j][2];

							//4							
							ptrD[j][0] = saturate_cast<uchar>(0.272 * R + 0.534 * G + 0.131 * B);// blue
							ptrD[j][1] = saturate_cast<uchar>(0.349 * R + 0.686 * G + 0.168 * B);// green
							ptrD[j][2] = saturate_cast<uchar>(0.393 * R + 0.769 * G + 0.189 * B);// red
						}

						//5 feather羽化				
						if (fVal.use_feather)
						{
							double dx = static_cast<double>(std::abs(center_x - static_cast<int>(j + rc.x)));
							double dy = static_cast<double>(std::abs(center_y - static_cast<int>(i + rc.y)));


							//4、s1 有根据 ratio 修正 dx dy值。
							if (center_x > center_y)
							{
								dx = dx * ratio;
							}
							else
							{
								dy = dx * ratio;
							}

							// s1
							double s1 = dx * dx + dy * dy;
							// v
							// 3、V = 255 * 当前点Point距中点距离的平方s1 / (顶点距中点的距离平方s2 * mSize);
							double v = 255 * s1 / (s2 * mSize);

							int b = ptrD[j][0];
							int g = ptrD[j][1];
							int r = ptrD[j][2];

							// 1、通过对rgb值增加额外的V值实现朦胧效果
							ptrD[j][0] = saturate_cast<uchar>(b + v);
							ptrD[j][1] = saturate_cast<uchar>(g + v);
							ptrD[j][2] = saturate_cast<uchar>(r + v);
						}

						if (fVal.use_brightValue || fVal.use_contrastValue)
						{
							for (int k = 0; k < 3; k++)
							{
								int val = ptrD[j][k];

								/*
								if (fVal.use_brightValue)
								{
									int x1=(i==0)? 0 : i - 1;
									int y1=(j==0) ? 0: j - 1;
									val = ptrD[j][k] - d.at<Vec3b>(x1, y1)[k] + 128; // 雕刻
								//int res2 = img.at<Vec3b>(i + 1, j + 1)[k] - img.at<Vec3b>(i - 1, j - 1)[k] + 128; //浮雕

								//relief1.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(res1);
								//relief2.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(res2);
								}
								*/


								//1亮度
								if (fVal.use_brightValue)
								{
									val = val + cb_;
									if (val < 0) val = 0;
									else if (val > 255) val = 255;
								}

								//2对比度
								if (fVal.use_contrastValue)
								{
									val = (int)(((1.0 * val / 255.0 - 0.5) * contrast + 0.5) * 255);
									if (val < 0) val = 0;
									else if (val > 255) val = 255;
								}

								ptrD[j][k] = val;

							}
						}

						if (fVal.use_sketch)
						{
							int max = std::max(std::max(ptrD[j][0], ptrD[j][1]), ptrD[j][2]);
							int min = std::min(std::min(ptrD[j][0], ptrD[j][1]), ptrD[j][2]);

							for (int k = 0; k < 3; k++)
							{
								//1、去色
								gray.at<Vec3b>(i, j)[k] = (max + min) / 2;
								//2、复制去色图层，并且反色
								gray_revesal.at<Vec3b>(i, j)[k] = 255 - gray.at<Vec3b>(i, j)[k];
							}

						}

					}
				}

				if (fVal.use_sketch)
				{
					GaussianBlur(gray_revesal, gray_revesal, Size(7, 7), 0);
					for (int i = 0; i < row; i++)
					{
						Vec3b* ptrD = d.ptr<Vec3b>(i);
						for (int j = 0; j < col; j++)
						{
							for (int k = 0; k < 3; k++)
							{
								int a = gray.at<Vec3b>(i, j)[k];
								int b = gray_revesal.at<Vec3b>(i, j)[k];
								b = (b == 255) ? 254 : b;
								int c = std::min(a + (a * b) / (255 - b), 255);

								ptrD[j][k] = c;
							}
						}
					}
				}

				z_struct v;
				v.rc = rc;
				v.dst = d;

				return v;
			}));
		}
	}

	for (auto&& result : results)
	{
		z_struct v = result.get();
		v.dst.copyTo(dst(v.rc));
	}
}
