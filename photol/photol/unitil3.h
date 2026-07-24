///////////////////////////////////////////////////////////////////////
/// @file unitil3.h
/// @brief PhotoNest 图像处理附加工具头文件,paint程序
/// @details 提供图像缩放比例计算、颜色均值计算、位图资源加载等函数:
///           - 获取缩放比例 (get_scale)
///           - 计算颜色均值 (calMEAN)
///           - 加载位图资源 (load_bitmap)
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件依赖 OpenCV 库
///////////////////////////////////////////////////////////////////////

#pragma once

#include <opencv2/opencv.hpp>

using namespace cv;

double get_scale(int type, int width, int height);
double calMEAN(Scalar result);
void load_bitmap(HINSTANCE his, Mat& img, UINT resID);
