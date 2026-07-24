///////////////////////////////////////////////////////////////////////
/// @file paint_pt.cpp
/// @brief 绘图模块 - 点操作和裁剪工具实现
/// @details 实现鼠标点坐标转换、矩形范围扩展、裁剪区域计算等
///          与点和裁剪相关的工具函数
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理图像坐标系统和裁剪区域的逻辑计算
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "resource.h"

// 将窗口/目标坐标转换为 Lena 图像坐标（考虑当前缩放比例）
Point CPaint::get_lena_pt(int x, int y)
{
	int x0 = x;
	int y0 = y;
	if (_dst_zoom_w > 0)
	{
		x0 = x * _lena_w / _dst_zoom_w;
		y0 = y * _lena_w / _dst_zoom_w;
	}
	return Point(x0, y0);
}

// 扩展矩形范围，按厚度 thick 扩展并限制到边界 w/h
void CPaint::get_rect1(int thick, int w, int h, int &x0, int &y0, int &x1, int &y1)
{
	x0 = max(x0 - thick, 0);
	y0 = max(y0 - thick, 0);
	x1 = min(x1 + thick, w);
	y1 = min(y1 + thick, h);
}

// 根据给定点更新矩形四边（取 min/max）
void CPaint::get_rect2(Point pt, int &x0, int &y0, int &x1, int &y1)
{
	x0 = min(x0, pt.x);
	y0 = min(y0, pt.y);
	x1 = max(x1, pt.x);
	y1 = max(y1, pt.y);
}

// 将点限制在左上边界(lf,tp)与当前目标图像框(_dst_in_frame + _dst)之间
void CPaint::reset_pt(Point &pt, int lf, int tp)
{
	if (pt.x < lf)
	{
		pt.x = lf;
	}
	else if (pt.x > _dst_in_frame.x + _dst.cols)
	{
		pt.x = _dst_in_frame.x + _dst.cols;
	}
	if (pt.y < tp)
	{
		pt.y = tp;
	}
	else if (pt.y > _dst_in_frame.y + _dst.rows)
	{
		pt.y = _dst_in_frame.y + _dst.rows;
	}
}

// 根据选择的裁剪比例(_crop_opt)调整点 pt，使宽高比符合要求，且不超出图像边界
void CPaint::proc_crop_pt(Point &pt, Point pt0)
{
	if (_crop_opt == crop_0_0)
	{
		return;
	}

	int rx = 1;
	int ry = 1;
	switch (_crop_opt)
	{
	case crop_1_1:
		rx = 1;
		ry = 1;
		break;
	case crop_3_2:
		rx = 3;
		ry = 2;
		break;
	case crop_4_3:
		rx = 4;
		ry = 3;
		break;
	case crop_5_4:
		rx = 5;
		ry = 4;
		break;
	case crop_7_5:
		rx = 7;
		ry = 5;
		break;
	case crop_16_9:
		rx = 16;
		ry = 9;
		break;
	default:
		break;
	}

	if (abs(pt.x - pt0.x) * ry > abs(pt.y - pt0.y) * rx)
	{
		if (pt.y > pt0.y)
		{
			pt.y = pt0.y + abs(pt.x - pt0.x) * ry / rx;
			if (pt.y > _dst.rows)
			{
				pt.y = _dst.rows;
				if (pt.x > pt0.x)
				{
					pt.x = pt0.x + abs(pt.y - pt0.y) * rx / ry;
				}
				else
				{
					pt.x = pt0.x - abs(pt.y - pt0.y) * rx / ry;
				}
			}
		}
		else
		{
			pt.y = pt0.y - abs(pt.x - pt0.x) * ry / rx;
			if (pt.y < 0)
			{
				pt.y = 0;
				if (pt.x > pt0.x)
				{
					pt.x = pt0.x + abs(pt.y - pt0.y) * rx / ry;
				}
				else
				{
					pt.x = pt0.x - abs(pt.y - pt0.y) * rx / ry;
				}
			}
		}
	}
	else
	{
		if (pt.x > pt0.x)
		{
			pt.x = pt0.x + abs(pt.y - pt0.y) * rx / ry;
			if (pt.x > _dst.cols)
			{
				pt.x = _dst.cols;
				if (pt.y > pt0.y)
				{
					pt.y = pt0.y + abs(pt.x - pt0.x) * ry / rx;
				}
				else
				{
					pt.y = pt0.y - abs(pt.x - pt0.x) * ry / rx;
				}
			}
		}
		else
		{
			pt.x = pt0.x - abs(pt.y - pt0.y) * rx / ry;
			if (pt.x < 0)
			{
				pt.x = 0;
				if (pt.y > pt0.y)
				{
					pt.y = pt0.y + abs(pt.x - pt0.x) * ry / rx;
				}
				else
				{
					pt.y = pt0.y - abs(pt.x - pt0.x) * ry / rx;
				}
			}
		}
	}
}

// 将目标区域 dst_rect 转换为 lena 图像坐标并根据裁剪选项修正大小，返回是否有效
bool CPaint::crop_rt(Rect &rc)
{
	Rect rt = rc;
	rt.x = _dst_in_lena.x + rt.x * _lena_w / _dst_zoom_w;
	rt.y = _dst_in_lena.y + rt.y * _lena_w / _dst_zoom_w;
	rt.width = rt.width * _lena_w / _dst_zoom_w;
	rt.height = rt.height * _lena_w / _dst_zoom_w;


	Point pt0 = Point(rt.x, rt.y);
	Point pt1 = Point(pt0.x + rt.width, pt0.y + rt.height);

	int x0 = max(min(pt0.x, pt1.x), 0);
	int y0 = max(min(pt0.y, pt1.y), 0);
	int x1 = min(max(pt0.x, pt1.x), _lena_w);
	int y1 = min(max(pt0.y, pt1.y), _lena_h);
	Rect rc0(x0, y0, x1 - x0, y1 - y0);

	if (rc0.x < 0 || rc0.y < 0 || rc0.width <= 0 || rc0.height <= 0 ||
		rc0.x + rc0.width >_weightedImage.cols || rc0.y + rc0.height >_weightedImage.rows)
	{
		return false;
	}

	switch (_crop_opt)
	{
	case crop_1_1:
		rc0.width = min(rc0.width, rc0.height);
		rc0.height = rc0.width;
		break;
	case crop_3_2:
		rc0.width = rc0.height * 3 / 2;
		break;
	case crop_4_3:
		rc0.width = rc0.height * 4 / 3;
		break;
	case crop_5_4:
		rc0.width = rc0.height * 5 / 4;
		break;
	case crop_7_5:
		rc0.width = rc0.height * 7 / 5;
		break;
	case crop_16_9:
		rc0.width = rc0.height * 16 / 9;
		break;
	default:
		break;
	}

	if (rc0.x + rc0.width > _weightedImage.cols)
	{
		rc0.x = _weightedImage.cols - rc0.width;
	}
	if (rc0.x < 0)
	{
		rc0.x = 0;
	}

	rc = rc0;
	return true;
}

// 判断点是否位于矩形右下角的一个 48x48 的区域（用于判断是否在裁剪把手处）
bool CPaint::is_crop50(Point pt, Rect rc)
{
	if (pt.x > rc.x + rc.width - 48 &&
		pt.x < rc.x + rc.width - 2 &&
		pt.y > rc.y + rc.height - 48 &&
		pt.y < rc.y + rc.height - 2
		)
	{
		return true;
	}
	return false;
}

// 判断点 pt 是否在给定矩形 rc 的外部（带偏移 off）
bool CPaint::is_out_rect(Rect rc, Point pt, int off)
{
	if (pt.x > rc.x - off &&
		pt.x < rc.x + rc.width + off &&
		pt.y > rc.y - off &&
		pt.y < rc.y + rc.height + off
		)
	{
		return false;
	}
	return true;
}

// 判断点 pt 是否在矩形 rc 内
bool CPaint::at_rect(Point pt, Rect rc)
{
	if (pt.x > rc.x &&
		pt.x < rc.x + rc.width &&
		pt.y > rc.y &&
		pt.y < rc.y + rc.height
		)
	{
		return true;
	}
	return false;
}

// 判断坐标 (x,y) 是否在目标显示区域内（排除工具栏等 UI 区域）
bool CPaint::is_area_dst(int x, int y)
{
	int off = 0;

	if (x > _dst_in_frame.x &&
		x < _dst_in_frame.x + _dst.cols  &&
		y > _dst_in_frame.y &&
		y < _dst_in_frame.y + _dst.rows &&
		x > TOOLBAR_LEFT_WIDTH + off &&
		y > TOOLBAR_TOP_HEIGHT)
	{
		return true;
	}

	return false;
}

// 判断坐标 (x,y) 是否位于输入/导航控件区域（用于导航点击检测）
bool CPaint::is_area_inp(int x, int y)
{

	if (!_inpaint_click && _down_nav_click)
	{
		//int lf = TOOLBAR_BTN_LEFT;

		//if (_app_type == FROM_EXE)
		//{
		//	lf += TOOLBAR_BTN_STEP_X;
		//	//lf += 15;
		//	lf += 3;
		//	lf += 21;

		//}
		int top3 = TOOLBAR_TOP_HEIGHT;// +_off_top;
		int step = MENU_BTN_STEP;

		if (x > _lf_nav && x < _lf_nav + _combo_width_nav && y > top3 && y < top3 + step * 5)
		{
			return true;
		}
	}
	return false;
}

// 帮助函数：在拖动过程中计算并返回相对目标坐标（考虑 _pt_down 与 _dst_in_frame）
bool CPaint::vvv_help(bool b, int x, int y, Point &pt)
{
	if (_pt_down.x != 0 && _pt_down.y != 0)
	{
		int x0 = x;
		int y0 = y;
		Point pt2(x0, y0);
		if (!b)
		{
			reset_pt(pt2, _dst_in_frame.x, _dst_in_frame.y);
		}
		pt2.x -= _dst_in_frame.x;
		pt2.y -= _dst_in_frame.y;

		x0 = pt2.x;
		y0 = pt2.y;

		pt = Point(x0, y0);
		return true;
	}
	return false;
}

// 封装 vvv_help：维护 _pt_down 和 _last_out_pt 的状态，然后调用 vvv_help
bool CPaint::vvv(bool b, int x, int y, Point &pt)
{
	if (b)
	{
		if (_pt_down.x == 0 && _pt_down.y == 0)
		{
			reset_pt(_last_out_pt, _dst_in_frame.x, _dst_in_frame.y);
			_pt_down.x = _last_out_pt.x;
			_pt_down.y = _last_out_pt.y;
		}
	}
	else
	{
		_last_out_pt.x = x;
		_last_out_pt.y = y;
	}

	return vvv_help(b, x, y, pt);
}

// 计算鼠标在逻辑图像坐标中的点：可选择是否以 lena 坐标系为基准
Point CPaint::get_mouse_pt(bool b, bool islena, int x, int y)
{
	int x0 = x;
	int y0 = y;
	Point pt2(x0, y0);
	if (!b)
	{
		reset_pt(pt2, TOOLBAR_LEFT_WIDTH, TOOLBAR_TOP_HEIGHT);
	}

	Point pt;
	if (islena)
	{
		pt2.x -= _lena_in_frame.x;
		pt2.y -= _lena_in_frame.y;

		if (pt2.x < 0)
		{
			pt2.x = 0;
		}
		if (pt2.y < 0)
		{
			pt2.y = 0;
		}

		x0 = pt2.x;
		y0 = pt2.y;
		pt = get_lena_pt(x0, y0);
	}
	else
	{
		pt2.x -= _dst_in_frame.x;
		pt2.y -= _dst_in_frame.y;

		if (pt2.x < 0)
		{
			pt2.x = 0;
		}
		if (pt2.y < 0)
		{
			pt2.y = 0;
		}

		x0 = pt2.x;
		y0 = pt2.y;
		pt = Point(x0, y0);
	}

	return pt;
}

// 在点容器为空且开始绘制时，初始化第一个点（考虑 lena 与 dst 坐标系差异）
void CPaint::pts_empty(bool b, bool islena, int x, int y)
{
	if (b)
	{
		if (_pts.empty())
		{
			reset_pt(_last_out_pt, TOOLBAR_LEFT_WIDTH, TOOLBAR_TOP_HEIGHT);

			Point pt3;
			if (islena)
			{
				_last_out_pt.x -= _lena_in_frame.x;
				_last_out_pt.y -= _lena_in_frame.y;

				pt3 = get_lena_pt(_last_out_pt.x, _last_out_pt.y);
				_prePoint = pt3;
			}
			else
			{
				pt3 = Point(_last_out_pt.x - _lena_in_frame.x, _last_out_pt.y - _lena_in_frame.y);
			}

			if (pt3.x < 0) pt3.x = 0;
			if (pt3.y < 0) pt3.y = 0;

			_pts.push_back(pt3);
		}
	}
	else
	{
		_last_out_pt.x = x;
		_last_out_pt.y = y;
	}
}

// 将目标显示区域 dst_rc 转换为 Lena 图像坐标的矩形 rc，并裁剪到图像边界
void CPaint::dst_rc_2_lena_rc(Rect dst_rc, Rect& rc)
{
	rc.x = _dst_in_lena.x + dst_rc.x * _lena_w / _dst_zoom_w;
	rc.y = _dst_in_lena.y + dst_rc.y * _lena_w / _dst_zoom_w;
	rc.width = dst_rc.width * _lena_w / _dst_zoom_w;
	rc.height = dst_rc.height * _lena_w / _dst_zoom_w;

	if (rc.x + rc.width > _weightedImage.cols)
	{
		rc.width = _weightedImage.cols - rc.x;
	}
	if (rc.y + rc.height > _weightedImage.rows)
	{
		rc.height = _weightedImage.rows - rc.y;
	}

}

// 移动目标区域 target_rc（基于 pt - pre 的偏移），并确保不超出窗口/工具栏限制
bool CPaint::proc_target_rc(Rect &target_rc, int offset, Point pt, Point pre)
{
	int xx = target_rc.x + pt.x - pre.x;
	int yy = target_rc.y + pt.y - pre.y;

	target_rc.x = xx;
	target_rc.y = yy;

	bool b = false;

	if (xx + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH - target_rc.width + offset)
	{
		target_rc.x = -_dst_in_frame.x + TOOLBAR_LEFT_WIDTH - target_rc.width + offset;
		b = true;
	}
	if (xx + _dst_in_frame.x + target_rc.width > _win_w + target_rc.width - offset)
	{
		target_rc.x = _win_w - target_rc.width - _dst_in_frame.x + target_rc.width - offset;
		b = true;
	}

	if (yy + _dst_in_frame.y < TOOLBAR_TOP_HEIGHT - target_rc.height + offset)
	{
		target_rc.y = -_dst_in_frame.y + TOOLBAR_TOP_HEIGHT - target_rc.height + offset;
		b = true;
	}
	if (yy + _dst_in_frame.y + target_rc.height > _win_h + target_rc.height - offset)
	{
		target_rc.y = _win_h - target_rc.height - _dst_in_frame.y + target_rc.height - offset;
		b = true;
	}
	return b;
}

// 根据当前目标框位置和窗体边界调整裁剪区域偏移，防止裁剪框超出可视区域
bool CPaint::proc_crop_offset(Rect &target_rc, Point pt)
{
	int xx = target_rc.x;
	int yy = target_rc.y;

	bool b = false;

	if (_dst_in_frame.x >= TOOLBAR_LEFT_WIDTH)
	{
		if (xx + _dst_in_frame.x < TOOLBAR_LEFT_WIDTH)
		{
			target_rc.x = 10 + TOOLBAR_LEFT_WIDTH - _dst_in_frame.x;
			b = true;
		}
		if (xx + target_rc.width + _dst_in_frame.x > _win_w)
		{
			target_rc.x = _win_w - _dst_in_frame.x - target_rc.width - 10;
			b = true;
		}
	}

	if (_dst_in_frame.y >= TOOLBAR_TOP_HEIGHT)
	{
		if (yy + _dst_in_frame.y < TOOLBAR_TOP_HEIGHT)
		{
			target_rc.y = 10 + TOOLBAR_TOP_HEIGHT - _dst_in_frame.y;
			b = true;
		}
		if (yy + target_rc.height + _dst_in_frame.y > _win_h)
		{
			target_rc.y = _win_h - _dst_in_frame.y - target_rc.height - 10;
			b = true;
		}
	}

	return b;
}
