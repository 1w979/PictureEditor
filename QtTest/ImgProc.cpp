#include"ImgProc.h"

#include <qstring.h>

void emptyCall(int x, void* data)
{
} //空的滑动条回调函数
void ImgProc::emboss(Mat img, Mat& result)
{
	//TODO:有一个奇怪的BUG，如果窗口flags设置为0，窗口就会被拉伸，如果使用默认即1，窗口就会莫名的大，滚动条也莫名的长

	namedWindow("浮雕",0);
	int slider = 0;
	int slider2 = 0;
	//创建滑动条
	createTrackbar("kernelSize", "浮雕", &slider, 8, emptyCall); //内核的大小,越大浮雕效果越明显
	createTrackbar("kernelType", "浮雕", &slider2, 3, emptyCall); //内核的类型,0 1 2 3分别对应左下，右下，右上，左上四种

	while (true)
	{
		//根据滑动条获取对应的值
		int size = getTrackbarPos("kernelSize", "浮雕");
		size += 2; //由于内核的最小规模为2x2，因此size+=2
		SetEmbossKernel(size); //根据size设置内核
		int s = getTrackbarPos("kernelType", "浮雕");
		int height = img.size().height;
		int width = img.size().width;
		Mat y = Mat(height, width, CV_8U, Scalar(128)); //创建一个灰色的蒙版
		Mat gray;
		cvtColor(img, gray, COLOR_BGR2GRAY);


		for (int i = 0; i < s; i++) //通过旋转获得不同类型的内核
			rotate(embossKernel, embossKernel, ROTATE_90_COUNTERCLOCKWISE);
		Mat dst;
		filter2D(gray, dst, -1, embossKernel); //使用定义好的内核进行

		add(dst, y, result); //得到结果
		//显示原图和带滑动条的效果图

		imshow("浮雕", result);
		Mat empty;
		if (waitKey(1) == 27) //如果按q则输出结果图并关闭窗口
		{
			result = empty.clone();
			break;
		}
		if (embossIsEnd)
		{
			break;
		}
	}
	destroyAllWindows();
}
