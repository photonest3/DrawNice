// mfthumb.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"

#include <io.h>
#include <shlobj.h>
#include <thumbcache.h>
#include <string>
#include<fstream>

#include "photol.h"
#include "unitil2.h"
#include "unitil3.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/objdetect.hpp>
#include <opencv2/face.hpp>

#include <MediaInfo/MediaInfo.h>
#include <gif_lib.h>
#include <process.h>
#include "resource.h"
#include "Blowfish.h"

#include "paint.h"

using namespace std;
using namespace cv;
using namespace cv::face;

//#pragma comment( lib, "ippicvmt.lib" )
//#pragma comment( lib, "ippiw.lib" )
//#pragma comment( lib, "IlmImf.lib" )

#pragma comment( lib, "libtiff.lib" )
#pragma comment( lib, "libpng.lib" )
//#pragma comment( lib, "libjasper.lib" )
#pragma comment( lib, "libjpeg-turbo.lib" )
#pragma comment( lib, "libwebp.lib" )
#pragma comment( lib, "libopenjp2.lib" )
#pragma comment( lib, "zlib.lib" )

#pragma comment( lib, "opencv_core480.lib" )
#pragma comment( lib, "opencv_imgcodecs480.lib" )
#pragma comment( lib, "opencv_imgproc480.lib" )
#pragma comment( lib, "opencv_quality480.lib" )
#pragma comment( lib, "opencv_ml480.lib" )
#pragma comment( lib, "opencv_highgui480.lib" ) 
#pragma comment( lib, "opencv_photo480.lib" ) 

#pragma comment( lib, "opencv_objdetect480.lib" )
#pragma comment( lib, "opencv_face480.lib" )

#pragma comment( lib, "giflib.lib" )
#pragma comment( lib, "ZenLib.lib" ) 
#pragma comment( lib, "MediaInfo-Static.lib" ) 
#pragma comment( lib, "lunasvg.lib" ) 

CBlowfish* pBF = NULL;
CPaint* pPaint = NULL;
Mat _artMat;

wstring cascade_file = L"";
wstring eye1_file = L"";
wstring eye2_file = L"";
string face_model = "";

Ptr<LBPHFaceRecognizer> model;
CascadeClassifier cascade_face;
CascadeClassifier classifier_eye1;
CascadeClassifier classifier_eye2;

void add_arr(Mat& bg0)
{
	int channels = bg0.channels();
	if (channels == 4)
	{
		cvtColor(bg0, bg0, CV_BGRA2BGR);
	}

	channels = bg0.channels();
	if (channels == 3)
	{
		if (_artMat.empty())
		{
			HINSTANCE his = ::GetModuleHandle(L"photol.dll");
			load_bitmap(his, _artMat, IDB_ART);
		}

		if (bg0.cols > _artMat.cols && bg0.rows > _artMat.rows)
		{
			int	width = _artMat.cols;
			int height = _artMat.rows;
			Mat roi = bg0(Rect((bg0.cols - width) / 2, (bg0.rows - height) / 2, width, height));

			for (int i = 0; i < roi.rows; i++)
			{
				for (int j = 0; j < roi.cols; j++)
				{
					Vec3b& v1 = roi.at<Vec3b>(i, j);
					Vec3b v2 = _artMat.at<Vec3b>(i, j);
					if (v2[0] == 0 && v2[1] == 0 && v2[2] == 255)
					{
					}
					else
					{
						v1 = v2;
					}
				}
			}
		}
	}
}

unsigned char* memstr(unsigned char* full_data, int full_data_len, unsigned char* substr, int sublen)
{
	if (full_data == NULL || full_data_len <= 0 || substr == NULL)
	{
		return NULL;
	}

	if (*substr == '\0')
	{
		return NULL;
	}

	//int sublen = strlen(substr);

	int i;
	unsigned char* cur = full_data;
	int last_possible = full_data_len - sublen + 1;
	for (i = 0; i < last_possible; i++)
	{
		if (*cur == *substr)
		{
			//assert(full_data_len - i >= sublen);
			if (memcmp(cur, substr, sublen) == 0)
			{
				//found
				return cur;
			}
		}
		cur++;
	}

	return NULL;
}

int save_mat(int ty, const char* ext, Mat src, const wchar_t* szDst, bool ismp4)
{
	string ext2 = ext;
	vector<int> ql;
	if (strcmp(ext, ".png") == 0)
	{
		ext2 = ".png";
		ql.push_back(IMWRITE_PNG_COMPRESSION);
		ql.push_back(3);
	}
	else if (strcmp(ext, ".webp") == 0)
	{
		ext2 = ".webp";
		//q.push_back(IMWRITE_WEBP_QUALITY);
		//q.push_back(64);
	}
	else /*if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)*/
	{
		ext2 = ".jpg";
		ql.push_back(IMWRITE_JPEG_QUALITY);
		ql.push_back(70);
	}

	wchar_t dst[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szDst, MAX_PATH, dst, NULL);

	int width = src.cols;
	int height = src.rows;

	if (ty == 2)
	{
		Mat ResImg;
		if (width == 50 && height == 50) {
			ResImg = src;
		}
		else
		{
			resize(src, ResImg, Size(50, 50), 0, 0);
		}

		if (ismp4)
		{
			add_arr(ResImg);
		}

		vector<uchar> buf;
		imencode(ext, ResImg, buf, ql);

		ofstream file2(szDst, ios::out | ios::binary);
		if (!file2)
		{
			return -1;
		}
		file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
		file2.close();
	}
	else
	{
		double scale = get_scale(ty, width, height);

		Size ResImgSiz = Size(0, 0);
		Mat ResImg = Mat(ResImgSiz, src.type());
		if (scale < 2)
		{
			ResImg = src;
		}
		else
		{
			double f = 1.0f / scale;
			resize(src, ResImg, ResImgSiz, f, f);
		}

		vector<uchar> buf;
		imencode(ext2.c_str(), ResImg, buf, ql);

		ofstream file2(dst, ios::out | ios::binary);
		if (!file2)
		{
			return -1;
		}
		file2.write((char*)&buf[0], buf.size() * sizeof(uchar));
		file2.close();

		Mat ResImg2;
		if (width == 50 && height == 50) {
			ResImg2 = ResImg;
		}
		else
		{
			resize(ResImg, ResImg2, Size(50, 50), 0, 0);
		}

		if (ismp4)
		{
			add_arr(ResImg2);
		}

		vector<uchar> buf3;
		imencode(ext2.c_str(), ResImg2, buf3, ql);

		wstring szDst2 = dst;
		szDst2 += L"_2.png";
		ofstream file3(szDst2.c_str(), ios::out | ios::binary);
		if (!file3)
		{
			return -1;
		}
		file3.write((char*)&buf3[0], buf3.size() * sizeof(uchar));
		file3.close();
	}
	return 0;
}

int openmp4file(const wchar_t* sMp4file, uint32_t& width, uint32_t& height, uint32_t& duration, long& mp4rotate)
{
	wstring W, H;
	MediaInfoLib::MediaInfo* _MI = new MediaInfoLib::MediaInfo();
	_MI->Open(sMp4file);

	W = _MI->Get(MediaInfoLib::stream_t::Stream_Video, 0, L"Width");
	H = _MI->Get(MediaInfoLib::stream_t::Stream_Video, 0, L"Height");
	wstring d = _MI->Get(MediaInfoLib::stream_t::Stream_Video, 0, L"Duration");
	wstring r = _MI->Get(MediaInfoLib::stream_t::Stream_Video, 0, L"Rotation");

	width = _wtoi(W.c_str());
	height = _wtoi(H.c_str());
	duration = _wtol(d.c_str()) / 1000;
	mp4rotate = _wtoi(r.c_str());
	//90.000

	if (mp4rotate == 90 || mp4rotate == 270)
	{
		long t = width;
		width = height;
		height = t;
	}

	_MI->Close();
	delete _MI;

	if (width > 0)
	{
		return 0;
	}

	return -1;
}

inline bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{

	typedef LONG(__stdcall* fnRtlGetVersion)(PRTL_OSVERSIONINFOW lpVersionInformation);

	RTL_OSVERSIONINFOEXW verInfo = { 0 };
	verInfo.dwOSVersionInfoSize = sizeof(verInfo);

	static auto RtlGetVersion = (fnRtlGetVersion)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");

	if (RtlGetVersion != 0 && RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo) == 0)
	{
		if (verInfo.dwMajorVersion > wMajorVersion)
			return true;
		else if (verInfo.dwMajorVersion < wMajorVersion)
			return false;

		if (verInfo.dwMinorVersion > wMinorVersion)
			return true;
		else if (verInfo.dwMinorVersion < wMinorVersion)
			return false;

		if (verInfo.wServicePackMajor >= wServicePackMajor)
			return true;
	}

	return false;
}

inline bool IsWindows8OrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}

inline void NewSize(Mat src2)
{
	Size si = src2.size();
	//3840 2160
	//	7680 4320
	//	6000 3000
	//	6000 4000
	//	4096 2160
	//	6250 3515
	//	5000 2800
	//	------ - 1920 2048 2000

	int val = max(si.width, si.height);
	if (val > 5000)
	{
		float f = 1.0f;
		div_t div_result = div(val, 7);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 7.0f;
		}
		div_result = div(val, 6);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 6.0f;
		}
		div_result = div(val, 5);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 5.0f;
		}
		div_result = div(val, 4);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 4.0f;
		}
		div_result = div(val, 3);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 3.0f;
		}
		div_result = div(val, 2);
		if (div_result.rem == 0 && div_result.quot > 2000 && div_result.quot < 4000)
		{
			f = 2.0f;
		}

		if (f < 2.0f)
		{
			if (val > 10000)
			{
				f = 4.0f;
			}
			else
			{
				f = 2.0f;
			}
		}
		resize(src2, src2, Size((int)(si.width / f), (int)(si.height / f)), 0, 0);
	}
}

HRESULT proc_bimap(int ty, HBITMAP hbmp, wstring dst, long& mp4rotate)
{
	if (hbmp)
	{
		BITMAP bm = { 0 };
		int ret = GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bm);
		if (ret == 0 || ret > sizeof(BITMAP))
		{
			return S_FALSE;
		}

		int nChannels = bm.bmBitsPixel == 1 ? 1 : bm.bmBitsPixel / 8;
		int depth = bm.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

		Mat src;
		src.create(cvSize(bm.bmWidth, bm.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
		GetBitmapBits(hbmp, bm.bmHeight * bm.bmWidth * nChannels, src.data);

		if (mp4rotate == 90 || mp4rotate == 270)
		{
			if (!IsWindows8OrGreater())
			{
				Mat temp;
				transpose(src, temp);
				flip(temp, src, 1);
			}
		}

		if (src.data == NULL)
		{
			return 2;
		}

		ret = save_mat(ty, ".png", src, dst.c_str(), true);
		if (ret == 0)
		{
			return 0;
		}
	}
	return S_FALSE;
}

HRESULT GetThumbnailEx(int ty, uint32_t cx, wstring szFile, wstring dst, long& mp4rotate)
{
	HRESULT hr;

	IShellItem* psi;
	hr = SHCreateItemFromParsingName(szFile.c_str(), NULL, IID_PPV_ARGS(&psi));
	if (SUCCEEDED(hr))
	{
		IThumbnailProvider* pThumbProvider;
		hr = psi->BindToHandler(NULL, BHID_ThumbnailHandler, IID_PPV_ARGS(&pThumbProvider));
		if (SUCCEEDED(hr))
		{
			HBITMAP hThumbnail = NULL;
			WTS_ALPHATYPE wtsAlpha = WTSAT_UNKNOWN;
			hr = pThumbProvider->GetThumbnail(cx, &hThumbnail, &wtsAlpha);
			if (SUCCEEDED(hr))
			{
				hr = proc_bimap(ty, hThumbnail, dst, mp4rotate);
				DeleteObject(hThumbnail);
			}

			pThumbProvider->Release();
		}
		psi->Release();
	}

	return hr;
}

HRESULT GetCacheThumbnail(int ty, uint32_t width, uint32_t height, const wchar_t* szFile, wstring dst, long& mp4rotate)
{
	IShellItem* item = nullptr;
	HRESULT hr = SHCreateItemFromParsingName(szFile, nullptr, IID_PPV_ARGS(&item));
	if (hr == S_OK)
	{
		IThumbnailCache* cache = nullptr;
		hr = CoCreateInstance(
			CLSID_LocalThumbnailCache,
			nullptr,
			CLSCTX_INPROC,
			IID_PPV_ARGS(&cache));
		if (hr == S_OK)
		{
			ISharedBitmap* shared_bitmap;
			hr = cache->GetThumbnail(
				item,
				width * height,
				WTS_EXTRACT,
				&shared_bitmap,
				nullptr,
				nullptr);

			if (hr == S_OK)
			{
				// Retrieve thumbnail HBITMAP
				HBITMAP hThumbnail = NULL;
				hr = shared_bitmap->GetSharedBitmap(&hThumbnail);
				if (hr == S_OK)
				{
					hr = proc_bimap(ty, hThumbnail, dst, mp4rotate);
					DeleteObject(hThumbnail);
				}

				shared_bitmap->Release();
			}

			cache->Release();
		}
		item->Release();
	}
	return hr;
}

PHOTOL_API int resize_img(const char* ext, const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height, uint32_t& level)
{
	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	ifstream file(src, ios::in | ios::binary | ios::ate);
	if (!file.is_open())
	{
		return -1;
	}

	streampos size = file.tellg();
	file.seekg(0, ios::beg);
	string buffer(size, 0);
	file.read(&buffer[0], size);
	file.close();
	if ((int)size == 0)
	{
		return 2;
	}

	vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
	Mat src2 = imdecode(vec_data, IMREAD_UNCHANGED);
	int u = src2.depth();
	if (src2.data != NULL && u != 0)
	{
		Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
		temp.convertTo(src2, CV_8U);
	}

	if (src2.data == NULL)
	{
		return 2;
	}

	int ty = 2;
	if (width == 0 && height == 0)
	{
		ty = 0;
	}

	width = src2.cols;
	height = src2.rows;

	if (ty == 2 && (width < 16 || height < 16))
	{
		//		return 1;
	}
	int ret = save_mat(ty, ext, src2, szDst, false);
	if (level == 1)
	{
		NewSize(src2);
		level = (uint32_t)(BRISQUE(src2) * 100);

	}

	return ret;
}

PHOTOL_API int img_brisque(const wchar_t* szSrc, const char* enckey, uint32_t& level)
{
	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	ifstream file(src, ios::in | ios::binary | ios::ate);
	if (!file.is_open())
	{
		return -1;
	}

	streampos size = file.tellg();
	file.seekg(0, ios::beg);
	string buffer(size, 0);
	file.read(&buffer[0], size);
	file.close();
	if ((int)size == 0)
	{
		return 2;
	}
	vector<uchar> vec_data;

	if (strlen(enckey) == 0)
	{
		vec_data = vector<uchar>(&buffer[0], &buffer[0] + size);
	}
	else
	{
		if (pBF == NULL)
		{
			pBF = new CBlowfish();
			pBF->SetPassword((char*)enckey);
		}

		DWORD lengthActual = (DWORD)size;

		DE_HEADER* de_hdr = (DE_HEADER*)buffer.c_str();
		int offset = 8 + de_hdr->offset;

		pBF->Decrypt((void*)(buffer.c_str() + 8), lengthActual - 8);

		vec_data = vector<uchar>(&buffer[offset], &buffer[offset] + lengthActual - offset);
	}

	//vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
	Mat src2 = imdecode(vec_data, IMREAD_UNCHANGED);
	int u = src2.depth();
	if (src2.data != NULL && u != 0)
	{
		Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
		temp.convertTo(src2, CV_8U);
	}

	if (src2.data == NULL)
	{
		return 2;
	}

	NewSize(src2);
	level = (uint32_t)(BRISQUE(src2) * 100);

	return 0;
}

PHOTOL_API int img_size(const wchar_t* szSrc, uint32_t& width, uint32_t& height)
{
	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	ifstream file(src, ios::in | ios::binary | ios::ate);
	if (!file.is_open())
	{
		return -1;
	}

	streampos size = file.tellg();
	file.seekg(0, ios::beg);
	string buffer(size, 0);
	file.read(&buffer[0], size);
	file.close();
	if ((int)size == 0)
	{
		return 2;
	}

	vector<uchar> vec_data(&buffer[0], &buffer[0] + size);
	Mat src2 = imdecode(vec_data, IMREAD_UNCHANGED);
	int u = src2.depth();
	if (src2.data != NULL && u != 0)
	{
		Mat temp = imdecode(vec_data, IMREAD_ANYCOLOR);
		temp.convertTo(src2, CV_8U);
	}

	if (src2.data == NULL)
	{
		return 2;
	}

	width = src2.cols;
	height = src2.rows;

	return 0;
}

PHOTOL_API int capture_mp4(const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height, uint32_t& duration)
{
	int ty = 2;
	if (width == 0 && height == 0)
	{
		ty = 0;
	}

	int rt = 2;
	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	long mp4rotate = 0;
	int ret = openmp4file(src, width, height, duration, mp4rotate);
	if (ret != 0)
	{
		width = 576;
		height = 576;
	}

	HRESULT hr = GetThumbnailEx(ty, max(width, height), src, szDst, mp4rotate);
	if (hr != S_OK)
	{
		Sleep(100);
		hr = GetCacheThumbnail(ty, width, height, src, szDst, mp4rotate);
	}
	if (hr == 0)
	{
		rt = 0;
	}
	else
	{
		width = 0;
		height = 0;
		rt = 1;
	}


	if (rt != 0 && ty == 2)
	{
		//url0 = sample + L"\\templ\\themes\\default\\images\\mp4.png";
		Mat mp4;
		HINSTANCE his = ::GetModuleHandle(L"photol.dll");
		load_bitmap(his, mp4, IDB_MP4);
		if (!mp4.empty())
		{
			rt = save_mat(ty, ".png", mp4, szDst, false);
		}
	}
	return rt;
}

PHOTOL_API int capture_gif(const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height)
{
	int ty = 2;
	if (width == 0 && height == 0)
	{
		ty = 0;
	}

	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	int ret = 2;
	int err;

	HANDLE hFile = CreateFile(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ret;
	}

	bool error = false;
	GifFileType* GifFile = NULL;
	int fd = ::_open_osfhandle(reinterpret_cast <::intptr_t> (hFile), _A_RDONLY);
	if (-1 == fd)
	{
		error = true;
	}
	else
	{
		GifFile = DGifOpenFileHandle(fd, &err);
		if (GifFile == NULL)
		{
			error = true;
		}
	}

	if (error)
	{
		//CloseHandle(hFile);
		return ret;
	}


	GifRowType* ScreenBuffer = (GifRowType*)malloc(GifFile->SHeight * sizeof(GifRowType));
	if (ScreenBuffer == NULL)
	{
		error = true;
	}
	else
	{
		int len0 = GifFile->SWidth * sizeof(GifPixelType);
		ScreenBuffer[0] = (GifRowType)malloc(len0);
		if (ScreenBuffer[0] == NULL)
		{
			error = true;
		}
		else
		{
			for (int i = 0; i < GifFile->SWidth; i++)
				ScreenBuffer[0][i] = GifFile->SBackGroundColor;

			for (int i = 1; i < GifFile->SHeight; i++)
			{
				ScreenBuffer[i] = (GifRowType)malloc(len0);
				if (ScreenBuffer[i] == NULL)
				{
					error = true;
				}
				else
				{
					memcpy(ScreenBuffer[i], ScreenBuffer[0], len0);
				}
			}
		}
	}

	ColorMapObject* ColorMap = NULL;
	if (!error)
	{
		ColorMap = (GifFile->Image.ColorMap ? GifFile->Image.ColorMap : GifFile->SColorMap);
		if (NULL == ColorMap)
		{
			error = true;
		}
	}

	if (error)
	{
		if (ScreenBuffer != NULL)
		{
			free(ScreenBuffer);
		}

		DGifCloseFile(GifFile, &err);
		return ret;
	}

	GifRecordType RecordType;
	do
	{
		if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR)
		{
			break;
		}

		switch (RecordType)
		{
		case IMAGE_DESC_RECORD_TYPE:
		{
			if (DGifGetImageDesc(GifFile) == GIF_ERROR)
			{
				error = true;
				break;
			}

			int Row = GifFile->Image.Top;
			int Col = GifFile->Image.Left;
			width = GifFile->Image.Width;
			height = GifFile->Image.Height;

			if (GifFile->Image.Left + GifFile->Image.Width > GifFile->SWidth ||
				GifFile->Image.Top + GifFile->Image.Height > GifFile->SHeight)
			{
				error = true;
				break;
			}

			if (GifFile->Image.Interlace)
			{
				int	InterlacedOffset[] = { 0, 4, 2, 1 };
				int	InterlacedJumps[] = { 8, 8, 4, 2 };

				for (int i = 0; i < 4; i++)
				{
					for (int j = Row + InterlacedOffset[i]; j < Row + height; j += InterlacedJumps[i])
					{
						if (DGifGetLine(GifFile, &ScreenBuffer[j][Col], width) == GIF_ERROR)
						{
							error = true;
							break;
						}
					}

					if (error)
					{
						break;
					}
				}
			}
			else
			{
				for (uint32_t i = 0; i < height; i++)
				{
					if (DGifGetLine(GifFile, &ScreenBuffer[Row++][Col], width) == GIF_ERROR)
					{
						error = true;
						break;
					}
				}
			}

			if (!error)
			{
				width = GifFile->SWidth;
				height = GifFile->SHeight;

				Mat	img = Mat(Size(GifFile->SWidth, GifFile->SHeight), CV_8UC3);
				GifRowType GifRow;
				GifColorType* ColorMapEntry;
				for (int i = 0; i < GifFile->SHeight; i++)
				{
					GifRow = ScreenBuffer[i];
					for (int j = 0; j < GifFile->SWidth; j++)
					{
						ColorMapEntry = &ColorMap->Colors[GifRow[j]];
						img.at<Vec3b>(i, j) = Vec3b(ColorMapEntry->Blue, ColorMapEntry->Green, ColorMapEntry->Red);
					}
				}

				ret = save_mat(ty, ".png", img, szDst, false);

				error = true;
				break;
			}
		}
		break;
		case EXTENSION_RECORD_TYPE:
		{
			int  ExtCode;
			GifByteType* Extension;
			if (DGifGetExtension(GifFile, &ExtCode, &Extension) == GIF_ERROR)
			{
				error = true;
				break;
			}
			while (Extension != NULL)
			{
				if (DGifGetExtensionNext(GifFile, &Extension) == GIF_ERROR)
				{
					error = true;
					break;
				}
			}

			if (error)
			{
				break;
			}
		}
		break;
		case TERMINATE_RECORD_TYPE:
			break;
		default:
			break;
		}

		if (error)
		{
			break;
		}

	} while (RecordType != TERMINATE_RECORD_TYPE);

	free(ScreenBuffer);
	DGifCloseFile(GifFile, &err);
	return ret;
}

PHOTOL_API int mp3_cover(int ty, const wchar_t* szSrc, const wchar_t* szDst, long& duration, bool& cover)
{
	wchar_t src[MAX_PATH] = { 0 };
	DWORD dwSize = GetFullPathName(szSrc, MAX_PATH, src, NULL);

	int rt = 1;
	cover = false;

	MediaInfoLib::MediaInfo* _MI = new MediaInfoLib::MediaInfo();

	_MI->Open(src);
	wstring d = _MI->Get(MediaInfoLib::stream_t::Stream_Audio, 0, L"Duration");
	duration = _wtol(d.c_str()) / 1000;

	wstring cover0 = _MI->Get(MediaInfoLib::stream_t::Stream_General, 0, L"Cover");
	_MI->Close();
	delete _MI;

	if (duration == 0)
	{
		rt = -1;
	}
	else
	{
		if (cover0 != L"")
		{
			char* pic1 = NULL;
			int filesize = 0;

			HANDLE hFile = CreateFile(src, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				int nTemp = 1000 * 1024;
				unsigned char* tempbuf = (unsigned char*)malloc(nTemp + 1);
				if (tempbuf != NULL)
				{
					DWORD dwRead = 0;
					memset(tempbuf, 0, nTemp + 1);

					BOOL b = ReadFile(hFile, tempbuf, nTemp, &dwRead, 0);
					if (b)
					{
						unsigned char buf2[10] = { 0 };
						memcpy(buf2, "APIC", 4);
						unsigned char* p = memstr(tempbuf, dwRead, buf2, 4);
						if (p != NULL)
						{
							unsigned char* code = (unsigned char*)p + 4;
							filesize = code[0] * 0x1000000
								+ code[1] * 0x10000
								+ code[2] * 0x100
								+ code[3] * 0x1;

							if (filesize + p + 10 - tempbuf < nTemp)
							{
								unsigned char sub[3] = { 0xff,0xd8,0 };
								//unsigned char *pt = sub;
								unsigned char* p1 = memstr(p + 10, filesize, sub, 2);
								if (p1 != NULL)
								{
									filesize -= (int)(p1 - p - 10);
									pic1 = (char*)malloc(filesize);
									memset(pic1, 0, filesize);
									memcpy(pic1, p1, filesize);
								}
							}
						}
					}

					free(tempbuf);
				}

				CloseHandle(hFile);
			}

			if (pic1 != NULL)
			{
				vector<uchar> vec_data(pic1, pic1 + filesize);
				Mat src2 = imdecode(vec_data, 1);

				if (src2.data == NULL)
				{
					free(pic1);
					rt = 2;
				}
				else
				{
					rt = save_mat(ty, ".png", src2, szDst, true);
					free(pic1);

					cover = true;
				}
			}
		}
	}

	if (rt != 0 && ty == 2)
	{
		//url0 = sample + L"\\templ\\themes\\default\\images\\mp4.png";
		Mat mp3;
		HINSTANCE his = ::GetModuleHandle(L"photol.dll");
		load_bitmap(his, mp3, IDB_MP3);
		if (!mp3.empty())
		{
			rt = save_mat(ty, ".png", mp3, szDst, false);
		}
	}
	return rt;

}

PHOTOL_API int paint_show(int type, uint64_t hwnd, int pixelsy, int w, int h, const wchar_t* fname, const wchar_t* langue, const wchar_t* user, const wchar_t* enckey, int ov)
{
	if (pPaint == NULL)
	{
		pPaint = new CPaint();
		pPaint->init2(type, pixelsy, langue, user, ov);
	}

	pPaint->show((HWND)hwnd, w, h, fname, enckey);

	return 0;
}

PHOTOL_API int capture_ot(int type, const wchar_t* szDst)
{
	int ret = 0;
	UINT id = IDB_MP3;
	if (type == 1)
	{
		id = IDB_MP4;
	}
	else if (type == 2)
	{
		id = IDB_JPG;
	}
	Mat mp3;
	HINSTANCE his = ::GetModuleHandle(L"photol.dll");
	load_bitmap(his, mp3, id);
	if (!mp3.empty())
	{
		ret = save_mat(2, ".png", mp3, szDst, false);
	}
	return ret;
}

int getMatAndLabels(int ty, wstring root, const char* enckey, const char* filelist_txt, vector<TRAIN_DTO>& lstTrain, int& mul)
{
	vector<string> ids;
	vector<wstring> files;
	vector<string> v1 = tokenize(filelist_txt, "\r\n", true, "");

	int len = (int)v1.size();
	for (int i = 0; i < len; i++)
	{
		vector<string> v2 = tokenize(v1[i], "|", true, "");
		if ((int)v2.size() == 2)
		{
			ids.push_back(v2[0]);
			files.push_back(_u2w(v2[1]));
		}
	}
	//wstring root = get_module_path(NULL);

	if (cascade_file == L"")
	{
		cascade_file = root + L"\\bin\\model\\haarcascade_frontalface_alt2.yml"; //haarcascade_frontalface_default.yml";
		eye1_file = root + L"\\bin\\model\\haarcascade_lefteye_2splits.yml";// haarcascade_eye.yml"; //haarcascade_eye_tree_eyeglasses.yml";
		eye2_file = root + L"\\bin\\model\\haarcascade_righteye_2splits.yml";
	}

	if (cascade_face.empty())
	{
		cascade_face = CascadeClassifier(_w2u(cascade_file));
	}

	if (cascade_face.empty())
	{
		return 1;
	}

	if (classifier_eye1.empty())
	{
		classifier_eye1 = CascadeClassifier(_w2u(eye1_file));
	}
	if (classifier_eye1.empty())
	{
		return 1;
	}

	if (classifier_eye2.empty())
	{
		classifier_eye2 = CascadeClassifier(_w2u(eye2_file));
	}
	if (classifier_eye2.empty())
	{
		return 1;
	}

	for (int j = 0; j < files.size(); j++)
	{
		wchar_t src[MAX_PATH] = { 0 };
		DWORD dwSize = GetFullPathName(files[j].c_str(), MAX_PATH, src, NULL);

		ifstream file(src, ios::in | ios::binary | ios::ate);
		if (!file.is_open())
		{
			return -1;
		}

		streampos size = file.tellg();
		file.seekg(0, ios::beg);
		string buffer(size, 0);
		file.read(&buffer[0], size);
		file.close();
		if ((int)size == 0)
		{
			return 2;
		}
		vector<uchar> vec_data;

		if (strlen(enckey) == 0)
		{
			vec_data = vector<uchar>(&buffer[0], &buffer[0] + size);
		}
		else
		{
			if (pBF == NULL)
			{
				pBF = new CBlowfish();
				pBF->SetPassword((char*)enckey);
			}

			DWORD lengthActual = (DWORD)size;

			DE_HEADER* de_hdr = (DE_HEADER*)buffer.c_str();
			int offset = 8 + de_hdr->offset;

			pBF->Decrypt((void*)(buffer.c_str() + 8), lengthActual - 8);

			vec_data = vector<uchar>(&buffer[offset], &buffer[offset] + lengthActual - offset);
		}

		Mat frame = imdecode(vec_data, IMREAD_COLOR);
		if (!frame.empty())
		{
			if (ty == 1)
			{
				NewSize(frame);

				Mat gray;
				cvtColor(frame, gray, CV_BGR2GRAY);
				//equalizeHist(gray, gray);

					//faces:5,eye1s:5,eye1s&&eye2s
				Size minSize(100, 100);
				Size maxSize(800, 800);
				Size eyeSize(30, 30);

				double scaleFactor = 1.1;
				int minNeighbors = 5;

				int label = atoi(ids[j].c_str());
				vector<Rect> faces;
				vector<Rect> eye1s;
				vector<Rect> eye2s;
				cascade_face.detectMultiScale(gray, faces, scaleFactor, 3, CASCADE_DO_CANNY_PRUNING, minSize, maxSize);

				vector<TRAIN_DTO> lstDto;

				int len0 = (int)faces.size();
				for (int i = 0; i < len0; i++)
				{
					bool b1 = false;
					Rect rect = faces[i];
					rect.height = rect.height * 2 / 3;

					Mat temp = gray(rect).clone();
					equalizeHist(temp, temp);

					classifier_eye1.detectMultiScale(temp, eye1s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);
					classifier_eye2.detectMultiScale(temp, eye2s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);
					if (eye1s.size() > 0 && eye2s.size() > 0)
					{
						b1 = true;
					}

					/*if (len0 == 1)*/
					{
						if (eye1s.size() > 0 || eye2s.size() > 0)
						{
							b1 = true;
						}
					}

					if (!b1 && len0 == 1)
					{
						rect = faces[i];
						temp = gray(rect).clone();
						equalizeHist(temp, temp);

						if (eye1s.size() == 0 && eye2s.size() == 0)
						{
							classifier_eye1.detectMultiScale(temp, eye1s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);
							classifier_eye2.detectMultiScale(temp, eye2s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);

						}
						else if (eye1s.size() == 0)
						{
							classifier_eye1.detectMultiScale(temp, eye1s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);

						}
						else if (eye2s.size() == 0)
						{
							classifier_eye2.detectMultiScale(temp, eye2s, scaleFactor, minNeighbors, CASCADE_DO_CANNY_PRUNING, eyeSize);

						}

						if (eye1s.size() > 0 && eye2s.size() > 0)
						{
							b1 = true;
						}

						if (len0 == 1)
						{
							if (eye1s.size() > 0 || eye2s.size() > 0)
							{
								b1 = true;
							}
						}
					}

					if (b1)
					{
						rect = faces[i];
						TRAIN_DTO dto;
						Mat dst;
						resize(frame(rect), dst, Size(128, 128), 0, 0, INTER_LINEAR);
						dto.ori = dst.clone();

						Mat dst2;
						resize(gray(rect), dst2, Size(128, 128), 0, 0, INTER_LINEAR);
						dto.gray = dst2.clone();

						dto.image_id = ids[j];
						dto.label = Int2Str(label * 10 + i);
						dto.al = "1";
						lstDto.push_back(dto);
					}
				}

				len0 = (int)lstDto.size();
				vector<_TRAIN_DTO>::iterator it;

				if (len0 == 1)
				{
					for (it = lstDto.begin(); it != lstDto.end(); it++)
					{
						lstTrain.push_back(*it);
					}
				}
				else
				{
					for (it = lstDto.begin(); it != lstDto.end(); it++)
					{
						string name = format("%s.jpg", it->label.c_str());
						wstring szDst2 = root + L"\\data1\\_train\\_temp\\" + _u2w(name);
						vector<uchar> buf0;
						imencode(".jpg", it->ori, buf0);

						ofstream file2(szDst2.c_str(), ios::out | ios::binary);
						if (file2)
						{
							file2.write((char*)&buf0[0], buf0.size() * sizeof(uchar));
							file2.close();
						}
					}
					mul++;
				}
			}
			else
			{
				Mat gray;
				cvtColor(frame, gray, CV_BGR2GRAY);

				TRAIN_DTO dto;
				dto.ori = frame.clone();
				dto.gray = gray.clone();
				dto.label = ids[j];
				dto.image_id = dto.label.substr(0, dto.label.length() - 1);
				dto.al = Int2Str((int)files.size());

				lstTrain.push_back(dto);
			}
		}
	}

	return 0;
}

PHOTOL_API int predict(int ty, const char* enckey, const char* trainlist_txt, char** mlabels, int& mul)
{
	string ret = "";

	wstring root = get_module_path(NULL);

	if (face_model == "")
	{
		face_model = _w2u(root + L"\\data1\\_train\\_model\\face_model.yml");
	}

	vector<TRAIN_DTO> lstTrain;
	getMatAndLabels(ty, root, enckey, trainlist_txt, lstTrain, mul);
	if (lstTrain.empty())
	{
		return 1;
	}

	int start = 0;
	bool b = false;
	if (_access(face_model.c_str(), _A_NORMAL) == -1)
	{
		model = LBPHFaceRecognizer::create();

		TRAIN_DTO dto = lstTrain[0];
		string name = format("%s.jpg", dto.label.c_str());

		wstring szDst2 = root + L"\\data1\\_train\\_face\\" + _u2w(name);
		vector<uchar> buf0;
		imencode(".jpg", dto.ori, buf0);

		ofstream file2(szDst2.c_str(), ios::out | ios::binary);
		if (file2)
		{
			file2.write((char*)&buf0[0], buf0.size() * sizeof(uchar));
			file2.close();

			vector<Mat> mats2;
			vector<int> labels2;
			mats2.push_back(dto.gray);
			labels2.push_back(atoi(dto.label.c_str()));

			b = true;
			model->train(mats2, labels2);

			ret += dto.image_id + "|" + dto.image_id + "|" + name + "|" + dto.al + "|997\r\n";
		}

		start = 1;
	}
	else
	{
		if (model == nullptr)
		{
			model = Algorithm::load<LBPHFaceRecognizer>(face_model);
		}

		if (model->empty())
		{
			return 1;
		}
	}
	vector<Mat> mats0;
	vector<int> labels0;

	for (int i = start; i < lstTrain.size(); i++)
	{
		string image_id = lstTrain[i].image_id;

		int label = -1;
		double confidence = 0;
		model->predict(lstTrain[i].gray, label, confidence);

		if (confidence < 80)
		{
			string name = format("%d.jpg", label);
			ret += image_id + "|" + Int2Str(label) + "|" + name + "|" + lstTrain[i].al + "|" + Int2Str(confidence) + "\r\n";
		}
		else
		{
			string label0 = lstTrain[i].label;
			label = atoi(label0.c_str());

			string name = format("%s.jpg", label0.c_str());
			wstring szDst2 = root + L"\\data1\\_train\\_face\\" + _u2w(name);
			vector<uchar> buf0;
			imencode(".jpg", lstTrain[i].ori, buf0);

			ofstream file2(szDst2.c_str(), ios::out | ios::binary);
			if (file2)
			{
				file2.write((char*)&buf0[0], buf0.size() * sizeof(uchar));
				file2.close();

				if (ty == 1)
				{
					vector<Mat> mats2;
					vector<int> labels2;
					mats2.push_back(lstTrain[i].gray);
					labels2.push_back(label);
					model->update(mats2, labels2);
				}
				else
				{
					mats0.push_back(lstTrain[i].gray);
					labels0.push_back(label);
				}
				b = true;

				ret += image_id + "|" + image_id + "|" + name + "|" + lstTrain[i].al + "|997\r\n";
			}
		}

	}

	if (!mats0.empty())
	{
		model->update(mats0, labels0);
	}

	if (b)
	{
		model->save(face_model);
	}

	int len2 = (int)ret.size() + 1;
	*mlabels = (char*)malloc(len2);
	memset(*mlabels, 0, len2);
	strcpy_s(*mlabels, len2, ret.c_str());

	return 0;
}
