/* myinpaint.h
 * the driver class.
 * inpaint.
 
 */
 
 


#ifndef _MYPAINT_H_
#define _MYPAINT_H_

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>


using namespace cv;


typedef struct {
	HANDLE hdl;
	bool stop;
	int contours1;
	int step;
	int opt;
} t_struct;

#define back0 0
#define search 100
#define hole 255

class myinpaint
{
public:
	myinpaint(void);
	~myinpaint(void);

public:
	int inpaint(Mat Src, Mat inpaintMask, Mat &inpaintedImage, int idx, t_struct *myt);

private:
	float GetWindowSize1(Vec3b* c, int length);

	void InitMask1(Mat Mask, Mat& Mask2);
	void InitOff1(Mat Mask, Mat& Off);
	float Distance1(Mat Dst, Mat Src);
	Mat GetPatch1(Mat Src, int row, int col);
	float GetDelta1(float* sim, int length);
	Vec3b GetMeanshift1(float* w, Vec3b* c, int now_length, int odd);
	Vec3b GeneratePixel1(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col, int odd, int idx);
	void GenerateImage1(Mat& Dst, Mat Src, Mat Mask, Mat Off, int odd, int idx);
	Vec3b GeneratePixel21(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col);
	void GenerateImage21(Mat& Dst, Mat Src, Mat Mask, Mat Off);
	int GetMinPatch21(Mat Dst, Mat Src1, Mat Src2);
	void RandomSearch1(Mat Dst, Mat Src, Mat Mask, Mat& Off, int row, int col);
	int GetMinPatch11(Mat Dst, Mat Src1, Mat Src2, Mat Src3);
	void Propagation1(Mat Dst, Mat Src, Mat Mask, Mat& Off, int row, int col, int odd);
	void PyrDownMask1(Mat InM, Mat& OutM);
	void PyrUpOff1(Mat InO, Mat InM, Mat& OutO);

private:

	Mat DstImg;
	Mat SrcImg;
	Mat MaskImg;
	Mat OffsetImg;
	Mat show;
	int PatchSize;
	int MaxWindow;
	int MinWindow;
	int PryIteration;
	int ANNIteration;
	float RandomAttenuation;
	double searchrowratio;

};

#endif
