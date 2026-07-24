///////////////////////////////////////////////////////////////////////
/// @file photol.h
/// @brief PhotoNest 图像处理 DLL 导出接口头文件,PhotoLExports接口
/// @details 定义图像处理 DLL 的导出 API,包括:
///           - 图像缩放 (resize_img)
///           - 图像尺寸获取 (img_size)
///           - 图像质量评估 BRISQUE (img_brisque)
///           - 视频捕获 MP4/GIF (capture_mp4, capture_gif)
///           - MP3 封面提取 (mp3_cover)
///           - 图像绘制显示 (paint_show)
///           - 加密密钥设置 (set_key)
/// @author PhotoNest Team
/// @date 2024
/// @version 1.0
/// @note 本文件是 PhotoNest 图像处理模块的核心接口
///////////////////////////////////////////////////////////////////////

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PHOTOL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MFTHUMB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#define PHOTOL_EXPORTS

#ifdef PHOTOL_EXPORTS
#define PHOTOL_API __declspec(dllexport)
#else
#define PHOTOL_API __declspec(dllimport)
#endif

PHOTOL_API int resize_img(const char* ext, const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height, uint32_t& level);
PHOTOL_API int img_size(const wchar_t* szSrc, uint32_t& width, uint32_t& height);
PHOTOL_API int img_brisque(const wchar_t* szSrc, const char* enckey, uint32_t& level);
PHOTOL_API int capture_mp4(const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height, uint32_t& duration);
PHOTOL_API int capture_gif(const wchar_t* szSrc, const wchar_t* szDst, uint32_t& width, uint32_t& height);
PHOTOL_API int mp3_cover(int ty, const wchar_t* szSrc, const wchar_t* szDst, long& duration, bool& cover);
PHOTOL_API int paint_show(int type, uint64_t hwnd, int pixelsy, int w, int h, const wchar_t* fname, const wchar_t* langue, const wchar_t* user, const wchar_t* enckey, int ov);
PHOTOL_API int capture_ot(int type, const wchar_t* szDst);
PHOTOL_API int predict(int ty, const char* enckey, const char* trainlist_txt, char** mlabels);
PHOTOL_API int set_key(const char* enckey);

