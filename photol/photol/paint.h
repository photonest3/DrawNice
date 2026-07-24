///////////////////////////////////////////////////////////////////////
/// @file paint.h
/// @brief PhotoNest 图像绘制模块头文件
/// @details 定义绘制模块的 UI 元素 ID 和常量:
///           - 工具栏按钮 ID (加载、保存、撤销、重做、缩放等)
///           - 导航模式 ID (绘制、分割、贴纸、滤镜、修复)
///           - 绘制工具 ID (画笔、自由选择、魔术棒、橡皮擦、矩形等)
///           - 图像变换 ID (旋转、翻转、裁剪)
///           - 包含 OpenCV 和自定义模块头文件
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 图像绘制模块的核心定义文件
///////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <opencv2/opencv.hpp>
#include "lang2.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

#include <opencv2/quality.hpp>

#include "ThreadPool.h"
#include <algorithm>
#include <functional>
#include "myinpaint.h"
#include "Blowfish.h"


using namespace cv;
using namespace std;

#pragma region define  

#define THICKNESS_VAL 3
#define TOOLBAR_BTN_TOP 3
#define TOOLBAR_SUB_OFF 4
#define TOOLBAR_BTN_LEFT 8
#define MENU_BTN_STEP 36
#define TOOLBAR_BTN_STEP_Y 60
#define TOOLBAR_MEN_STEP_Y 40
#define TOOLBAR_RIGHT_WIDTH 4

#define toolbar_load 1
#define toolbar_save 2
#define toolbar_undo 3
#define toolbar_redo 4
#define toolbar_zoom_in 5
#define toolbar_zoom_out 6
#define toolbar_one 7
#define toolbar_input 8
#define toolbar_open 10
#define toolbar_close 11
#define toolbar_redraw 12
#define toolbar_cancel 13

#define nav_draw 20
#define nav_segment 21
#define nav_sticker 22
#define nav_filter 23
#define nav_inp 24

#define menu_help 34

#define btn_seg_tips 49
#define btn_inp_tips 50
#define btn_brush 51
#define btn_free_select 52
#define btn_magic 53
#define btn_eraser 54
#define btn_rect 55
#define btn_optimize 56
#define btn_inpaint 57
#define btn_nothing 58
#define btn_delete 59
#define btn_filter 60

#define btn_rota270 61
#define btn_rota90 62
#define btn_rota180 63
#define btn_flipv 64
#define btn_fliph 65
#define btn_crop 66

#define btn_crop_bar 67
#define btn_rota_bar 68

#define crop_0_0 71
#define crop_1_1 72
#define crop_4_3 73
#define crop_3_2 74
#define crop_5_4 75
#define crop_7_5 76
#define crop_16_9 77

#define btn_font 78
#define btn_input_bg 79

#define btn_loc 80
#define btn_firstopen 81

#define bth_saveas 83
#define btn_move 85

#define btn_copy 91
#define btn_paste 92
#define btn_sticker 94
#define btn_sticker_tips 95
#define btn_sticker_apply 96
#define btn_seg_fgd 97
#define btn_seg_bgd 98
#define inpaint_ok 100
#define btn_seg_apply 105
#define btn_init 106
#define btn_pasteas 107
#define btn_change_bg 108
#define btn_sticker_ok 109
#define btn_change_bg_ok 110
#define btn_seg_trans 111

#define act_nothing 120
#define act_display 121
#define act_tips_ui 122

#define btn_rota 148

#define btn_draw_free 149
#define btn_draw_text 150
#define btn_draw_picker 151
#define btn_draw_magic 152
#define btn_draw_fill 153
#define btn_draw_straight 155
#define btn_draw_arrow 156
#define btn_draw_rect 157
#define btn_draw_circle 158
#define btn_draw_color 159
#define btn_draw_sky 160

#define btn_draw_eraser 161
#define btn_draw_select 162
#define btn_seg_rect 165
#define btn_draw_trans 167
#define btn_draw_mosaic 168


#define WM_PAINT_CLOSE WM_USER + 105
#define WM_PAINT_RUN WM_USER + 106
#define WM_PAINT_SAVE WM_USER + 107
#define WM_PAINT_SIZE WM_USER + 108
#define WM_PAINT_MASK WM_USER + 111
#define WM_PAINT_INIT WM_USER + 112
#define WM_PAINT_FILE WM_USER + 113

#define WM_EDIT_LBUTTONDOWN WM_USER + 109
#define WM_EDIT_MOUSEMOVE WM_USER + 110

#define FROM_EXE 0
#define FROM_APP_NAV 1
#define FROM_APP_ONE 2

#pragma endregion

#pragma region struct  


typedef struct _z_struct {
	Rect rc;
	Mat dst;
} z_struct;

typedef struct _p_struct {
	Rect rc;
	vector<Point> pts;
	Mat mask;
	Mat image;
} p_struct;

typedef struct _z_filter {
	int contrastValue = 0;
	int brightValue = 0;
	int bkLightValue = 1;

	bool use_contrastValue = false;
	bool use_brightValue = false;
	bool use_decolor = false;
	bool use_sketch = false;
	bool use_comic_strip = false;
	bool use_vintage = false;
	bool use_feather = false;

	bool curr_use_contrastValue = false;
	bool curr_use_brightValue = false;
	bool curr_use_sketch = false;
	bool curr_use_comic_strip = false;
	bool curr_use_decolor = false;
	bool curr_use_vintage = false;
	bool curr_use_feather = false;
} z_filter;


struct z_sticker {
	int id = 0;
	int idx = 0;
	Mat dst;
	Rect rc;

	bool operator < (const z_sticker rhs) const
	{
		return idx < rhs.idx;
	}

	bool operator > (const z_sticker rhs) const
	{
		return idx > rhs.idx;
	}

};

typedef struct _opt_t {
	int opt_type = 0;
	int nav_type = 0;

	vector<Point> pts;
	Point lena_in_frame;
	int thickness = 3;
	Scalar color;
	int loDiff = 0;
	int upDiff = 0;
	z_filter fVal;
	Mat mask;
	Mat bgra;
	Mat image;
	vector<z_sticker> sticker_arr;
	time_t t = 0;
} opt_t;

typedef struct _DE_HEADER
{
	unsigned char ot1;
	unsigned char crc;
	unsigned char offset;
	unsigned char type;
	unsigned short ot2;
	unsigned short ot3;
} DE_HEADER, * PDE_HEADER;

typedef struct _NAV_DTO
{
	wstring id = L"";
	wstring file_name = L"";
	wstring ori_name = L"";
	wstring file_time = L"";
	wstring rotation = L"";
} NAV_DTO, * PNAV_DTO;

struct Inputparama {
	uchar low_h = 78;                     // 识别天空区域hsv颜色的最底H值
	uchar high_h = 124;                   // 识别天空区域hsv颜色的最高H值
	uchar low_s = 0;                      // 识别天空区域hsv颜色的最底S值
	uchar high_s = 255;                   // 识别天空区域hsv颜色的最高S值
	uchar low_v = 78;                     // 识别天空区域hsv颜色的最底V值
	uchar high_v = 255;                   // 识别天空区域hsv颜色的最高V值
	int close_size = 4;                 // 非天空区域闭运算尺寸，该值越大则区域越完整，代价是一些孔洞处没法进行图像更换
	int blur_size = 2;                  // 非天空区域滤波窗口尺寸，该值越大则天空与非天空区衔接处越模糊，适当的数值可以带来较优的融合效果
};

typedef struct _TRAIN_DTO
{
	Mat ori;
	Mat gray;
	string label = "";
	string image_id = "";
	string al = "1";
} TRAIN_DTO;

#pragma endregion

class CPaint
{
public:
	CPaint(void);
	~CPaint(void);
public:
	void init2(int type, int pixelsy, const wchar_t* en, const wchar_t* user, int ov, bool bfirst = true);
	int show(HWND hParent, int w, int h, const wchar_t* fname, const wchar_t* enckey);

private:
	void init_inp(int cx, int w, int h);
	void init_seg(int cx, int w, int h);
	void init_sticker(int cx, int w, int h);
	void init_filter(int cx, int w, int h);
	void init_more(int cx, int w, int h);
	void init_draw(int cx, int w, int h);

#pragma region nav  

	Mat nav_inp_idle;
	Mat nav_inp_over;
	Mat nav_inp_gray;

	Mat nav_segment_idle;
	Mat nav_segment_gray;
	Mat nav_segment_over;

	Mat nav_sticker_idle;
	Mat nav_sticker_gray;
	Mat nav_sticker_over;

	Mat nav_filter_idle;
	Mat nav_filter_gray;
	Mat nav_filter_over;

	Mat nav_draw_idle;
	Mat nav_draw_gray;
	Mat nav_draw_over;


#pragma endregion

#pragma region inp  
	Mat inp_brush_idle;
	Mat inp_brush_over;
	Mat inp_brush_gray;

	Mat inp_select_idle;
	Mat inp_select_over;
	Mat inp_select_gray;

	Mat inp_rect_idle;
	Mat inp_rect_over;
	Mat inp_rect_gray;

	Mat inp_magic_idle;
	Mat inp_magic_over;
	Mat inp_magic_gray;

	Mat inp_eraser_idle;
	Mat inp_eraser_over;
	Mat inp_eraser_gray;

	Mat inp_optimize_idle;
	Mat inp_optimize_over;
	Mat inp_optimize_gray;


	Mat inp_sample;
#pragma endregion

#pragma region seg  
	Mat gc_fgd_idle;
	Mat gc_fgd_over;
	Mat gc_fgd_gray;
	Mat lbl_fusion;

	Mat gc_bgd_idle;
	Mat gc_bgd_over;
	Mat gc_bgd_gray;

	Mat seg_rect_idle;
	Mat seg_rect_over;
	Mat seg_rect_gray;

	Mat select_bg_idle;
	Mat select_bg_over;
	Mat select_bg_gray;

	Mat seg_start_idle;
	Mat seg_start_over;
	Mat seg_start_gray;

	Mat seg_sample;
#pragma endregion

#pragma region sticker  
	Mat select_sticker_idle;
	Mat select_sticker_over;
	Mat select_sticker_gray;

	Mat normal_clone_idle;
	Mat normal_clone_over;

	Mat mixed_clone_idle;
	Mat mixed_clone_over;

	Mat monochrome_idle;
	Mat monochrome_over;

	Mat paste_only_idle;
	Mat paste_only_over;

	Mat sticker_sample;

#pragma endregion

#pragma region more  
	Mat rota90_idle;
	Mat rota90_over;

	Mat rota270_idle;
	Mat rota270_over;

	Mat rota180_idle;
	Mat rota180_over;

	Mat flipv_idle;
	Mat flipv_over;

	Mat fliph_idle;
	Mat fliph_over;

	Mat crop_custom_idle;
	Mat crop_custom_over;

	Mat crop1_1;
	Mat crop1_1_over;
	Mat crop1_1_gray;

	Mat crop4_3;
	Mat crop3_2;
	Mat crop5_4;
	Mat crop7_5;
	Mat crop16_9;

	Mat crop4_3_over;
	Mat crop3_2_over;
	Mat crop5_4_over;
	Mat crop7_5_over;
	Mat crop16_9_over;
#pragma endregion

#pragma region draw  
	Mat picker_draw_idle;
	Mat picker_draw_over;

	Mat draw_select_idle;
	Mat draw_select_over;
	Mat draw_select_gray;

	Mat draw_eraser_idle;
	Mat draw_eraser_over;
	Mat draw_eraser_gray;

	Mat fill_draw_idle;
	Mat fill_draw_over;

	Mat free_draw_idle;
	Mat free_draw_over;

	Mat line_draw_idle;
	Mat line_draw_over;

	Mat arrow_draw_idle;
	Mat arrow_draw_over;

	Mat rect_draw_idle;
	Mat rect_draw_over;

	Mat mosaic_draw_idle;
	Mat mosaic_draw_over;

	Mat circle_draw_idle;
	Mat circle_draw_over;

	Mat color_draw_idle;
	Mat color_draw_over;

	Mat text_draw_idle;
	Mat text_draw_over;

	Mat font_idle;
	Mat font_gray;
	Mat font_over;

	Mat sky_draw_idle;
	Mat sky_draw_over;

#pragma endregion

#pragma region toolbar  
	Mat hand_idle;
	Mat hand_over;
	Mat hand_gray;

	Mat paste_idle;
	Mat paste_over;
	Mat paste_gray;

	Mat copy_idle;
	Mat copy_over;
	Mat copy_gray;

	Mat cut_idle;
	Mat cut_over;
	Mat cut_gray;

	Mat load_idle;
	Mat load_over;
	Mat load_gray;

	Mat down_idle;
	Mat down_over;
	Mat down_gray;

	Mat down2_idle;
	Mat down2_over;
	Mat down2_gray;

	Mat redo_idle;
	Mat redo_over;
	Mat redo_gray;

	Mat undo_idle;
	Mat undo_over;
	Mat undo_gray;

	Mat zoomin_idle;
	Mat zoomin_over;
	Mat zoomin_gray;

	Mat zoomout_idle;
	Mat zoomout_over;
	Mat zoomout_gray;

	Mat prev_idle;
	Mat prev_over;
	Mat prev_gray;

	Mat loc_idle;
	Mat loc_over;


	Mat next_idle;
	Mat next_over;
	Mat next_gray;

	Mat center_idle;
	Mat center_over;
	Mat center_gray;

	Mat func_ico_idle;

	Mat open_ico_idle;
	Mat open_ico_over;
	Mat open_ico_gray;

	Mat save_idle;
	Mat save_over;
	Mat save_gray;

	Mat save_ico_idle;
	Mat save_ico_over;
	Mat save_ico_gray;

	Mat saveas_idle;
	Mat saveas_over;
	Mat saveas_gray;

	Mat saveas_ico_idle;
	Mat saveas_ico_over;
	Mat saveas_ico_gray;

	Mat pasteas_ico_idle;
	Mat pasteas_ico_over;

	Mat original_idle;
	Mat original_over;
	Mat original_gray;

	Mat lbl_drag_idle;
	Mat lbl_drag_over;
	Mat lbl_loading;
	Mat lbl_openerr;

	Mat firstopen_idle;
	Mat firstopen_over;

	Mat expiration_idle;
	Mat expiration_over;

	Mat help_idle;
	Mat help_over;
	Mat help_gray;

	Mat stop_idle;
	Mat stop_over;
	Mat stop_gray;

	Mat home_idle;
	Mat home_over;

	Mat reg_idle;
	Mat reg_over;
	Mat about_idle;
	Mat about_over;
	Mat settings_idle;
	Mat settings_over;

	Mat lbl_tolerance;
	Mat lbl_thickness;
	Mat lbl_seamless;
	Mat lbl_small;

	Mat lbl_ov;

	Mat lbl_brightness;
	Mat lbl_contrast;
	Mat lbl_decolor;
	Mat lbl_comic_strip;
	Mat lbl_vintage;
	Mat lbl_feather;
	Mat lbl_sketch;

	Mat close_idle;
	Mat close_over;
	Mat close_gray;

	Mat input_opaque_model;
	//Mat input_opaque_idle;
	//Mat input_opaque_over;
	//Mat input_opaque_gray;

	Mat input_trans_idle;
	Mat input_trans_over;
	Mat input_trans_gray;

	Mat yes_idle;
	Mat yes_over;

	Mat no_idle;
	Mat no_over;

	Mat yes2_idle;
	Mat yes2_over;

	Mat no2_idle;
	Mat no2_over;

	Mat cancel_idle;
	Mat cancel_over;

	Mat discard_idle;
	Mat discard_over;

	Mat cancel2_idle;
	Mat cancel2_over;

	Mat discard2_idle;
	Mat discard2_over;

	Mat apply_idle;
	Mat apply_over;
	Mat apply_gray;

	Mat done_idle;
	Mat done_over;
	Mat done_gray;

	Mat done_inp_idle;
	Mat done_inp_over;
	Mat done_inp_gray;

	Mat tips_idle;
	Mat tips_over;
	Mat warning_idle;

	Mat info_idle;

#pragma endregion

#pragma region paint_icon 2 
	int _combo_width_sticker_2;
	int _combo_width_filter;
	int _combo_width_draw;
	int _combo_width_inp;
	int _combo_width_seg;
	int _combo_width_sticker;
	int _combo_width_help;

	bool _down_saveas_click;
	bool _down_sticker_click;
	bool _down_cut_click;

#pragma endregion

#pragma region paint_init  
	void load_image(string type0, const wchar_t* fname, bool binit = true);
	void load_ico(int cx);
	bool reset(bool binit = true);
	void load_ini();
	void set_winsize(HWND hWnd, int w, int h);
	void on_resize_cef();

	list<wstring> _lstFile;
	int _idx_file;
	int _len_files;
	int _first_load;
	int _show_err_type;
#pragma endregion

#pragma region paint_ui  
	void draw_ui();
	void show_tips(int& pre_xy, int& cur_xy, int& hit);
#pragma endregion

#pragma region paint_main  
	static void handleSizeDrag(int x, int y, int width, int height, int flags, wchar_t* pfname, char* nav, void* param);
	void show_window(int type, bool& closed);

	bool _exist_coi;
#pragma endregion

#pragma region paint_text  
	bool load_input_wnd(POINT pt, int w, int h);
	void reload_input_wnd();

	void move_input_wnd();
	void set_draw_color();
	void set_input_font();
	void on_init_bg_wnd(HWND hDlg);

	Mat put_input_text(Rect rc0, int left, const wchar_t* str);
	void set_trans_era(Mat dst, Rect rc0, Mat box);
	void get_trans_pos(HWND hDlg, RECT& rc);

	void input_command(int type);

	WNDPROC SetWndProcPtr(HWND hWnd, WNDPROC wndProc);

	static LRESULT CALLBACK input_wnd_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	WNDPROC edit_wndproc_old_;
	static LRESULT CALLBACK EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK bg_wnd_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	HWND _hInputWnd;
	Point _input_wnd_offset;
	LOGFONT _input_lf;

	COLORREF _draw_rgb;
	COLORREF _input_bg_rgb;
	int _input_bg_trans;
	void bg_ok(int trans);
	COLORREF _select_bg_rgb;
	int _drag_rc_type;

	wchar_t _input_buf[2048];
	RECT _win_handle_rw;

	void destory_input();
	void get_rgb(int& r, int& g, int& b);
#pragma endregion

#pragma region paint_clip  

	void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	HBITMAP mat2bitmap(const Mat imgTmp, HDC hDC);
	bool copy_clipboard(Rect& rc);
	void paste_clipboard();
	void paste_ok();
	bool crop_rt(Rect& rc);

	Mat _clip_mat;

#pragma endregion

#pragma region paint_move  
	void get_crop_cursor(Rect rc, UINT& cursor, int off = 20);
	bool mouse_down_input(Rect rc, int& pre_input_box);
	void mouse_down_init(Rect& rc, Point& prev_pt);
	bool mouse_isdown_input(Rect& rc, Point& prev_pt, int off = 20, int off1 = 40);
	bool mouse_isdown_crop(Rect& rc, Point& prev_pt, int off = 20);
	bool mouse_isdown_paste(Rect& rc, Point& prev_pt);
	void paste_ok(Mat& weighted);
	void int_xy(Rect& sr);

#pragma endregion

#pragma region paint_sticker  
	void open_sticker();
	bool mouse_down_sticker();
	bool mouse_isdown_sticker(Rect& rc, Point& prev_pt, int id);
	void sticker_ok(Mat& dst2, Mat& imageROI);
	void sticker_ok();
	void seamless_10(Mat& dst, Rect& roi, Mat src, Point pt, int loDiff = 30, int upDiff = 30, int color = 255);
	Rect get_rc255(Mat mask);

	vector<z_sticker> _sticker_arr;
	int _seamless_mode;
	int _curr_sticker_id;
	Point _pt1_sticker;
	Point _pt2_sticker;
#pragma endregion

#pragma region paint_about  

	static INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK Register(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	//void on_init_settings_wnd(HWND hDlg);
	//void on_init_register_wnd(HWND hDlg);
	//void on_init_about_wnd(HWND hDlg);
	void show_msg(int x, int y, string txt, Scalar backcolor = Scalar(220, 213, 255), Scalar color = Scalar(0xff, 0x22, 0x22));

	bool set_working(int btn, bool state);
	bool set_working2(int btn, bool state, bool falg = true);

	bool clear_opt();
	bool isdo_nothing();

#pragma endregion

#pragma region paint_pt  
	Point get_lena_pt(int x, int y);
	void get_rect1(int thick, int w, int h, int& x0, int& y0, int& x1, int& y1);
	void get_rect2(Point pt, int& x0, int& y0, int& x1, int& y1);
	void reset_pt(Point& pt, int lf, int tp);
	void proc_crop_pt(Point& pt, Point pt0);
	bool is_out_rect(Rect rc, Point pt, int off);
	bool is_crop50(Point pt, Rect rc);
	bool is_area_dst(int x, int y);
	bool is_area_inp(int x, int y);
	bool at_rect(Point pt, Rect rc);

	bool vvv(bool b, int x, int y, Point& pt);
	bool vvv_help(bool b, int x, int y, Point& pt);
	void dst_rc_2_lena_rc(Rect dst_rc, Rect& rc);
	bool proc_target_rc(Rect& target_rc, int offset, Point pt, Point pre);
	bool proc_crop_offset(Rect& target_rc, Point pt);
	void proc_clip_rc(Rect& rc, Rect sr, Mat dst);

#pragma endregion

#pragma region paint_mouse  
	void mouse_down();
	void mouse_isdown();
	void mouse_up();
	void mouse_well_up();
	void mouse_well_down();
	void mouse_down_r();
	void mouse_isdown_r();
	Point get_mouse_pt(bool b, bool islena, int x, int y);
	void pts_empty(bool b, bool islena, int x, int y);

#pragma endregion

#pragma region paint_btn  
	void oneone();
	void undo();
	void redo();
	void zoom(int type);
	void open();
	int save(int type, int& w, int& h, int& q);

	void myflip(int opt);
	int rotate(int angle);
	void crop_ok(Rect& rc, Point& prev_pt);

	static UINT __stdcall handle_inpaint(LPVOID pParam);
	static UINT __stdcall handle_loadimage(LPVOID pParam);
	static UINT __stdcall handle_loadimage2(LPVOID pParam);
	static UINT __stdcall handle_segment(LPVOID pParam);
	static UINT __stdcall handle_sticker(LPVOID pParam);
	static UINT __stdcall handle_input(LPVOID pParam);
	static UINT __stdcall handle_save(LPVOID pParam);

	void loadimage2ok();
	wstring _fname = L"";
	bool _fnameLoaded = false;
	void draw_processing();
	void inpaint_proc();

	void add_opt(opt_t& op, bool bNew = true);
	void set_opt_arr();

	double get_zoom_r(int type);
	int save_coi(int type, Mat src, wchar_t* szCoi, wchar_t* szDst);
	void nav(int type);
	void grabcut();
	void grabcut_apply();
	//void resize_mask(Mat image, Mat &mask, Size size);
	void trans_apply(bool btrans = true);
	double _zoom_r;

	Ptr<quality::QualityBRISQUE> _brisque = nullptr;
	double BRISQUE(Mat img);

#pragma endregion

#pragma region paint_display  
	void reset_frame();
	void get_frame(HWND hWnd, int& left, int& top);
	void filter(Mat src, Mat& dst);
	bool get_display_win(Point pt0);

	void redraw(int idx, int fx = 1);
	void redraw_helper(int idx, int& cur_deg);
	void displayImage(int type, Mat dst);
	void high_displayImage();

#pragma endregion

#pragma region paint_filter  

	z_filter _fVal;
	time_t _last_t;

	Mat RotateImage(Mat& src, double angle, bool isClip);

	Point _last_out_pt;

	void Bright(const Mat src, Mat& dst, z_filter fVal);

	void init_fVal(z_filter& fv);
	void filter_ok(int ty);

	bool _filter_changed;
	bool _filter_mousedown;
	bool _filter_mouseup;

	void saveas(int btn, HWND hWnd);
	int Encryptfile(wstring src, wstring Encryptname);

#pragma endregion

#pragma region paint_utils  
	void make_over(Mat src, Mat& over, int src_color = 43, int over_color = 69);
	void make_gray(Mat src, Mat& over, Mat& gray);
	void make_gray(const char* txt, Mat& src, Mat& over, Mat& gray);
	void make_gray2(Mat& src);

	void drawWeighted(Mat& weighted, Mat beforeROI, Mat afterROI, int BChannel = 0, int GChannel = 0, int RChannel = 255);
	void drawWeighted_picker(Mat& weighted, Rect rect, Mat afterROI, Scalar color);
	void drawWeighted_seg(Mat& weighted, Mat mask);

	void set_dst_by_v3(Mat& dst, Mat src);
	void set_mask_by_v1(Mat& mask, Mat src);
	bool set_mask_by_v3(Mat& mask, Mat src);

	void draw_tip_txt(int x1, int y1, const char* txt, Scalar color = Scalar::all(190), bool bottom = false);
	Mat get_lbl(const wchar_t* txt, Scalar bkColor = Scalar::all(43), Scalar color = Scalar::all(190), int fontSize = 9);
	void draw_txt(int x1, int y1, const char* txt, Scalar color = Scalar::all(190), Scalar bkColor = Scalar::all(43));

	int draw_button(const char* txt, Mat& idle, Mat& over, Mat& gray, Scalar color = Scalar::all(190));
	int draw_button(const char* txt, Mat& idle, Mat& over, Scalar bkColor = Scalar::all(43), Scalar color = Scalar::all(190), int fontSize = 9);
	int draw_ico_button(const char* txt, Mat& btn, int rows = 0, int off = 8);
	int CPaint::draw_ico_button3(const char* txt, Mat& btn);
	int draw_ico_button(const char* txt, Mat& idle, Mat& over, Scalar bkColor = Scalar::all(43), Scalar color = Scalar::all(190), int fontSize = 9);
	int draw_ico_button(Mat temp, Mat& idle, Mat& over);
	int draw_ico_button(const char* txt, Mat& idle, Scalar bk, Scalar color);
	Mat putTextZH(const wchar_t* str, Scalar bkcolor, Scalar color, int fontSize = 9, const wchar_t* fn = L"Arial"); //L"NSimSun Regular");//);

	void make_draw_ico(Mat& ico, Scalar color);
	void make_draw_ico(Mat& ico, int c, Scalar color);
	void praser_file(wstring szline);

	void make_bgra_bg(Mat src, Mat& alpha);

	void WritePrivate(string key, int val);
	void WritePrivate(string key, const char* val);
	void ReadPrivate(string key, string& val);
	//void load_bitmap(Mat& img, UINT resID);
	void drawDashRect(Mat img, int linelength, int dashlength, Point pt1, Point pt2);
	void save_to_file(Mat dst, wstring szDst2);
	long GetFolderPics(wstring strPath, list<wstring >& lstFile);
	bool is_picture(wchar_t* cFileName);

	int resizemode();
	void alpha(Vec3b& v, int BChannel, int GChannel, int RChannel, double alphaReserve);

#pragma endregion

#pragma region paint_license 
	int _ov;
	wstring _usr_lic;
#pragma endregion

#pragma region paint_inp
	bool canInpaint();
	bool show_unapplied(int btn = 0, bool state = false);
	bool is_unapplied(bool canInp);
	bool canSave();
	bool canExit();
	void show_rect(Mat dst2, Rect rc, int opt);
	void show_crop_apply(Mat dst2, Rect rc, Point pt1, int opt);
	void print_bottom_info(int x, wchar_t* buf, int fx = 0);

	void nav_saveas_is_in_combo(int x, int y, bool& cbo);
	void nav_saveas_ui(int lf, bool b0);
	void navbar_state_ui(int x, int y, string mouse);
	void nav_draw_ui();
	void nav_crop_ui();
	void nav_rota_ui();
	void nav_seg_ui();
	void nav_sticker_ui();
	void nav_filter_ui(z_filter& fVal);
	void nav_inp_ui(bool canInp);
	void get_toolbar_rc(Rect& rc);
	int get_toolbar_lf();

	int _sel_nav_btn;
	bool _sel_nav_state;
	void select_btn(int opt);
	void select_btn2(int opt);
	void blank_click();
#pragma endregion

#pragma region application  
	wstring PRODUCT_NAME;
	string WINDOW_NAME;

	int _app_type;

	HWND _win_handle;
	HWND _hParent;
	UINT _currCursor;
	bool _firstrun;
	bool _need_refresh;
	CLang _lang;
	ThreadPool* _pool;

	wchar_t _fn[255];
	NAV_DTO _curr_nav_dto;
	wstring _file_temp;
	wstring _drag_name;
	wstring _cur_name;

	string _nav_data;
	int _show_error_times;
	int _cur_xy;

	Mat _bg;
	Mat _kb_info;
	Mat _size_info;
	Mat _dst_tips;
	Rect _curr_rt;
#pragma endregion

#pragma region paint_lena

	int _win_w;
	int _win_h;
	int _lena_w;
	int _lena_h;
	int _dst_zoom_w;
	int _dst_zoom_h;

	Mat _frame;
	Mat _lena2;
	Mat _lena_bak;

	Mat _inpaintMask;
	Mat _weightedImage;
	Mat _weighted_befoe_input;
	Mat _bgra_mask;
	Mat _eraser_mask;
	Mat _dst;

	int xrect_img;
	int yrect_img;
	Point _lena_in_frame;
	Point _dst_in_frame;
	Rect _dst_in_lena;

	Point _prePoint;
	Point _pt_down;
	Point _pt_all_down;

	vector<Point> _pts;
	vector<opt_t> _opt_arr;

#pragma endregion

#pragma region paint global  
	HDC _hDC;

	int _nav_btn;
	int _opt;
	int _tool_btn;
	int _pointer_btn;
	int _tip_btn;

	int _lastKey;
	bool _space_key_mouse;

	int _last_draw_opt;
	int _last_opt;

	bool _changed;
	int _working;
	int _idx;
	int _mask_idx;
	bool _is_in_combo;

	bool _inpaint_click;
	bool _inpainting;
	bool _has_tips;
	bool _save_click;

	int _save_ret;
	int _disp_nb;
	int _close_ret;
	int _do_nav_btn;
	bool _do_nav_state;

	Mat _seg_mask;
	Mat _seg_bak;
	Mat _seg_bg;
	Mat _result;

	Mat _lbl_inp;
	Mat _lbl_segment;
	Mat _lbl_sticker;
	Mat _lbl_filter;
	Mat _lbl_draw;
	Mat _lbl_nav;

	Mat _left_bar;
	Rect _left_bar_rc;

	Scalar_<int> _picker_val;

	Point _pt_prev;

	Rect _input_win_in_dst;
	Rect _input_win_in_dst_last;

	// in dst rect
	Rect _rect_select;
	Rect _rect_crop;
	Rect _rect_paste;

#pragma endregion

#pragma region other  
	int _opt_rotate;
	int _angleValue;
	int _angleValue_change;
	int _total_deg;

	int _inpaint_thickness;
	int _draw_thickness;
	int _erase_thickness;

	int _loDiff;
	int _upDiff;
	int _connectivity;
	int _newMaskVal;

	int _crop_opt;
	bool _crop_mouseup;

	int _help_btn;

	int _help_click;
	int _show_help;
	int _lf_copy;
	int _lf_undo;
	int _lf_font;
	int _lf_apply;
	int _lf_move;
	int _lf_nav;
	int _lf_saveas_down;
	int _lf_open;
	int _lf_save;

	bool _for_font;
	bool _for_select;

	void change_bg();
	void select_bg_file(int ty, Mat& bg);
	bool erase_lastopt(int opt_type, int nav_type);

	int _input_off;

	long read_svg(HINSTANCE hrcInst, UINT cr_id, string& xml);
	void set_svg(int w, int h, UINT cr_id, Mat& dst, int w0 = 32, int h0 = 32, Scalar color = Scalar::all(43));

	void change_width(Mat temp, Mat& src, Scalar color = Scalar::all(43), int x = 0);
	Point _pt_close;
	bool _closed;
	HANDLE _handle_thread;

	t_struct threads[100];
	int get_thread_idx();
	t_struct* get_thread();

	Rect _rect_seg_select;
	Rect _rect_seg_bak;
	bool _in_rect_seg_select;
	int _process_nb;

	HCURSOR _hcursor_we;
	HCURSOR _hcursor_ns;
	HCURSOR _hcursor_nwse;
	HCURSOR _hcursor_nesw;


	int TOOLBAR_BTN_STEP_X;
	int FOOTER_HEIGHT;
	int TOOLBAR_TOP_HEIGHT;
	int _left_width;
	int _off_top;
	int _pixelsy;

	HBITMAP _hBmpTxt;
	TEXTMETRICW _cw;
	Mat _brushMask;
	Mat _weighted_temp;
	Mat _weighted_mask;
	Mat _weighted_era;

	void paint_eraser(int type, vector<Point> pts, int th);
	void paint_brush(int type, vector<Point> pts, int th);
	void paint_free(int type, vector<Point> pts, int th);
	void paint_mosaic(vector<Point> pts, int th);

	Mat SkySeparation(Mat src, Inputparama input);
	Mat ImageFusion(Mat src1, Mat src2, Mat mask);
	int change_sky();

	Mat _eraser_mask_tmp;

	Mat _eraser_mask_bak;
	Point _lena_in_frame0;
	Rect _brfore_change_bg;
	Rect _change_bg;
	CRITICAL_SECTION _cs;

	void transl2(HWND hDlg, UINT id, int isBtn = 0);
	//int on_register(HWND hDlg);
	//int get_license(string& tit, string& exp);

	wstring _language;
	bool _down_nav_click;
	int _combo_width_nav;
	Size _lenaSize;
	double _lenaf;

	bool _is_sticker_move;
	void proc_paper_rc(Rect& rc, Rect sr, Mat dst);
#pragma endregion
	int _navbar_state;
	int watermark(Mat& wordSrc);

	int _toosmall;
	int TOOLBAR_LEFT_WIDTH = 110;

	wstring _fcoi;
	int _mkb = 0;

	wstring get_extension_dir();
	void sc(wstring path, wstring appDataDir, wstring subfolder);

	void reset_appdata_path(wstring& p);

	CBlowfish _BF;

};