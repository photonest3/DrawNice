///////////////////////////////////////////////////////////////////////
/// @file paint_ot.cpp
/// @brief 绘图模块 - 输入工具初始化实现
/// @details 实现输入工具(画笔、自由选区、矩形、模糊选择等)
///          SVG 资源加载、按钮状态初始化和样式配置
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件处理绘图输入工具的 UI 资源准备和初始化
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "unitil2.h"
#include "cvui.h"
#include "resource.h"
#include <windows.h>
#include "unitil3.h"
extern HINSTANCE _his;

void CPaint::change_width(Mat temp, Mat &src, Scalar color, int x)
{
	temp = color;
	src.copyTo(temp(Rect(x, (temp.rows - src.rows) / 2, src.cols, src.rows)));
	src = temp.clone();
}

void CPaint::init_inp(int cx, int w, int h)
{
	set_svg(w, h, IDR_SVG_INP_BRUSH, inp_brush_idle);
	make_gray(inp_brush_idle, inp_brush_over, inp_brush_gray);

	set_svg(w, h, IDR_SVG_INP_FREE_SELECT, inp_select_idle);
	make_gray(inp_select_idle, inp_select_over, inp_select_gray);

	set_svg(w, h, IDR_SVG_INP_RECT, inp_rect_idle);
	make_gray(inp_rect_idle, inp_rect_over, inp_rect_gray);

	set_svg(w, h, IDR_SVG_INP_FUZZY, inp_magic_idle);
	make_gray(inp_magic_idle, inp_magic_over, inp_magic_gray);

	set_svg(w, h, IDR_SVG_INP_ERASER, inp_eraser_idle);
	make_gray(inp_eraser_idle, inp_eraser_over, inp_eraser_gray);

	//Scalar(21, 206, 112)
	set_svg(w, h, IDR_SVG_INP_OPTIMIZE, inp_optimize_idle);
	make_gray(inp_optimize_idle, inp_optimize_over, inp_optimize_gray);

	load_bitmap(_his, inp_sample, IDB_INP_SAMPLE);
	Mat lbl = get_lbl(_u2w(_lang.trans("Remove unnecessary Objects")).c_str(), Scalar::all(43), Scalar::all(128), 10);
	lbl.copyTo(inp_sample(Rect(0, inp_sample.rows - 32, lbl.cols, lbl.rows)));
}

void CPaint::init_seg(int cx, int w, int h)
{
	double alphaReserve = 0.5;
	//seg_sample = get_lbl(_u2w(_lang.trans("Select a rectangular area around the object you want to segment")).c_str());
	load_bitmap(_his, seg_sample, IDB_SEG_SAMPLE);
	Mat lbl = get_lbl(_u2w(_lang.trans("Background Matting")).c_str(), Scalar::all(43), Scalar::all(128), 10);
	lbl.copyTo(seg_sample(Rect(0, seg_sample.rows - 32, lbl.cols, lbl.rows)));

	set_svg(w, h, IDR_SVG_GC_FGD_IDLE, gc_fgd_idle);
	make_over(gc_fgd_idle, gc_fgd_over);
	//set_svg(w, h, IDR_SVG_GC_FGD_OVER, gc_fgd_over);
	set_svg(w, h, IDR_SVG_GC_FGD_GRAY, gc_fgd_gray);

	set_svg(w, h, IDR_SVG_GC_BGD_IDLE, gc_bgd_idle);
	make_over(gc_bgd_idle, gc_bgd_over);
	set_svg(w, h, IDR_SVG_GC_BGD_GRAY, gc_bgd_gray);

	set_svg(w, h, IDR_SVG_BG_FILE, select_bg_idle);
	make_gray(select_bg_idle, select_bg_over, select_bg_gray);

	set_svg(w, h, IDR_SVG_INP_RECT, seg_rect_idle);
	make_gray(seg_rect_idle, seg_rect_over, seg_rect_gray);

	set_svg(w, h, IDR_SVG_SEG_START, seg_start_idle);
	make_gray(seg_start_idle, seg_start_over, seg_start_gray);

	lbl_fusion = get_lbl(_u2w(_lang.trans("Complete image fusion")).c_str());

}

void CPaint::init_sticker(int cx, int w, int h)
{
	//load_bitmap(normal_clone_idle, IDB_ITEM);

	//mixed_clone_idle = normal_clone_idle.clone();
	//monochrome_idle = normal_clone_idle.clone();
	//paste_only_idle = normal_clone_idle.clone();
	int rows = input_trans_idle.rows;

	draw_ico_button(_lang.trans("Normal clone").c_str(), normal_clone_idle, rows);
	_combo_width_sticker_2 = normal_clone_idle.cols;

	draw_ico_button(_lang.trans("Mixed clone").c_str(), mixed_clone_idle, rows);
	_combo_width_sticker_2 = max(_combo_width_sticker_2, mixed_clone_idle.cols);

	draw_ico_button(_lang.trans("Monochrome transfer").c_str(), monochrome_idle, rows);
	_combo_width_sticker_2 = max(_combo_width_sticker_2, monochrome_idle.cols);

	draw_ico_button(_lang.trans("Paste").c_str(), paste_only_idle, rows);
	_combo_width_sticker_2 = max(_combo_width_sticker_2, paste_only_idle.cols);
	_combo_width_sticker_2 += 16;

	int off = 12;
	Mat temp = Mat::zeros(Size(_combo_width_sticker_2, paste_only_idle.rows), CV_8UC3);
	change_width(temp, normal_clone_idle, Scalar::all(43), off);
	change_width(temp, mixed_clone_idle, Scalar::all(43), off);
	change_width(temp, monochrome_idle, Scalar::all(43), off);
	change_width(temp, paste_only_idle, Scalar::all(43), off);

	make_over(normal_clone_idle, normal_clone_over);
	make_over(mixed_clone_idle, mixed_clone_over);
	make_over(monochrome_idle, monochrome_over);
	make_over(paste_only_idle, paste_only_over);

	set_svg(w, h, IDR_SVG_ADD_FILE, select_sticker_idle);

	make_gray(select_sticker_idle, select_sticker_over, select_sticker_gray);

	load_bitmap(_his, sticker_sample, IDB_STICKER_SAMPLE);
	Mat lbl = get_lbl(_u2w(_lang.trans("Image Stitching")).c_str(), Scalar::all(43), Scalar::all(128), 10);
	lbl.copyTo(sticker_sample(Rect(0, sticker_sample.rows - 32, lbl.cols, lbl.rows)));
}

void CPaint::init_filter(int cx, int w, int h)
{
	lbl_brightness = get_lbl(_u2w(_lang.trans("Brightness")).c_str());
	_combo_width_filter = lbl_brightness.cols;

	lbl_decolor = get_lbl(_u2w(_lang.trans("Decolor")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_decolor.cols);

	lbl_comic_strip = get_lbl(_u2w(_lang.trans("Comic strip")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_comic_strip.cols);

	lbl_contrast = get_lbl(_u2w(_lang.trans("Contrast")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_contrast.cols);

	lbl_vintage = get_lbl(_u2w(_lang.trans("Nostalgic")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_vintage.cols);

	lbl_feather = get_lbl(_u2w(_lang.trans("Feather")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_feather.cols);


	set_svg(w, h, IDR_SVG_DRAW_SKY, sky_draw_idle);
	draw_ico_button3(_lang.trans("Change...").c_str(), sky_draw_idle);
	make_over(sky_draw_idle, sky_draw_over);
	_combo_width_filter = max(_combo_width_filter, sky_draw_idle.cols);


	lbl_sketch = get_lbl(_u2w(_lang.trans("Sketch")).c_str());
	_combo_width_filter = max(_combo_width_filter, lbl_sketch.cols);
	_combo_width_filter = max(140, _combo_width_filter + 50);
}

void CPaint::init_more(int cx, int w, int h)
{
	set_svg(w, h, IDR_SVG_CROP, crop_custom_idle);
	make_over(crop_custom_idle, crop_custom_over);

	load_bitmap(_his, crop1_1, cx == 32 ? IDB_CROP_11_14 : IDB_CROP_11_18);
	make_over(crop1_1, crop1_1_over);

	load_bitmap(_his, crop3_2, cx == 32 ? IDB_CROP_32_14 : IDB_CROP_32_18);
	make_over(crop3_2, crop3_2_over);

	load_bitmap(_his, crop4_3, cx == 32 ? IDB_CROP_43_14 : IDB_CROP_43_18);
	make_over(crop4_3, crop4_3_over);

	load_bitmap(_his, crop5_4, cx == 32 ? IDB_CROP_54_14 : IDB_CROP_54_18);
	make_over(crop5_4, crop5_4_over);

	load_bitmap(_his, crop7_5, cx == 32 ? IDB_CROP_75_14 : IDB_CROP_75_18);
	make_over(crop7_5, crop7_5_over);

	load_bitmap(_his, crop16_9, cx == 32 ? IDB_CROP_169_14 : IDB_CROP_169_18);
	make_over(crop16_9, crop16_9_over);

	set_svg(w, h, IDR_SVG_ROTA90, rota90_idle);
	make_over(rota90_idle, rota90_over);

	set_svg(w, h, IDR_SVG_ROTA270, rota270_idle);
	make_over(rota270_idle, rota270_over);

	set_svg(w, h, IDR_SVG_ROTA180, rota180_idle);
	make_over(rota180_idle, rota180_over);

	set_svg(w, h, IDR_SVG_FLIPV, flipv_idle);
	make_over(flipv_idle, flipv_over);

	set_svg(w, h, IDR_SVG_FLIPH, fliph_idle);
	make_over(fliph_idle, fliph_over);
}

void CPaint::init_draw(int cx, int w, int h)
{
	set_svg(w, h, IDR_SVG_DRAW_TEXT, text_draw_idle);
	make_over(text_draw_idle, text_draw_over);

	set_svg(w, h, IDR_SVG_FONT, font_idle);
	make_gray(font_idle, font_over, font_gray);

	set_svg(w, h, IDR_SVG_DRAW_FREE, free_draw_idle);
	make_over(free_draw_idle, free_draw_over);
	_combo_width_draw = free_draw_idle.cols;

	set_svg(w, h, IDR_SVG_DRAW_PICKER, picker_draw_idle);
	make_over(picker_draw_idle, picker_draw_over);

	set_svg(w, h, IDR_SVG_DRAW_FILL, fill_draw_idle);
	make_over(fill_draw_idle, fill_draw_over);

	set_svg(w, h, IDR_SVG_DRAW_LINE, line_draw_idle);
	make_over(line_draw_idle, line_draw_over);
	_combo_width_draw = max(_combo_width_draw, line_draw_idle.cols);

	set_svg(w, h, IDR_SVG_DRAW_COLOR, color_draw_idle);
	make_over(color_draw_idle, color_draw_over);
	_combo_width_draw = max(_combo_width_draw, color_draw_idle.cols);

	set_svg(w, h, IDR_SVG_DRAW_ARROW, arrow_draw_idle);
	make_over(arrow_draw_idle, arrow_draw_over);
	_combo_width_draw = max(_combo_width_draw, arrow_draw_idle.cols);

	set_svg(w, h, IDR_SVG_DRAW_RECT, rect_draw_idle);
	make_over(rect_draw_idle, rect_draw_over);
	_combo_width_draw = max(_combo_width_draw, rect_draw_idle.cols);

	set_svg(w, h, IDR_SVG_DRAW_MOSAIC, mosaic_draw_idle);
	make_over(mosaic_draw_idle, mosaic_draw_over);
	_combo_width_draw = max(_combo_width_draw, mosaic_draw_idle.cols);

	set_svg(w, h, IDR_SVG_DRAW_CIRCLE, circle_draw_idle);
	make_over(circle_draw_idle, circle_draw_over);
	_combo_width_draw = max(_combo_width_draw, circle_draw_idle.cols);

	set_svg(w, h, IDR_SVG_INP_ERASER, draw_eraser_idle);
	make_gray(draw_eraser_idle, draw_eraser_over, draw_eraser_gray);

	set_svg(w, h, IDR_SVG_DRAW_SELECT, draw_select_idle);
	make_gray(draw_select_idle, draw_select_over, draw_select_gray);

	_combo_width_draw = _combo_width_draw * 2 + 16;
	_combo_width_inp = max(done_inp_idle.cols + 5, _combo_width_draw);
	_combo_width_seg = max(done_idle.cols + 5, _combo_width_draw);
	_combo_width_sticker = max(done_idle.cols + 5, _combo_width_draw);
}
