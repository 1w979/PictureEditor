#include"ImgProc.h"

#include <qstring.h>
Point prevPt(-1, -1);
Mat img;
Mat inpaintMask;//声明Mat对象的蒙版进行修补
String windowName;
Mat img_mask;
// onMouse函数用于鼠标处理
//用于绘制需要修复的区域
void onMouse(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON))
		prevPt = Point(-1, -1);
	else if (event == EVENT_LBUTTONDOWN)
		prevPt = Point(x, y);
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		Point pt(x, y);
		if (prevPt.x < 0)
			prevPt = pt;
		line(inpaintMask, prevPt, pt, Scalar::all(255), 5, 8, 0);
		line(img, prevPt, pt, Scalar::all(255), 5, 8, 0);
		prevPt = pt;
		imshow(windowName, img);
	}
}
void ImgProc::ImgRepair(Mat image, string winName)
{

	img = image.clone();
	namedWindow(winName, WINDOW_AUTOSIZE);

	// 为原始图像创建一个副本
	img_mask = img.clone();
	//初始化遮罩（黑色图像）
	inpaintMask = Mat::zeros(img_mask.size(), CV_8U);
	imshow(winName, img);
	windowName = winName;
	setMouseCallback(winName, onMouse, NULL);
	while (waitKey(1) != 27 && !ImgRepairIsEnd)
	{
		imshow(winName, img);
	}
	destroyWindow(winName);
}

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

	while (!embossIsEnd)
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
		if (waitKey(1) == 27) //如果按q则关闭窗口
		{
			
			break;
		}
		
	}
	destroyAllWindows();
}
void ImgProc::imgRepairReset(Mat& imgRep)
{
	inpaintMask = Scalar::all(0);
	img_mask.copyTo(img);
	imshow(windowName, img);
}
void ImgProc::imgRepairFFM(Mat& imgRep, Mat& result)
{
	inpaint(imgRep, inpaintMask, result, 3, INPAINT_TELEA);
	imshow("使用FMM的Inpaint输出", result);


}
void ImgProc::imgRepairNS(Mat& imgRep, Mat& result)
{
	//使用Bertalmio等人提出的算法。（NS技术）
	inpaint(imgRep, inpaintMask, result, 3, INPAINT_NS);
	imshow("使用NS技术的Inpaint输出", result);
		

}
