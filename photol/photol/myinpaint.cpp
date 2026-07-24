#include "stdafx.h"
#include "myinpaint.h"
#include "ThreadPool.h"

myinpaint::myinpaint(void)
{
}

myinpaint::~myinpaint(void)
{

}

//返回最小差值（最相似）的块的代表数字，用于后续判断
//GetMinPatch1的简化版
int myinpaint::GetMinPatch21(Mat Dst, Mat Src1, Mat Src2)
{
	float dist1 = Distance1(Dst, Src1);
	float dist2 = Distance1(Dst, Src2);

	if (dist1 < dist2)
		return 1;
	else
		return 2;
}

//迭代第二步：随机搜索
//（row,col）=（now_row, now_col）：修复patch里的像素
void myinpaint::RandomSearch1(Mat Dst, Mat Src, Mat Mask, Mat& Off, int row, int col)
{
	//获取修复基准框，在框内操作
	Mat DstPatch = GetPatch1(Dst, row, col);

	//迭代指数
	int attenuate = 0;
	while (true)
	{
		//获取随机参数，在 [-1;1] 间
		float divcol = rand() % 2000 / 1000.0f - 1.0f;
		float divrow = rand() % 2000 / 1000.0f - 1.0f;

		//减小框大小的公式，𝑢_𝑖=𝑣_0+𝑤*𝛼^𝑖*𝑅_𝑖
		//行列分别处理，MaxWindow：原始框宽度；divcol：随机系数；pow(A,B):A的B次方。随迭代次数而变小的缩小系数；RandomAttenuation=0.5；
		float veccol = (float)(MaxWindow * pow(RandomAttenuation, attenuate) * divcol);
		float vecrow = (float)(MaxWindow * pow(RandomAttenuation, attenuate) * divrow);

		float length = sqrt(veccol * veccol + vecrow * vecrow);
		//如果低于1个像素，没有意义，直接结束整个循环，对下一个像素处理
		if (length < 1)
			break;

		//x方向,前2项指向(row, col)的match块，后面是公式的后一项
		int nowrow = (int)(row + Off.at < Vec2f >(row, col)[0] + vecrow);
		//y方向
		int nowcol = (int)(col + Off.at < Vec2f >(row, col)[1] + veccol);

		//判断随机搜索的patch不越界，在search内
		if (nowcol >= 0 && nowcol <= Off.cols - 1 && nowrow >= 0
			&& nowrow <= Off.rows - 1
			&& Mask.at < uchar >(nowrow, nowcol) == search
			&& abs(nowrow - row) < searchrowratio * Mask.rows)//abs：绝对值
		{

			//取出原来的match块
			Mat SrcPatch1 = GetPatch1(Src, (int)Off.at < Vec2f >(row, col)[0] + row,
				(int)Off.at < Vec2f >(row, col)[1] + col);
			//取出现在的随机match块
			Mat SrcPatch2 = GetPatch1(Src, nowrow, nowcol);

			//对比相似性，找出最好的块
			int location = GetMinPatch21(DstPatch, SrcPatch1, SrcPatch2);

			//结合最好的相似块给像素新的偏置值
			switch (location)
			{
			case 2:
				Off.at < Vec2f >(row, col)[1] = (float)(nowcol - col);
				Off.at < Vec2f >(row, col)[0] = (float)(nowrow - row);
				break;
				//这个break的含义在于，只要找到更好的块就跳出while循环。
			}
		}

		//迭代指数增加
		attenuate++;
	}
}

//返回最小差值（最相似）的块的代表数字，用于后续判断
int myinpaint::GetMinPatch11(Mat Dst, Mat Src1, Mat Src2, Mat Src3)
{
	//patch块与每一个match块对比
	float dist1 = Distance1(Dst, Src1);
	float dist2 = Distance1(Dst, Src2);
	float dist3 = Distance1(Dst, Src3);

	//返回最小差值（最相似）的块的代表数字，用于后续判断
	if (dist1 < dist2)
	{
		if (dist1 < dist3)
			return 1;
		else
			return 3;
	}
	else if (dist2 < dist3)
		return 2;
	else
		return 3;
}


//迭代第一步：传播
//（now_row, now_col）：patch里的像素
//odd：当前迭代次
void myinpaint::Propagation1(Mat Dst, Mat Src, Mat Mask, Mat& Off, int row, int col, int odd)
{
	//获取长度为 patchsize = 3的边界框, (row, col)代表的是中心像素点坐标
	Mat DstPatch = GetPatch1(Dst, row, col);

	if (odd % 2 == 0)//偶次迭代
	{
		//提取(row, col)的match块
		Mat SrcPatch = GetPatch1(Src, row + (int)Off.at < Vec2f >(row, col)[0],
			col + (int)Off.at < Vec2f >(row, col)[1]);

		//提取(row, col-1)的match块
		Mat LSrcPatch = GetPatch1(Src, row + (int)Off.at < Vec2f >(row, col - 1)[0],
			col - 1 + (int)Off.at < Vec2f >(row, col - 1)[1]);

		//提取(row-1, col)的match块
		Mat USrcPatch = GetPatch1(Src,
			row - 1 + (int)Off.at < Vec2f >(row - 1, col)[0],
			col + (int)Off.at < Vec2f >(row - 1, col)[1]);

		//返回上面4个块最相似的块的代表数字，用于switch判断
		int location = GetMinPatch11(DstPatch, SrcPatch, LSrcPatch, USrcPatch);

		//利用上面的信息更新像素点的偏置地图
		switch (location)
		{
		case 2:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row, col - 1)[0];
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row, col - 1)[1] - 1;
			//这里由于是将(row,col)处的Patch映射到(row+Off.at < Vec2f >(row, col - 1)[0],
			//col-1+Off.at < Vec2f >(row, col - 1)[1])处的Match,所以(row,col)处的偏置地图即为：
			//Off.at < Vec2f >(row, col - 1)[0]与Off.at < Vec2f >(row, col - 1)[1] - 1。
			break;
		case 3:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row - 1, col)[0] - 1;
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row - 1, col)[1];
			break;
		}
	}

	else//奇数次迭代
	{
		Mat SrcPatch = GetPatch1(Src, row + (int)Off.at < Vec2f >(row, col)[0],
			col + (int)Off.at < Vec2f >(row, col)[1]);
		Mat RSrcPatch = GetPatch1(Src, row + (int)Off.at < Vec2f >(row, col + 1)[0],
			col + 1 + (int)Off.at < Vec2f >(row, col + 1)[1]);
		Mat DSrcPatch = GetPatch1(Src,
			row + 1 + (int)Off.at < Vec2f >(row + 1, col)[0],
			col + (int)Off.at < Vec2f >(row + 1, col)[1]);

		int location = GetMinPatch11(DstPatch, SrcPatch, RSrcPatch, DSrcPatch);
		switch (location)
		{
		case 2:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row, col + 1)[0];
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f>(row, col + 1)[1] + 1;
			break;
		case 3:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f>(row + 1, col)[0] + 1;
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row + 1, col)[1];
			break;
		}
	}
}

//以块为单位，用所有像素点的相同颜色通道的差平方来简单判断相似度
float myinpaint::Distance1(Mat Dst, Mat Src)
{
	float distance = 0;

	for (int i = 0; i < Dst.rows; i++)
	{
		for (int j = 0; j < Dst.cols; j++)
		{
			for (int k = 0; k < 3; k++)//K=3个颜色通道
			{
				distance += abs(Src.at < Vec3b >(i, j)[k] - Dst.at < Vec3b >(i, j)[k]);
			}
		}
	}

	return distance;
}


//获取长度为 patchsize=3 的边界框,(row,col)代表的是中心像素点坐标，考虑了像素点在边缘上的情况
//这个框就是修复的标准框
Mat myinpaint::GetPatch1(Mat Src, int row, int col)
{
	//注意：图像最后一行或最后一列的行数或列数是Src.rows - 1/Src.cols - 1，不要搞混了
	int row_begin = row - (PatchSize / 2) >= 0 ? row - (PatchSize / 2) : 0;
	int row_end =
		row + (PatchSize / 2) <= Src.rows - 1 ?
		row + (PatchSize / 2) : Src.rows - 1;

	int col_begin = col - (PatchSize / 2) >= 0 ? col - (PatchSize / 2) : 0;
	int col_end =
		col + (PatchSize / 2) <= Src.cols - 1 ?
		col + (PatchSize / 2) : Src.cols - 1;

	return Src(Range(row_begin, row_end + 1), Range(col_begin, col_end + 1));
	//opencv 提供的边界确定函数
	//其中，Range(row_begin, row_end + 1)意为提取Src中的row_begin行至row_end + 1行，不包括row_end + 1行。
}

//具体如何修复像素？修复像素=原图像素+偏置像素
Vec3b myinpaint::GeneratePixel21(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col)
{
	return Src.at < Vec3b>(row + (int)Off.at < Vec2f >(row, col)[0], col + (int)Off.at < Vec2f>(row, col)[1]);
}

//修复当前像素
void myinpaint::GenerateImage21(Mat& Dst, Mat Src, Mat Mask, Mat Off)
{
	for (int i = 0; i < Dst.rows; i++)
		for (int j = 0; j < Dst.cols; j++)
			if (Mask.at < uchar >(i, j) == hole)
				Dst.at < Vec3b >(i, j) = GeneratePixel21(Dst, Src, Mask, Off, i, j);
}

//此函数是修复主函数调用的修复子函数
//（row,col）-在Dst_img中的扫描的像素点
//odd-当前迭代的次数
Vec3b myinpaint::GeneratePixel1(Mat Dst, Mat Src, Mat Mask, Mat Off, int row,
	int col, int odd, int idx)
{
	//获取坐标（row,col）为中心的长度为 patchsize 的边界框
	//这里值得一提的是，扫描的坐标和GetPatch的坐标一个在左上方，一个在中心
	Mat DstSize = GetPatch1(Dst, row, col);

	int window_length = DstSize.rows * DstSize.cols; //返回得到的Patch中的元素个数。

													 //初始化缓存数组，这些数组是用来存储patch里的图像的，用的是指针的存储方法
													 //语句声明a是float型指针，并将其初始化为指向一个具有 window_length 个 float型元素数组的首地址
	float* a = new float[window_length];
	float* sim = new float[window_length];
	float* w = new float[window_length];
	Vec3b* c = new Vec3b[window_length];
	int nowlength = 0;

	//确定比例系数
	float ratio =
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) > 150 ?
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) / 150 : 1;

	//偶数次迭代，扫描右下的patch
	if (odd % 2 == 0)
	{
		//以patch为单位，PatchSize=3
		for (int i = 0; i < PatchSize; i++)
		{
			for (int j = 0; j < PatchSize; j++)
			{
				//逐个扫描以（row,col）为中心，宽为patchsize的patch里，每一个像素（now_row，now_col）
				int now_row = row - (PatchSize / 2) + i;
				int now_col = col - (PatchSize / 2) + j;

				//考虑到在patch里的像素点可能会在边界上，或者在back0上，跳过
				if (now_row < PatchSize / 2 || now_col < PatchSize / 2
					|| now_row > Dst.rows - 1 - PatchSize / 2
					|| now_col > Dst.cols - 1 - PatchSize / 2
					|| Mask.at < uchar >(now_row, now_col) == back0)
				{
					continue;//跳出当前像素循环
				}

				//传播
				Propagation1(Dst, Src, Mask, Off, now_row, now_col, odd);//（now_row, now_col）：patch里的像素
																		 //随机扰动搜索
				RandomSearch1(Dst, Src, Mask, Off, now_row, now_col);

				//提取偏置图参数，还原参考的块，用于跟更新后的块对比
				int patch_row = now_row + (int)Off.at < Vec2f >(now_row, now_col)[0];
				int patch_col = now_col + (int)Off.at < Vec2f >(now_row, now_col)[1];

				//提取更新后块，原偏置块
				Mat DstPatch = GetPatch1(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch1(Src, patch_row, patch_col);

				//c
				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);
				//按照距离该点的距离算
				a[nowlength] = (float)(sqrt((now_row - patch_row) * (now_row - patch_row) * 1.0 + (now_col - patch_col) * (now_col - patch_col) * 1.0) / ratio);

				//通过得到2个块的差平方的值，判断的相似度
				sim[nowlength] = Distance1(SrcPatch, DstPatch);

				//简单自增记录循环次数
				nowlength++;
			}
		}

		float delta = GetDelta1(sim, nowlength);

		if (delta == 0)
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i]);//e的-sim[i]次方
				a[i] = pow(1.3f, -a[i]);//pow(a,b) 结果是a的b次方.
				w[i] = a[i] * sim[i];
			}
		else
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i] / (2 * delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
	}


	//奇数次迭代，扫描 左上 的patch
	else
	{
		for (int i = PatchSize - 1; i >= 0; i--)
		{
			for (int j = PatchSize - 1; j >= 0; j--)
			{
				int now_row = row - (PatchSize / 2) + i;
				int now_col = col - (PatchSize / 2) + j;

				if (now_row < PatchSize / 2 || now_col < PatchSize / 2
					|| now_row > Dst.rows - 1 - PatchSize / 2
					|| now_col > Dst.cols - 1 - PatchSize / 2
					|| Mask.at < uchar >(now_row, now_col) == back0)
				{
					continue;
				}

				Propagation1(Dst, Src, Mask, Off, now_row, now_col, odd);
				RandomSearch1(Dst, Src, Mask, Off, now_row, now_col);

				int patch_row = now_row + (int)Off.at < Vec2f >(now_row, now_col)[0];
				int patch_col = now_col + (int)Off.at < Vec2f >(now_row, now_col)[1];

				Mat DstPatch = GetPatch1(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch1(Src, patch_row, patch_col);

				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);
				//按照距离该点的距离算
				a[nowlength] = (float)(sqrt((now_row - patch_row) * (now_row - patch_row) * 1.0 + (now_col - patch_col) * (now_col - patch_col) * 1.0) / ratio);
				sim[nowlength] = Distance1(SrcPatch, DstPatch);
				nowlength++;
			}
		}

		float delta = GetDelta1(sim, nowlength);

		if (delta == 0)
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i]);
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
		else
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i] / (2 * delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
	}

	delete[] a;
	delete[] sim;
	Vec3b v = GetMeanshift1(w, c, nowlength, odd);
	delete[] w;
	delete[] c;

	return v;
}

/*
UINT __stdcall myinpaint::handle_GeneratePixel1(LPVOID pParam) {
generate_struct * st = (generate_struct*)pParam;
pThis->set_pixel1(st->Dst, st->Src, st->Mask, st->Off, st->i, st->j, st->odd);

return 0;
}


void myinpaint::set_pixel1(Mat &Dst, Mat Src, Mat Mask, Mat Off, int row,int col, int odd)
{
Dst.at < Vec3b >(i, j) = GeneratePixel1(Dst, Src, Mask, Off,
i, j, odd);

}
*/


//此函数是修复的主函数。
//为了避免收敛的问题，采用偶次，奇数迭代次数不同的搜索方向；
//odd ：当前迭代的次数
//其实无论是偶数次还是奇数次是往右下，最重要是匹配一点：右下遍历修复，找的块一定往左上找，反之亦然
void myinpaint::GenerateImage1(Mat& Dst, Mat Src, Mat Mask, Mat Off, int odd, int idx)
{

	if (odd % 2 == 1)   //奇次数迭代，遍历 右下 的像素点
	{
		for (int i = 0; i < Dst.rows; i++)
		{
			for (int j = 0; j < Dst.cols; j++)
			{
				if (Mask.at < uchar >(i, j) == hole)//确定（i，j）为破损像素
				{
					Dst.at < Vec3b >(i, j) = GeneratePixel1(Dst, Src, Mask, Off,
						i, j, odd, idx);
				}
			}
		}
	}
	else    //偶次数迭代,遍历左上的像素点
	{
		for (int i = Dst.rows - 1; i >= 0; i--)
		{
			for (int j = Dst.cols - 1; j >= 0; j--)
			{
				if (Mask.at < uchar >(i, j) == hole)
				{
					Dst.at < Vec3b >(i, j) = GeneratePixel1(Dst, Src, Mask, Off,
						i, j, odd, idx);
				}
			}
		}
	}
}

//把随机搜索的所有块按照相似性 从头到尾从大到小排序
float myinpaint::GetDelta1(float* sim, int length)
{
	//简单copy，使得下面的排序过程对sim不产生改变
	float* dist = new float[length];
	for (int i = 0; i < length; i++)
		dist[i] = sim[i];

	//冒泡排序,最后的序列是：从大到小
	for (int i = 0; i < length - 1; i++)
	{
		for (int j = 0; j < length - 1 - i; j++)
		{
			if (dist[j] > dist[j + 1])
			{
				float tmp = dist[j + 1];
				dist[j + 1] = dist[j];
				dist[j] = tmp;
			}
		}
	}

	float f = dist[(int)(length * 0.75)];

	delete[] dist;
	return f;
}


//求解出向量c中元素的平方差和。
float myinpaint::GetWindowSize1(Vec3b* c, int length)
{
	float allb = 0, allg = 0, allr = 0;
	for (int i = 0; i < length; i++)
	{
		allb += c[i][0];
		allg += c[i][1];
		allr += c[i][2];
	}
	allb = allb / length;
	allg = allg / length;
	allr = allr / length;

	float delta = 0;

	for (int i = 0; i < length; i++)
	{
		delta +=
			(allb - c[i][0]) * (allb - c[i][0]) +
			(allg - c[i][1]) * (allg - c[i][1]) +
			(allr - c[i][2]) * (allr - c[i][2]);
	}

	return delta;
}

Vec3b myinpaint::GetMeanshift1(float* w, Vec3b* c, int now_length, int odd)
{
	float delta = GetWindowSize1(c, now_length);
	float iter = 1;

	Vec3b centre;
	if (odd % 2 == 0)
		centre = c[0];
	else
		centre = c[now_length - 1];

	float* w1 = w;
	Vec3b* c1 = c;
	int length1 = now_length;

	float* w2 = new float[length1];
	Vec3b* c2 = new Vec3b[length1];

	while (true)
	{
		int length2 = 0;

		for (int i = 0; i < length1; i++)
		{
			if ((c1[i][0] - centre[0]) * (c1[i][0] - centre[0])
				+ (c1[i][1] - centre[1]) * (c1[i][1] - centre[1])
				+ (c1[i][2] - centre[2]) * (c1[i][2] - centre[2])
				< iter * iter * delta)
			{
				w2[length2] = w1[i];
				c2[length2] = c1[i];
				length2++;
			}
		}

		if (length2 == 0)
			break;

		float allb = 0, allg = 0, allr = 0;

		for (int i = 0; i < length2; i++)
		{
			allb += c2[i][0] - centre[0];
			allg += c2[i][1] - centre[1];
			allr += c2[i][2] - centre[2];
		}

		allb = allb / length2;
		allg = allg / length2;
		allr = allr / length2;

		Vec3b newcentre = Vec3b((uchar)(allb + centre[0]), (uchar)(allg + (int)centre[1]), (uchar)(allr + centre[2]));

		c1 = c2;
		w1 = w2;
		length1 = length2;

		if ((newcentre[0] - centre[0]) * (newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0]) * (newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0]) * (newcentre[0] - centre[0])
			< 1)
			break;
		else
			centre = newcentre;


	}


	float allp = 0, allb = 0, allg = 0, allr = 0;

	for (int i = 0; i < length1; i++)
	{
		allp += w1[i];
		allb += ((int)c1[i][0]) * w1[i];
		allg += ((int)c1[i][1]) * w1[i];
		allr += ((int)c1[i][2]) * w1[i];
	}

	delete[] w2;
	delete[] c2;

	return Vec3b((uchar)(allb / allp), (uchar)(allg / allp), (uchar)(allr / allp));
}

//进一步把search部分分为：边缘的不可进入black区域 和 可进入search部分
//这个是可以理解的，这个是为了不让搜索的框超出原图边缘
void myinpaint::InitMask1(Mat Mask, Mat& Mask2)
{
	Mask2 = Mask.clone();

	//将边界处的search设为back0
	for (int i = 0; i < Mask.rows; i++)
	{
		for (int j = 0; j < Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == search)
			{
				//获取了以（i，j）像素为中心，半径为patchsize/2的边界框
				Mat MaskPatch = GetPatch1(Mask, i, j);

				//如果以（i，j）为中心的块长宽不跟设定的完整块长宽一致，
				//那么证明（i，j）恰好位于图像边缘内（宽为patchsize/2），把search标记为back0，
				if (MaskPatch.rows * MaskPatch.cols != PatchSize * PatchSize)
				{
					Mask2.at<uchar>(i, j) = back0;
				}

			}
		}
	}
}

//初始化偏置图像
void myinpaint::InitOff1(Mat Mask, Mat& Off)
{
	//为方便起见，将所有的都附上，要求不能赋值到非搜索区域
	//初始化格式
	//2维无符号32位精度浮点数
	Off = Mat(Mask.size(), CV_32FC2, Scalar::all(0));

	for (int i = 0; i < Mask.rows; i++)
	{
		for (int j = 0; j < Mask.cols; j++)
		{
			//不考虑search区域，没有破损，他们的最佳偏移向量当然是0，自己
			if (Mask.at<uchar>(i, j) == search)
			{
				//<Vec2f> 向量，2维，浮点数
				Off.at<Vec2f>(i, j)[0] = 0;
				Off.at<Vec2f>(i, j)[1] = 0;
			}
			else
			{
				//处理hole，采用随机偏置  (此时Mask中包含back0区域，所以这里初始化的是back0和hole两部分)
				//先初始化2个偏置数r_col,r_row
				int r_col = rand() % Mask.cols; //rand（）产生随机数，主要是产生一个偏置的初始值
				int r_row = rand() % Mask.rows;
				r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;//边界检测
				r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;

				//为什么要有这个循环？因为一次的随机赋值，很可能会出现偏置后的块跑到破损区域，或者是超出限定搜索框的边界
				//注意：这里与参考图像为完好图像不同，需要考虑Patch偏置后的Match不会偏置到除搜索区域以外的部分
				int k = 0;

				//这里加上I，j，是因为他是A投影到B中的搜索偏置
				//searchrowratio=0.5，搜索的时候,确保r_row偏置不会太远，一定是在原图像的大小里	
				while (
					!(Mask.at<uchar>(r_row + i, r_col + j) == search
						&& abs(r_row) < searchrowratio * Mask.rows))
				{
					r_col = rand() % Mask.cols;
					r_row = rand() % Mask.rows;

					//边界检测
					r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;
					r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;

					//liewen 避免死循环
					k++;
					if (k > 50)
					{
						break;
					}
				}

				//赋偏置值
				Off.at<Vec2f>(i, j)[0] = (float)r_row;
				Off.at<Vec2f>(i, j)[1] = (float)r_col;
			}
		}
	}
}

//降采样过程
void myinpaint::PyrDownMask1(Mat InM, Mat& OutM)//实参与形参
{
	//降采样后的行和列 是上层的一半
	//多给一行，防止“/”抹去有效信息，导致mask的行列与src等图像不同
	int rows = (InM.rows + 1) / 2;
	int cols = (InM.cols + 1) / 2;
	OutM = Mat(rows, cols, InM.type());

	//把降采样后的hole，back0，和search都对应好，标记优先级：hole>back0>search
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//假设是（0，0）
			int m = i;
			int n = j;

			int m2 = 2 * i;		//0
			int m21 = 2 * i + 1;//1

			int n2 = 2 * j;     //0
			int n21 = 2 * j + 1;//1

			//整个过程是以块的左上角为坐标原点，考察左上，左下，右上，右下四个像素的原标记种类
			//在图中心的情况
			//如果右下都在image里面，左上肯定也在里面

			if (m21 < InM.rows && n21 < InM.cols)
			{
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m2, n21) == hole
					|| InM.at<uchar>(m21, n2) == hole
					|| InM.at<uchar>(m21, n21) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back0
					|| InM.at<uchar>(m2, n21) == back0
					|| InM.at<uchar>(m21, n2) == back0
					|| InM.at<uchar>(m21, n21) == back0)
					OutM.at<uchar>(m, n) = back0;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else if (m21 < InM.rows //在边缘的情况
				&& n21 == InM.cols)  //还是要注意：图像边缘处的行列应该是InM.cols-1和InM.rows-1。
			{
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m21, n2) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back0
					|| InM.at<uchar>(m21, n2) == back0)
					OutM.at<uchar>(m, n) = back0;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else if (m21 == InM.rows//在边缘的情况
				&& n21 < InM.cols)
			{
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m2, n21) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back0
					|| InM.at<uchar>(m2, n21) == back0)
					OutM.at<uchar>(m, n) = back0;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else//在边缘的情况
			{
				if (InM.at<uchar>(m2, n2) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back0)
					OutM.at<uchar>(m, n) = back0;
				else
					OutM.at<uchar>(m, n) = search;
			}
		}
	}
}


//升采样过程
void myinpaint::PyrUpOff1(Mat InO, Mat InM, Mat& OutO)
{
	int rows = InM.rows;
	int cols = InM.cols;

	OutO = Mat(rows, cols, InO.type());

	//扫描输入图像
	for (int i = 0; i < InO.rows; i++)
	{
		for (int j = 0; j < InO.cols; j++)
		{
			int m = i;
			int n = j;

			int m2 = 2 * i;
			int m21 = 2 * i + 1;

			int n2 = 2 * j;
			int n21 = 2 * j + 1;

			if (m21 < OutO.rows
				&& n21 < OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m2, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else if (m21 < OutO.rows
				&& n21 == OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else if (m21 == OutO.rows
				&& n21 < OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m2, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
		}
	}
}

int myinpaint::inpaint(Mat Src, Mat inpaintMask, Mat& inpaintedImage, int idx, t_struct* myt)
{
	if (!myt->stop)
	{
		myt->step++;
	}
	//Mat Src = imread("d:/data/3.png");

	//初始化Mask掩码图(灰度图，初始值0) 和 Search 图像,
	Mat Mask = Mat(Src.size(), CV_8UC1, Scalar::all(0));
	Mat Search = Mat(Src.size(), CV_8UC3, Scalar::all(0));

	//初始化掩码图MASK(即原始图片上为空白的区域标记为待修复区域，Mask上同样位置像素灰度值记为255，非空白区域标记为
	//搜寻空间，Mask上同样位置灰度值标记为100)
	for (int i = 0; i < Mask.rows; i++)
	{
		for (int j = 0; j < Mask.cols; j++)
		{
			if (inpaintMask.at<uchar>(i, j) != 0)  //检测到空白处，标记为待修复区域
			{
				Mask.at<uchar>(i, j) = hole;
			}
			else
			{
				Mask.at<uchar>(i, j) = search;	//其他标记为搜寻空间
			}
		}
	}

	int ps = 3;//损像素修复窗口半径,即是patchsize  （原程序预设为3）
	int pyr = 5;//图像金字塔的层数
	int ann = 3;//最低最近最邻域迭代次数 anniteration

				//修复过程中，Mask0是修复的操作图像
				//把像素破损点扩充为块
	Mat Mask0 = Mask.clone();
	for (int i = 0; i < Mask.rows; i++)
	{
		for (int j = 0; j < Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == hole)//如果是破损像素
			{
				//扩充破损像素窗口为9*9
				int e_s = ps;//3*3         （原程序预设为3*ps）

							 //让这个窗口不超越图像边界
							 //x方向
				int r_b = i - e_s / 2 > 0 ? i - e_s / 2 : 0;   //A? B:C  若A为真，取B，否则取C
				int r_e = i + e_s / 2 < Mask.rows ? i + e_s / 2 : Mask.rows - 1;
				//y方向
				int c_b = j - e_s / 2 > 0 ? j - e_s / 2 : 0;
				int c_e = j + e_s / 2 < Mask.cols - 1 ? j + e_s / 2 : Mask.cols - 1;

				for (int k = r_b; k < r_e; k++)
				{
					for (int p = c_b; p < c_e; p++)
					{
						Mask0.at<uchar>(k, p) = hole;//没有加具体通道，默认全设置 hole=255（白色）
					}
				}
			}
		}
	}

	//初始化搜索图像(前面注释掉部分是将Mask矩阵中灰度值为255的像素在Search中对应部分变为255，这里是在将像素
	//破损点扩充为块变为Mask0以后将Mask0中灰度值为255的像素在Search中对应部分变为255)
	Search.setTo(0);//MAT search 的值全部设置成0
	for (int i = 0; i < Mask.rows; i++)
	{
		for (int j = 0; j < Mask.cols; j++)
		{
			if (Mask0.at<uchar>(i, j) == search)
			{
				Search.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
			}
		}
	}

	//imshow("src", Src);
	//imshow("mask", Mask);
	//imshow("Mask0", Mask0);
	//imshow("search", Search);
	if (!myt->stop)
	{
		myt->step++;
	}

	srand((unsigned)time(NULL));//因为是伪随机函数，需要时刻变化的时间来作为随机函数srand的初始化种子

	SrcImg = Src.clone();
	DstImg = Src.clone();
	MaskImg = Mask.clone();
	//OffsetImg用于存储映射关系，两通道分别存取x方向的偏移和y方向的偏移。
	OffsetImg = Mat(MaskImg.size(), CV_32FC2, Scalar::all(0));
	show = Mat::zeros(DstImg.size(), CV_8UC3);
	PatchSize = ps;
	//在randomsearch函数里面重新设置
	MaxWindow = -1;
	MinWindow = 7 * PatchSize;
	//金字塔级数
	PryIteration = pyr;
	//最近最邻域搜索次数
	ANNIteration = ann;
	//随机衰减
	RandomAttenuation = 0.4f;
	searchrowratio = 0.6;//意思是：搜索的时候会只在以search为中心， 上下一共2*ratio里面搜索

						 //图像数组，缓存迭代结果（格外注意一下这种操作方式），虽然数组定义包含30个元素，但是实际上只用到了前5个
	Mat Srcs[30];
	Mat Dsts[30];
	Mat Masks[30];
	Mat Masks2[30];
	Mat Offs[30];

	Srcs[0] = SrcImg;//原始图像
	Dsts[0] = DstImg;//输出结果图像
	Masks[0] = MaskImg;//掩模图像
	Offs[0] = OffsetImg;//偏置图像

						//进一步把search部分分为：边缘的不可进入black区域 和 可进入search部分
	InitMask1(Masks[0], Masks2[0]);

	int i = 0;//金字塔层级计数

	//std::cout << "PyrDown Step " << i << std::endl;

	//降采样4种类型窗口
	while (i < PryIteration				//层数设为5
		&& Dsts[i].rows > MinWindow  	//金字塔最底层的图像尺寸（长宽）不能比设定的最小像素长度 7*patchsize=21 还小
		&& Dsts[i].cols > MinWindow)
	{
		//std::cout << "PyrDown Step " << i + 1 << std::endl;

		pyrDown(Srcs[i], Srcs[i + 1]);//opencv提供的降采样函数(具体实现降采样的操作过程见书OpenCV中224页所述)
		pyrDown(Dsts[i], Dsts[i + 1]);

		PyrDownMask1(Masks[i], Masks[i + 1]);//对掩码矩阵降采样，这个没法用自带的，因为降采样的时候，会用高斯内核卷积一次，这样就会破坏hole等的值了

		InitMask1(Masks[i + 1], Masks2[i + 1]);//对下个层级的掩码矩阵分化为black和search

		i++;
	}
	if (!myt->stop)
	{
		myt->step++;
	}

	//std::cout << std::endl;

	//初始化偏置地图
	InitOff1(Masks2[i], Offs[i]); //i为5时初始化此偏置图像，i为5时对应分辨率最低的掩模图像Masks2

								  //在完成初始化后的图像中执行↓
								  //patchmatch循环算法 ———— 传播，随机搜索
	while (i >= 0)
	{
		std::cout << "PyrUp Step " << i << std::endl;

		//记录当前图片最大的宽度
		MaxWindow = max(Srcs[i].rows, Srcs[i].cols);

		//最开始，很粗略地用初始偏置地图在像素级别上来修复像素
		GenerateImage21(Dsts[i], Srcs[i], Masks2[i], Offs[i]);

		//show是展示的图像的函数
		show.setTo(0);
		resize(Dsts[i], show, DstImg.size());//resize把图像调整到一定大小，（input，output，size）
											 //imshow("show", show);
											 //waitKey(100);
											 //waitKey(1);

											 //修复当前金字塔层级图像，ANNIteration：基础迭代次数3次；扩充次数：ANNIteration + i * 5
											 //在粗糙的级别上，多搜索几次。
		for (int j = 0; j < i * 5 + 1; j++)
		{
			int odd = j;
			if (!myt->stop)
			{
				myt->step++;
			}

			if (i < 2)
			{
				if (!myt->stop)
				{
					myt->step++;
				}
			}

			//std::cout << "ANN " << j << " start" << std::endl;

			//修复图像入口函数GenerateImage
			//这时候也仅仅是完成了所有前期的参数的初始化工作
			GenerateImage1(Dsts[i], Srcs[i], Masks2[i], Offs[i], odd, idx);
			std::cout << "ANN " << j << " end" << std::endl;

			show.setTo(0);
			resize(Dsts[i], show, DstImg.size());
			//imshow("show", show);
			//waitKey(100);
			//waitKey(1);
		}

		inpaintedImage = show.clone();

		//将偏置图升采样
		if (i > 0)
		{
			PyrUpOff1(Offs[i], Masks2[i - 1], Offs[i - 1]);
		}

		//std::cout << std::endl;

		i--;
	}
	std::cout << "ok" << std::endl;

	//waitKey();
	return 0;
}
