///////////////////////////////////////////////////////////////////////
/// @file unitil3.cpp
/// @brief PhotoNest 图像处理附加工具实现文件
/// @details 实现图像缩放比例计算、颜色均值计算、位图资源加载等函数:
///           - 获取缩放比例 (get_scale)
///           - 计算颜色均值 (calMEAN)
///           - 加载位图资源 (load_bitmap)
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件依赖 OpenCV 库
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "unitil2.h"
#include "unitil3.h"
#include <opencv2/quality.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>


double get_scale(int type, int width, int height)
{
	double scale_denom = 8.0f;

	int max0 = width;
	if (height > width)
	{
		max0 = height;
	}

	int min0 = width;
	if (height < width)
	{
		min0 = height;
	}

	if (type == 2)
	{
		scale_denom = 1.0f;
		if (min0 > 30)
		{
			scale_denom = (double)min0 / 30.0f;
		}
	}
	else
	{
		double maxWidth = 1280;
		double maxHeight = 720;

		double outW = 0;
		double outH = 0;

		if (width < maxWidth && height < maxHeight)
		{
			outW = width;
			outH = height;
			scale_denom = 1.0f;
		}
		else
		{
			double f0 = maxWidth / maxHeight;
			double f1 = (double)width / (double)height;

			if (f0 <= f1)
			{
				outW = maxWidth;
				outH = maxWidth / f1;
				scale_denom = (double)width / maxWidth;
			}
			else
			{
				outW = maxHeight * f1;
				outH = maxHeight;
				scale_denom = (double)height / maxHeight;
			}
		}
	}
	return scale_denom;
}

double calMEAN(Scalar result)
{
	int i = 0;
	double sum = 0;
	for (auto val : result.val)
	{
		if (0 == val || isinf(val))
		{
			break;
		}
		sum += val;
		i++;
	}

	if (i > 0)
	{
		return sum / i;
	}

	return 0;
}

void load_bitmap(HINSTANCE his, Mat& img, UINT resID)
{
	HBITMAP hbmp = ::LoadBitmap(his, MAKEINTRESOURCE(resID));
	DWORD err = GetLastError();
	if (hbmp)
	{
		BITMAP bm = { 0 };
		int ret = GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bm);
		if (ret == 0 || ret > sizeof(BITMAP))
		{
		}
		else
		{
			int nChannels = bm.bmBitsPixel == 1 ? 1 : bm.bmBitsPixel / 8;
			int depth = bm.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

			//Mat src;CV_8UC3
			img.create(cvSize(bm.bmWidth, bm.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
			GetBitmapBits(hbmp, bm.bmHeight * bm.bmWidth * nChannels, img.data);

			if (nChannels == 4)
			{
				cvtColor(img, img, CV_BGRA2BGR);
			}
		}
		DeleteObject(hbmp);
	}
}

