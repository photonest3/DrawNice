#pragma once

#include <opencv2/opencv.hpp>
#include "lang2.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "ThreadPool.h"
#include <algorithm>
#include <functional>
#include "myinpaint.h"


using namespace cv;
using namespace std;



class GMM
{
public:
	static const int componentsCount = 5;

	GMM(Mat& _model);
	double operator()(const Vec3d color) const;
	double operator()(int ci, const Vec3d color) const;
	int whichComponent(const Vec3d color) const;

	void initLearning();
	void addSample(int ci, const Vec3d color);
	void endLearning();

private:
	void calcInverseCovAndDeterm(int ci, double singularFix);
	Mat model;
	double* coefs;
	double* mean;
	double* cov;

	double inverseCovs[componentsCount][3][3];
	double covDeterms[componentsCount];

	double sums[componentsCount][3];
	double prods[componentsCount][3][3];
	int sampleCounts[componentsCount];
	int totalSampleCount;
};

