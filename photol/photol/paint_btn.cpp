///////////////////////////////////////////////////////////////////////
/// @file paint_btn.cpp
/// @brief PhotoNest 绘图模块 - 按钮和文件选择功能实现文件
/// @details 实现按钮回调和文件选择功能:
///           - 背景文件选择 (select_bg_file)
///           - 工具栏按钮处理
///           - 文件对话框管理
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 绘图模块的交互功能文件
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "paint.h"
#include "cvui.h"
#include "resource.h"
#include<fstream>
#include "unitil2.h"
#include <windows.h>
#include<commdlg.h>
#include <shlobj.h>
#include "unitil3.h"


void CPaint::select_bg_file(int ty, Mat& bg)
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
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;// | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = L"*";

	//iniDir = get_module_path(NULL) + L"\\extension\\sky";

	wstring iniDir = get_module_path(NULL);
	wstring::size_type pos = iniDir.find(L"\\windowsapps\\photonest");
	if (pos != wstring::npos)
	{
		iniDir = get_extension_dir();//get_appdata_path(alb);
	}

	iniDir += L"\\extension\\sky";

	if (ty == 1)
	{
		string val = "";
		ReadPrivate("paintdir", val);
		iniDir = _u2w(val);
	}

	if (iniDir != L"")
	{
		ofn.lpstrInitialDir = iniDir.c_str();
	}

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
				bg = imdecode(vec_data, IMREAD_UNCHANGED);//IMREAD_COLOR);//IMREAD_UNCHANGED
				int u = bg.depth();
				if (bg.data != NULL && u != 0)
				{
					Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
					temp.convertTo(bg, CV_8U);
				}

				if (bg.data == NULL)
				{
					bg = Mat::zeros(Size(0, 0), CV_8UC3);
				}

				int channels = bg.channels();
				if (channels == 4)
				{
					for (int i = 0; i < bg.rows; i++)
					{
						Vec4b* ptr = bg.ptr<Vec4b>(i);
						for (int j = 0; j < bg.cols; j++)
						{
							Vec4b& v = ptr[j];
							if (v[3] == 0)
							{
								v[0] = 255;
								v[1] = 255;
								v[2] = 255;
							}
						}
					}

					if (ty == 1)
					{
						_eraser_mask = Mat::zeros(Size(0, 0), CV_8UC1);
					}
					cvtColor(bg, bg, CV_BGRA2BGR);
				}
				else
				{
					if (channels == 1)
					{
						vector<Mat> v;
						v.push_back(bg);
						v.push_back(bg);
						v.push_back(bg);
						merge(v, bg);
					}
					else if (channels == 2)
					{
						bg = Mat::zeros(Size(0, 0), CV_8UC3);
					}
					if (ty == 1)
					{
						_eraser_mask = Mat::zeros(Size(0, 0), CV_8UC1);
					}
				}

				if (ty == 1)
				{
					wchar_t* p = wcsrchr(szFileName, L'\\');
					if (p != NULL)
					{
						p[0] = 0;
					}
					WritePrivate("paintdir", _w2u(szFileName).c_str());
				}
			}
		}
	}
}

void CPaint::oneone()
{
	if (_lena2.empty())
	{
		return;
	}

	int w0 = _lena_bak.cols;
	int h0 = _lena_bak.rows;

	if (w0 < _win_w - TOOLBAR_LEFT_WIDTH && h0 < _win_h - TOOLBAR_TOP_HEIGHT)
	{
		_dst_zoom_w = w0;
		_dst_zoom_h = h0;
	}
	else
	{
		double radio1 = (double)w0 / (double)h0;
		double radio2 = (double)(_win_w - TOOLBAR_LEFT_WIDTH) / (double)(_win_h - TOOLBAR_TOP_HEIGHT);
		if (radio1 > radio2)
		{
			_dst_zoom_w = (_win_w - TOOLBAR_LEFT_WIDTH);
			_dst_zoom_h = (int)((double)_dst_zoom_w / radio1);
		}
		else
		{
			_dst_zoom_h = (_win_h - TOOLBAR_TOP_HEIGHT);
			_dst_zoom_w = (int)((double)_dst_zoom_h * radio1);
		}
	}
	xrect_img = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
	yrect_img = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

	_lena_in_frame.x = xrect_img;
	_lena_in_frame.y = yrect_img;
	_lena_in_frame0 = _lena_in_frame;
	_dst_in_frame = Point(_lena_in_frame.x, _lena_in_frame.y);

	_zoom_r = (double)_dst_zoom_w / (double)_lena_w;
}

void CPaint::undo()
{
	if (_lena2.empty())
	{
		if (!_opt_arr.empty() && _opt_arr[0].opt_type == btn_init)
		{
		}
		else
		{
			return;
		}
	}

	if (!_clip_mat.empty())
	{
		paste_ok();
	}

	_idx -= 1;
	if (_idx < 0)
	{
		_idx = 0;
	}
	redraw(_idx, 0);
}

void CPaint::redo()
{
	if (_lena2.empty())
	{
		if (!_opt_arr.empty() && _opt_arr[0].opt_type == btn_init)
		{
		}
		else
		{
			return;
		}
	}

	_idx += 1;
	int len = (int)_opt_arr.size();
	if (_idx > len)
	{
		_idx = len;
	}
	redraw(_idx);
}

void CPaint::zoom(int type)
{
	if (_lena2.empty())
	{
		return;
	}

	if (_hInputWnd != NULL)
	{
		return;
	}
	if (_rect_select.width != 0)
	{
		return;
	}
	if (_rect_crop.width != 0)
	{
		return;
	}
	if (!_sticker_arr.empty())
	{
		return;
	}

	bool b = false;
	if (type == 0)
	{
		if (_zoom_r < 4.0f)
		{
			_zoom_r = get_zoom_r(0);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
			_dst_zoom_w = (int)(_lena_w * _zoom_r);

			_lena_in_frame.x = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
			_lena_in_frame.y = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

			b = true;
		}
	}
	else if (type == 1)
	{
		if (_zoom_r > 0.05f)
		{
			_zoom_r = get_zoom_r(1);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);
			_dst_zoom_w = (int)(_lena_w * _zoom_r);

			_lena_in_frame.x = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
			_lena_in_frame.y = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

			b = true;
		}
	}
	else if (type == 3)
	{
		oneone();
		b = true;
	}

	if (b)
	{
		_crop_mouseup = false;
		_working = act_display;
	}
}

void CPaint::open()
{
	if (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE)
	{
		load_image("", _curr_nav_dto.ori_name.c_str());
	}
	else
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
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;// | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = L"*";

		string val = "";
		ReadPrivate("paintdir", val);

		wstring iniDir = _u2w(val);
		if (iniDir != L"")
		{
			ofn.lpstrInitialDir = iniDir.c_str();
		}
		else
		{
			wchar_t* szDocument;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &szDocument)))
			{
				iniDir = szDocument;
				ofn.lpstrInitialDir = iniDir.c_str();
				CoTaskMemFree(szDocument);
			}
		}

		if (GetOpenFileName(&ofn))
		{
			_curr_nav_dto.file_name = szFileName;
			load_image("first", szFileName);

			wstring s = szFileName + wstring(L" - ") + PRODUCT_NAME;
			HWND pParent = ::GetParent(_win_handle);
			::SetWindowText(pParent, s.c_str());

			wchar_t* p = wcsrchr(szFileName, L'\\');
			if (p != NULL)
			{
				p[0] = 0;
			}
			WritePrivate("paintdir", _w2u(szFileName).c_str());
		}
	}
}

double CPaint::BRISQUE(Mat img)
{
	if (img.cols <= 16 || img.rows <= 16)
	{
		return 0;
	}


	if (_brisque == nullptr)
	{
		wstring root = get_module_path0(NULL);

		// path to the trained model
		wstring model_path = root + L"\\model\\brisque_model_live.yml";
		// path to range file
		wstring range_path = root + L"\\model\\brisque_range_live.yml";

		try {
			_brisque = quality::QualityBRISQUE::create(_w2u(model_path), _w2u(range_path));
		}
		catch (...) {
		}
	}
	if (_brisque == nullptr)
	{
		return 0;
	}

	Scalar result_static = _brisque->compute(img);

	return 100 - calMEAN(result_static);
}

int CPaint::save(int type, int& w, int& h, int& q)
{
	if (_ov == 1)
	{
		return 100;
	}

	Mat dst = _weightedImage.clone();

	wchar_t buf[MAX_PATH] = L"output";
	wstring szFileName = L"output";
	if (type == bth_saveas || _file_temp == _curr_nav_dto.file_name)
	{
		if (_file_temp != _curr_nav_dto.file_name)
		{
			szFileName = _curr_nav_dto.file_name;

			if ((_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE))
			{
				//E:/voy/102/PhotoNest/tests/cefclient/Release/data1/_thumb/galleries_38/IMG_20230706_151056.jpg_coi.png.krf
				wstring::size_type n1 = szFileName.rfind(L"/");
				if (n1 != wstring::npos)
				{
					szFileName = szFileName.substr(n1 + 1);
				}

				wstring::size_type pos = szFileName.find_last_of(L".krf");
				if (pos != wstring::npos)
				{
					szFileName = szFileName.substr(0, pos);
				}

				wstring::size_type  n2 = szFileName.rfind(L"_coi.png");
				if (n2 != wstring::npos)
				{
					szFileName = szFileName.substr(0, n2);
				}

				wstring::size_type n3 = szFileName.find(L".");
				if (n3 != wstring::npos)
				{
					szFileName = szFileName.substr(0, n3);
				}
			}
			else
			{
				wstring::size_type 	n1 = szFileName.rfind(L"\\");
				if (n1 != wstring::npos)
				{
					szFileName = szFileName.substr(n1 + 1);
				}

				wstring::size_type pos = szFileName.find_last_of(L".");
				if (pos != wstring::npos)
				{
					szFileName = szFileName.substr(0, pos);
				}
			}

			memset(buf, 0, sizeof(wchar_t) * MAX_PATH);
			wcscpy_s(buf, MAX_PATH, szFileName.c_str());
		}

		wchar_t szDir[MAX_PATH] = { 0 };

		string val = "";
		ReadPrivate("initialdir", val);
		if (val == "")
		{
			wchar_t* szDocument;
			if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &szDocument)))
			{
				GetShortPathName(szDocument, szDir, _MAX_PATH);
				CoTaskMemFree(szDocument);
			}
		}
		else
		{
			memset(szDir, 0, sizeof(wchar_t) * MAX_PATH);
			wcscpy_s(szDir, MAX_PATH, _u2w(val).c_str());
		}

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
#ifdef OPENFILENAME_SIZE_VERSION_400
		// we are not going to use new fields any way
		ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
#else
		ofn.lStructSize = sizeof(ofn);
#endif
		ofn.hwndOwner = _win_handle;

		if (!_eraser_mask.empty())
		{
			ofn.lpstrFilter = L"PNG files (*.png)\0*.png\0"
				L"JPEG files (*.jpg;*.jpeg;*.jpe)\0*.jpg;*.jpeg;*.jpe\0"
				L"Windows bitmap (*.bmp;*.dib)\0*.bmp;*.dib\0"
				L"WebP files (*.webp)\0*.webp\0"
				L"All Files (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"png";
		}
		else
		{
			ofn.lpstrFilter = L"JPEG files (*.jpg;*.jpeg;*.jpe)\0*.jpg;*.jpeg;*.jpe\0"
				L"PNG files (*.png)\0*.png\0"
				L"Windows bitmap (*.bmp;*.dib)\0*.bmp;*.dib\0"
				L"WebP files (*.webp)\0*.webp\0"
				L"All Files (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"jpg";
		}

		ofn.lpstrFile = buf;
		ofn.lpstrInitialDir = szDir;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN | OFN_NOCHANGEDIR;

		if (GetSaveFileName(&ofn))
		{
			szFileName = buf;

			wstring::size_type 	n1 = szFileName.rfind(L"\\");
			if (n1 != wstring::npos)
			{
				wstring tt = szFileName.substr(0, n1);
				WritePrivate("initialdir", _w2u(tt).c_str());
			}
		}
		else
		{
			DWORD dw = ::CommDlgExtendedError();

			string s = Int2Str(dw);
			return 2;
		}
	}

	if (!_eraser_mask.empty())
	{
		cvtColor(dst, dst, CV_BGR2BGRA);
		vector<int> fromto = { 0, 3 };
		mixChannels(_eraser_mask, dst, fromto);
	}

	if (type != bth_saveas && (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE))
	{
		q = (int)(BRISQUE(dst) * 100);
		if (q == 0)
		{
			q = 1;
		}
	}

	int ret = 0;
	wstring szDst2 = _curr_nav_dto.file_name;
	if (type == bth_saveas || _file_temp == _curr_nav_dto.file_name)
	{
		szDst2 = szFileName.c_str();
	}
	else
	{
		_cur_name = _curr_nav_dto.file_name;
	}

	string ext = ".png";
	wstring::size_type pos = szDst2.find_last_of(L".");
	if (pos != wstring::npos)
	{
		wstring ext2 = szDst2.substr(pos);

		if (ext2 == L".png" || ext2 == L".jpeg" || ext2 == L".jpg" ||
			ext2 == L".jpe" || ext2 == L".bmp" || ext2 == L".dib" || ext2 == L".webp")
		{
			ext = my_tolower(_w2u(ext2));
		}
	}

	if (!_eraser_mask.empty())
	{
		if (ext != ".png" && ext != ".webp")
		{
			cvtColor(dst, dst, CV_BGRA2BGR);
			for (int i = 0; i < _eraser_mask.rows; ++i)
			{
				for (int j = 0; j < _eraser_mask.cols; ++j)
				{
					if (_eraser_mask.at<uchar>(i, j) == 0)
					{
						dst.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
					}
				}
			}
		}
	}

	if (_lenaf > 1.0)
	{
		resize(dst, dst, Size(_lenaSize.width, _lenaSize.height), 0, 0, resizemode());
	}
	w = dst.cols;
	h = dst.rows;

	vector<int> ql;
	if (ext == ".png")
	{
		ql.push_back(IMWRITE_PNG_COMPRESSION);
		ql.push_back(3);
	}
	else if (ext == ".webp")
	{
	}
	else
	{
		ql.push_back(IMWRITE_JPEG_QUALITY);
		ql.push_back(70);
	}

	if (type == bth_saveas)
	{
		vector<uchar> buf0;
		imencode(ext.c_str(), dst, buf0, ql);

		ofstream file2(szDst2.c_str(), ios::out | ios::binary);
		if (!file2)
		{
			return -1;
		}
		file2.write((char*)&buf0[0], buf0.size() * sizeof(uchar));
		file2.close();
	}
	else
	{
		if (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE)
		{
			int type0 = 0;
			wstring coi = _curr_nav_dto.ori_name;
			wstring::size_type  pos = coi.find(L".krf");
			if (pos != wstring::npos)
			{
				type0 = 1;
				coi = coi.substr(0, pos);
				coi += L"_coi.png";
				_cur_name = coi + L".krf";
			}
			else
			{
				coi += L"_coi.png";
				_cur_name = coi;
			}
			ret = save_coi(type0, dst, (wchar_t*)coi.c_str(), (wchar_t*)_curr_nav_dto.file_name.c_str());

		}
		else if (_app_type == FROM_EXE)
		{
			vector<uchar> buf0;
			imencode(ext.c_str(), dst, buf0, ql);

			ofstream file2(szDst2.c_str(), ios::out | ios::binary);
			if (!file2)
			{
				return -1;
			}
			file2.write((char*)&buf0[0], buf0.size() * sizeof(uchar));
			file2.close();
		}
	}

	time_t t = 0;
	if (!_opt_arr.empty())
	{
		for (int i = 0; i < _idx; i++)
		{
			t = _opt_arr[i].t;
		}
	}

	_last_t = t;

	return ret;
}

void CPaint::myflip(int opt)
{
	opt_t op;
	op.opt_type = opt;
	op.nav_type = 0;
	op.thickness = THICKNESS_VAL;

	int type = 0;
	if (opt == btn_fliph)
	{
		type = 1;
	}

	flip(_weightedImage, _weightedImage, type);
	flip(_inpaintMask, _inpaintMask, type);
	flip(_lena_bak, _lena_bak, type);

	if (!_eraser_mask.empty())
	{
		flip(_eraser_mask, _eraser_mask, type);
	}

	add_opt(op);
	_pts.clear();
}

int CPaint::rotate(int angle)
{
	if (_lena2.empty())
	{
		return 0;
	}

	div_t di = div(angle, 360);
	if (di.rem == 0)
	{
		return 0;
	}

	di = div(angle, 180);
	if (di.rem == 0)
	{
		angle = 180;
	}
	else
	{
		if (angle < 0)
		{
			angle = -90;
		}
		else
		{
			angle = 90;
		}
	}

	int len = 3;
	if (!_eraser_mask.empty())
	{
		len = 4;
	}

	vector< future<z_struct> > results;
	for (int j = 0; j < len; j++)
	{
		results.emplace_back(_pool->enqueue([angle, j, this] {
			Mat s;
			if (j == 0)
			{
				s = RotateImage(_lena_bak, angle, true);
			}
			else if (j == 1)
			{
				s = RotateImage(_weightedImage, angle, true);
			}
			else if (j == 2)
			{
				s = RotateImage(_inpaintMask, angle, true);
			}
			else
			{
				if (!_eraser_mask.empty())
				{
					s = RotateImage(_eraser_mask, angle, true);
				}
			}

			z_struct v;
			v.dst = s;
			v.rc.x = j;
			return v;
			})
		);
	}

	for (auto&& result : results)
	{
		z_struct v = result.get();
		if (v.rc.x == 0)
		{
			_lena_bak = v.dst;
		}
		else if (v.rc.x == 1)
		{
			_weightedImage = v.dst;
		}
		else if (v.rc.x == 2)
		{
			_inpaintMask = v.dst;
		}
		else
		{
			if (!_eraser_mask.empty())
			{
				_eraser_mask = v.dst;
			}
		}
	}

	_lena_w = _weightedImage.cols;
	_lena_h = _weightedImage.rows;

	_dst_zoom_w = (int)(_lena_w * _zoom_r);
	_dst_zoom_h = (int)(_lena_h * _zoom_r);

	return 1;
}

void CPaint::crop_ok(Rect& rc, Point& prev_pt)
{
	if (_lena2.empty())
	{
		return;
	}

	if (rc.width > 0 && rc.height > 0)
	{
		Rect rt = rc;
		bool b = crop_rt(rt);
		if (b)
		{
			_weightedImage = _weightedImage(rt);
			_inpaintMask = _inpaintMask(rt);
			_lena_bak = _lena_bak(rt);

			if (!_eraser_mask.empty())
			{
				_eraser_mask = _eraser_mask(rt);
			}

			_lena_w = _weightedImage.cols;
			_lena_h = _weightedImage.rows;
			_dst_zoom_w = (int)(_lena_w * _zoom_r);
			_dst_zoom_h = (int)(_lena_h * _zoom_r);

			xrect_img = TOOLBAR_LEFT_WIDTH + (_win_w - _dst_zoom_w - TOOLBAR_LEFT_WIDTH) / 2;
			yrect_img = TOOLBAR_TOP_HEIGHT + (_win_h - _dst_zoom_h - TOOLBAR_TOP_HEIGHT) / 2;

			_lena_in_frame.x = xrect_img;
			_lena_in_frame.y = yrect_img;

			opt_t op;
			op.opt_type = _opt;
			op.nav_type = 0;
			op.pts.push_back(Point(rt.x, rt.y));
			op.pts.push_back(Point(rt.x + rt.width, rt.y + rt.height));
			op.thickness = THICKNESS_VAL;
			add_opt(op);
		}
		_pointer_btn = btn_move;
		_working = act_display;

		_pts.clear();

		rc = Rect(0, 0, 0, 0);
		prev_pt = Point(0, 0);

		_crop_mouseup = false;
	}
}

UINT __stdcall CPaint::handle_inpaint(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;
	pThis->inpaint_proc();
	return 0;
}

UINT __stdcall CPaint::handle_segment(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;
	pThis->grabcut();
	return 0;
}

UINT __stdcall CPaint::handle_sticker(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;
	pThis->sticker_ok();
	return 0;
}

UINT __stdcall CPaint::handle_input(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;
	pThis->destory_input();
	return 0;
}

UINT __stdcall CPaint::handle_save(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;
	pThis->_tool_btn = toolbar_save;
	pThis->_working = act_display;
	return 0;
}

UINT __stdcall CPaint::handle_loadimage2(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;

	pThis->load_image("first", pThis->_fname.c_str());
	pThis->_fnameLoaded = true;
	pThis->loadimage2ok();

	return 0;
}

void CPaint::loadimage2ok()
{
	_curr_nav_dto.file_name = _fname;
	wstring s = _fname + wstring(L" - ") + PRODUCT_NAME;
	HWND pParent = ::GetParent(_win_handle);
	::SetWindowText(pParent, s.c_str());

}

UINT __stdcall CPaint::handle_loadimage(LPVOID pParam) {
	CPaint* pThis = (CPaint*)pParam;

	wstring coi = pThis->_curr_nav_dto.ori_name;
	wstring::size_type  pos = coi.find(L".krf");
	if (pos != wstring::npos)
	{
		coi = coi.substr(0, pos) + L"_coi.png.krf";
	}
	else
	{
		coi += L"_coi.png";
	}

	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFile(coi.c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		pThis->_exist_coi = true;
		FindClose(hFind);
		pThis->load_image("first", coi.c_str());
	}
	else
	{
		pThis->_exist_coi = false;
		pThis->load_image("first", pThis->_curr_nav_dto.ori_name.c_str());
	}
	pThis->_fnameLoaded = true;

	return 0;
}

void CPaint::add_opt(opt_t& op, bool bNew)
{
	if (_lena2.empty())
	{
		return;
	}

	time(&(op.t));
	if (bNew)
	{
		op.lena_in_frame = _lena_in_frame;
	}
	op.loDiff = _loDiff;
	op.upDiff = _upDiff;

	EnterCriticalSection(&_cs);
	{
		int len0 = (int)_opt_arr.size();
		if (len0 > _idx && len0 > 0)
		{
			_opt_arr.erase(begin(_opt_arr) + _idx, end(_opt_arr));
		}

		bool b = false;
		if (op.opt_type == btn_filter)
		{
			if (!_opt_arr.empty())
			{
				int k = (int)_opt_arr.size() - 1;
				if (_opt_arr[k].opt_type == btn_filter)
				{
					_opt_arr[k].image = _weightedImage.clone();
					_opt_arr[k].fVal = _fVal;

					b = true;
				}
			}
		}

		if (!b)
		{
			_opt_arr.push_back(op);
		}

		_idx = (int)_opt_arr.size();
	}
	LeaveCriticalSection(&_cs);

	_mask_idx = -997;
}

double CPaint::get_zoom_r(int type)
{
	double r0 = 1.0f;
	double r = 1.0f;

	if (_lena2.empty())
	{
		return r;
	}

	if (_zoom_r >= 4.0f)
	{
		r = 4.0f;
		r0 = 3.5;
	}
	else if (_zoom_r >= 3.5)
	{
		r = 4.0f;
		r0 = 3.0f;
	}
	else if (_zoom_r >= 3.0f)
	{
		r = 3.5f;
		r0 = 2.5f;
	}
	else if (_zoom_r >= 2.5f)
	{
		r = 3.0f;
		r0 = 2.0f;
	}
	else if (_zoom_r >= 2.0f)
	{
		r = 2.5f;
		r0 = 1.5f;
	}
	else if (_zoom_r >= 1.5f)
	{
		r = 2.0f;
		r0 = 1.0f;
	}
	else if (_zoom_r >= 1.0f)
	{
		r = 1.5f;
		r0 = 0.9f;
	}
	else if (_zoom_r >= 0.9f)
	{
		r = 1.0f;
		r0 = 0.8f;
	}
	else if (_zoom_r >= 0.8f)
	{
		r = 0.9f;
		r0 = 0.7f;
	}
	else if (_zoom_r >= 0.7f)
	{
		r = 0.8f;
		r0 = 0.6f;
	}
	else if (_zoom_r >= 0.6f)
	{
		r = 0.7f;
		r0 = 0.5f;
	}
	else if (_zoom_r >= 0.5f)
	{
		r = 0.6f;
		r0 = 0.4f;
	}
	else if (_zoom_r >= 0.4f)
	{
		r = 0.5f;
		r0 = 0.3f;
	}
	else if (_zoom_r >= 0.3f)
	{
		r = 0.4f;
		r0 = 0.2f;
	}
	else if (_zoom_r >= 0.2f)
	{
		r = 0.3f;
		r0 = 0.1f;
	}
	else if (_zoom_r >= 0.1f)
	{
		r = 0.2f;
		r0 = 0.09f;
	}
	else if (_zoom_r >= 0.09f)
	{
		r = 0.1f;
		r0 = 0.08f;
	}
	else if (_zoom_r >= 0.08f)
	{
		r = 0.09f;
		r0 = 0.07f;
	}
	else if (_zoom_r >= 0.07f)
	{
		r = 0.08f;
		r0 = 0.06f;
	}
	else
	{
		r = 0.07f;
		r0 = 0.05f;
	}

	r = (type == 0) ? r : r0;

	if (r < 1.0)
	{
		if (_lena2.cols < 100 || _lena2.rows < 100)
		{
			return 1.0;
		}
		else
		{
			double w0 = min(_lena2.cols, _lena2.rows);
			if (w0 * r < 100.0f)
			{
				return 100.0f / w0;
			}
		}
	}

	return r;
}

int CPaint::save_coi(int type, Mat src, wchar_t* szCoi, wchar_t* szDst)
{
	vector<int> ql;
	ql.push_back(IMWRITE_PNG_COMPRESSION);
	ql.push_back(3);

	//coi
	{
		vector<uchar> buf;
		imencode(".png", src, buf, ql);

		ofstream file2(szCoi, ios::out | ios::binary);
		if (!file2)
		{
			return -1;
		}
		file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
		file2.close();

		if (type == 1)
		{
			wstring tt = szCoi;
			tt += L".krf";

			Encryptfile(szCoi, tt.c_str());
			DeleteFile(szCoi);
		}
	}

	wstring t = szDst;
	wstring::size_type  pos = t.find_last_of(L"_");
	if (pos != wstring::npos)
	{
		t = t.substr(0, pos);
	}

	wstring szfile = t + L"_coi.png";

	int width = src.cols;
	int height = src.rows;

	double scale = get_scale(1, width, height);

	Size ResImgSiz = Size(0, 0);
	Mat ResImg = Mat(ResImgSiz, src.type());
	if (scale < 2)
	{
		ResImg = src;
	}
	else
	{
		double f = 1.0f / scale;
		resize(src, ResImg, ResImgSiz, f, f, resizemode());
	}

	vector<uchar> buf;
	imencode(".png", ResImg, buf, ql);

	ofstream file2(szfile.c_str(), ios::out | ios::binary);
	if (!file2)
	{
		return -1;
	}
	file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
	file2.close();

	Encryptfile(szfile, szfile + L".krf");

	//SetFileAttributes(szfile.c_str(), FILE_ATTRIBUTE_NORMAL);
	DeleteFile(szfile.c_str());


	Mat ResImg2;
	resize(ResImg, ResImg2, Size(50, 50), 0, 0, resizemode());

	vector<uchar> buf3;
	imencode(".png", ResImg2, buf3, ql);

	szfile += L"_2.png";
	ofstream file3(szfile.c_str(), ios::out | ios::binary);
	if (!file3)
	{
		return -1;
	}
	file3.write((char*)&buf3[0], buf3.size() * sizeof(uchar));
	file3.close();

	Encryptfile(szfile, szfile + L".krf");
	//SetFileAttributes(szfile.c_str(), FILE_ATTRIBUTE_NORMAL);
	DeleteFile(szfile.c_str());

	return 0;
}

int CPaint::Encryptfile(wstring src, wstring Encryptname)
{
	HANDLE hWrite = CreateFile(Encryptname.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hWrite == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	HANDLE hFile = CreateFile(src.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hWrite);
		return 0;
	}

	// Retrive file size
	DWORD lengthActual = GetFileSize(hFile, NULL);

	unsigned char buf_hdr[100] = { 0 };

	DE_HEADER* de_hdr = (DE_HEADER*)buf_hdr;
	de_hdr->type = 1;

	div_t div_result = div(lengthActual, 8);
	if (div_result.rem != 0)
	{
		de_hdr->offset = 8 - div_result.rem;
	}

	de_hdr->crc = _BF._crc;

	srand((unsigned int)time(NULL));
	de_hdr->ot1 = rand() * 255;
	de_hdr->ot2 = rand() * 255;
	de_hdr->ot3 = 2024;

	DWORD dwWritten = 0;
	WriteFile(hWrite, buf_hdr, sizeof(DE_HEADER), &dwWritten, NULL);

	DWORD npos = 0, dwRead = 0;

	int nTemp = 100 * 1024;
	unsigned char* tempbuf = (unsigned char*)malloc(nTemp + 1);
	if (tempbuf != NULL)
	{
		while (npos < lengthActual)
		{
			memset(tempbuf, 0, nTemp + 1);

			if (npos == 0 && div_result.rem != 0)
			{
				BOOL b = ReadFile(hFile, tempbuf + de_hdr->offset, div_result.rem, &dwRead, 0);
				if (b)
				{
					_BF.Encrypt((void*)tempbuf, 8);
					WriteFile(hWrite, tempbuf, 8, &dwWritten, NULL);
				}
				else
				{
					break;
				}
			}
			else
			{
				BOOL b = ReadFile(hFile, tempbuf, nTemp, &dwRead, 0);
				if (b)
				{
					_BF.Encrypt((void*)tempbuf, dwRead);
					WriteFile(hWrite, tempbuf, dwRead, &dwWritten, NULL);
				}
				else
				{
					break;
				}

			}

			npos += dwRead;
		}

		free(tempbuf);
	}

	CloseHandle(hWrite);
	CloseHandle(hFile);

	return 0;
}

void CPaint::saveas(int btn, HWND hWnd)
{
	if (_lena2.empty())
	{
		return;
	}

	if (toolbar_save == btn && (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE))
	{
		DWORD dwAttr = GetFileAttributes(_curr_nav_dto.ori_name.c_str());
		if (dwAttr != -1)
		{
			// found something; better not be a folder....
			if (dwAttr & FILE_ATTRIBUTE_READONLY)
			{
				_save_ret = -2;
				_disp_nb = 1;
				return;
			}
		}
	}

	int w = 0;
	int h = 0;
	int q = 0;
	_save_ret = save(btn, w, h, q);
	if (_save_ret != 2)
	{
		_disp_nb = 1;
	}

	if (toolbar_save == btn && (_app_type == FROM_APP_NAV || _app_type == FROM_APP_ONE))
	{
		if (_ov == 0)
		{
			int id = _wtoi(_curr_nav_dto.id.c_str());
			_exist_coi = true;

			char buf[100] = { 0 };
			sprintf_s(buf, 100, "w=%d&h=%d&q=%d", w, h, q);
			ATOM atom = GlobalAddAtomA(buf);
			PostMessage(hWnd, WM_PAINT_SAVE, id, (LPARAM)atom);
		}
	}

	if (_close_ret == 1)
	{
		_save_click = false;
		_inpaint_click = false;
		_close_ret = 0;
		_closed = true;
	}
}

void CPaint::nav(int type)
{
	if (type == 0)
	{
		_idx_file--;
		if (_idx_file <= 0)
		{
			_idx_file = 1;
			return;
		}
	}
	else
	{
		_idx_file++;
		if (_idx_file > _len_files)
		{
			_idx_file = _len_files;
			return;
		}
	}

	list<wstring>::iterator it;
	int i = 0;
	for (it = _lstFile.begin(); it != _lstFile.end(); it++)
	{
		i++;
		if (i == _idx_file)
		{
			if (_app_type == FROM_EXE)
			{
				wstring s = *it + wstring(L" - ") + PRODUCT_NAME;
				load_image("", it->c_str());
				HWND pParent = ::GetParent(_win_handle);
				::SetWindowText(pParent, s.c_str());
				_curr_nav_dto.file_name = *it;
			}
			else
			{
				praser_file(*it);

				wstring coi = _curr_nav_dto.ori_name;
				wstring::size_type  pos = coi.find(L".krf");
				if (pos != wstring::npos)
				{
					coi = coi.substr(0, pos) + L"_coi.png.krf";
				}
				else
				{
					coi += L"_coi.png";
				}

				WIN32_FIND_DATAW ffd;
				HANDLE hFind = FindFirstFile(coi.c_str(), &ffd);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					_exist_coi = true;
					FindClose(hFind);
					load_image("", coi.c_str());
				}
				else
				{
					_exist_coi = false;
					load_image("", _curr_nav_dto.ori_name.c_str());
				}
			}
			break;
		}
	}
}

Mat CPaint::RotateImage(Mat& src, double angle, bool isClip)
{
	if (angle == 180)
	{
		flip(src, src, -1);
	}
	else
	{
		Mat temp_src;
		transpose(src, temp_src);

		if (angle == -90)
		{
			flip(temp_src, src, 0);
		}
		else if (angle == 90)
		{
			flip(temp_src, src, 1);
		}

	}
	return src;
}

int CPaint::watermark(Mat& wordSrc)
{
	Mat birdSrc = lbl_ov;

	int rOffset, cOffset;
	float bOca = 0.3f, wOca = 1 - bOca;
	int gamma = 0;
	for (int r = 0; r < wordSrc.rows; r++)
	{
		rOffset = r / birdSrc.rows * birdSrc.rows;
		for (int c = 0; c < wordSrc.cols; c++)
		{
			cOffset = c / birdSrc.cols * birdSrc.cols;

			Vec3b* wPixel = wordSrc.ptr<Vec3b>(r, c);
			Vec3b* bPixel = birdSrc.ptr<Vec3b>(r - rOffset, c - cOffset);

			if (bPixel->val[0] != 0)
			{
				int val;
				val = wPixel->val[0] * wOca + bPixel->val[0] * bOca + gamma;//B
				wPixel->val[0] = val > 255 ? 255 : (val < 0 ? 0 : val);
				val = wPixel->val[1] * wOca + bPixel->val[1] * bOca + gamma;//G
				wPixel->val[1] = val > 255 ? 255 : (val < 0 ? 0 : val);
				val = wPixel->val[2] * wOca + bPixel->val[2] * bOca + gamma;//R
				wPixel->val[2] = val > 255 ? 255 : (val < 0 ? 0 : val);
			}
		}
	}
	return 0;
}