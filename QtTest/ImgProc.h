#include <iostream>
#include <string>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;


/*
 * ImgProc类3.0已实现/封装的功能：
 * oldTV：读取一张图片，通过随机生成噪声并逐帧写入的方式导出一个类似老电视效果的视频
 * emboss：读取一张图片，通过滑动条进行调节的浮雕效果，按q可以将对应的效果导出
 * alignImages：读取两张图片，第一张图片作为对照图，与第二张图通过ORB特征进行匹配，实现相近部分的识别。
 * 并通过findHomography获得映射矩阵，再进行透视变换获得对齐后的图片
 */
class ImgProc
{
public:
	bool embossIsEnd;
	bool oldTVIsEnd;

private:
	Mat embossKernel; //浮雕效果所使用的内核
	int maxFeatures; //最大检测的特征数
	float matchPercent; //保留的匹配结果比例
	void SetEmbossKernel(int size) //根据size设置对应的内核，这个函数获得的是左下内核
	{
		Mat kernel = Mat(size, size, CV_8S, Scalar(0));
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (i < j)
				{
					kernel.at<schar>(i, j) = -1;
				}
				else if (i > j)
				{
					kernel.at<schar>(i, j) = 1;
				}
			}
		}
		embossKernel = kernel;
	}

public:
	ImgProc(): maxFeatures(1000), matchPercent(0.15f),embossIsEnd(false),oldTVIsEnd(false)
	{
	}

	Mat alignImages(Mat& img2, Mat& img1)
	{
		Mat Gray1, im2Gray, h, result;
		cvtColor(img1, Gray1, COLOR_BGR2GRAY);
		cvtColor(img2, im2Gray, COLOR_BGR2GRAY);

		// 定义关键点和描述符
		vector<KeyPoint> keypoints1, keypoints2;
		Mat descriptors1, descriptors2;

		Ptr<Feature2D> orb = ORB::create(maxFeatures);
		orb->detectAndCompute(Gray1, Mat(), keypoints1, descriptors1);
		orb->detectAndCompute(im2Gray, Mat(), keypoints2, descriptors2);

		//匹配特征
		vector<DMatch> matches;
		Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming"); //使用汉明距离判断相似度
		matcher->match(descriptors1, descriptors2, matches, Mat());

		//根据相似度得分进行排序
		sort(matches.begin(), matches.end());

		//移除相似度较低的匹配结果
		const int numGoodMatches = matches.size() * matchPercent;
		matches.erase(matches.begin() + numGoodMatches, matches.end());

		//在图像中绘制相似度最高的部分匹配结果
		Mat imMatches;
		drawMatches(img1, keypoints1, img2, keypoints2, matches, imMatches);
		resize(imMatches, imMatches, Size(800, 800));
		imshow("Matches Preview", imMatches);

		// 取得这些匹配项的位置
		vector<Point2f> points1, points2;

		for (size_t i = 0; i < matches.size(); i++)
		{
			points1.push_back(keypoints1[matches[i].queryIdx].pt);
			points2.push_back(keypoints2[matches[i].trainIdx].pt);
		}

		// 根据特征点找到两个图象的映射矩阵
		h = findHomography(points1, points2, RANSAC);

		// 进行透视变换得到结果
		warpPerspective(img1, result, h, img2.size());
		return result;
	}

	void oldTVOutPut(Mat& img, string fileName,int w = 0, int h = 0) //通过随机生成颜色值不同的噪声，并逐帧导出生成视频文件，实现类似老电视的效果
	{
		
		VideoWriter vw;
		Size size;
		if (w > 0 && h > 0) //如果指定了输出视频大小则按照设置调整size大小
			size = Size(w, h);
		else
			size = img.size(); //如果未指定参数，则size为原图片大小
		//设置输出视频文件的参数：10fps，编码方式为MPEG-4,非彩色
		vw.open(fileName+"60output.avi", CV_FOURCC('D', 'I', 'V', 'X'), 10.0, size, false);

		int count = 30; //总共写入30帧
		while (count > 0)
		{
			count--;
			int height = img.size().height;
			int width = img.size().width;
			Mat gray;
			cvtColor(img, gray, COLOR_BGR2GRAY);
			float thresh = 26; //每个像素生成噪声的概率
			float val = 79; //噪声的明显程度
			for (int i = 0; i < height; i++) //随机生成噪声
			{
				for (int j = 0; j < width; j++)
				{
					if (rand() % 100 <= thresh) //小于thresh则通过调整像素值生成噪声
					{
						if (rand() % 2 == 0) //随机根据val的值增减像素值
							gray.at<uchar>(i, j) = std::min(gray.at<uchar>(i, j) + rand() % (static_cast<int>(val) + 1),
							                                255);
						else
							gray.at<uchar>(i, j) = std::max(gray.at<uchar>(i, j) - rand() % (static_cast<int>(val) + 1),
							                                0);
					}
				}
			}
			if (size != img.size())
				resize(gray, gray, Size(w, h)); //如果并非使用原图的size，则通过resize调整图片宽高
			vw.write(gray); //写入视频帧
		}
		vw.release(); //释放VideoWriter
	}

	void oldTVPreview(Mat& img, int w = 0, int h = 0) //通过随机生成颜色值不同的噪声，并逐帧导出生成视频文件，实现类似老电视的效果
	{
		Size size;
		if (w > 0 && h > 0) //如果指定了输出视频大小则按照设置调整size大小
			size = Size(w, h);
		else
			size = img.size(); //如果未指定参数，则size为原图片大小
		int height = img.size().height;
		int width = img.size().width;
		
		while (waitKey(1)!=27&&!oldTVIsEnd)
		{
			
			Mat gray;
			
			cvtColor(img, gray, COLOR_BGR2GRAY);
			float thresh = 26; //每个像素生成噪声的概率
			float val = 79; //随机噪声的大小
			for (int i = 0; i < height; i++) //随机生成噪声
			{
				for (int j = 0; j < width; j++)
				{
					if (rand() % 100 <= thresh) //小于thresh则通过调整像素值生成噪声
					{
						if (rand() % 2 == 0) //随机根据val的值增减像素值
							gray.at<uchar>(i, j) = std::min(gray.at<uchar>(i, j) + rand() % (static_cast<int>(val) + 1),
								255);
						else
							gray.at<uchar>(i, j) = std::max(gray.at<uchar>(i, j) - rand() % (static_cast<int>(val) + 1),
								0);
					}
				}
			}
			if (size != img.size())
				resize(gray, gray, Size(w, h)); //如果并非使用原图的size，则通过resize调整图片宽高
			imshow("Preview", gray);
		}
		destroyAllWindows();
	}
	void emboss(Mat img,Mat &result);
	
};

//骰子画类
class TouDraw
{
private:
	//图片
	Mat image;
	//图片行与列
	int imgCols;
	int imgRows;
	//灰度图
	Mat grayImg;
	//特征矩阵
	Mat imgFeature;
	//输出图
	Mat result;

	string inWin;
	string outWin;

	int pieceSize;
	int meanSize;

	//骰子画的特征数组
	const int Tou3x3[10][3][3] =
	{
		//0
		{
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		},
		//1
		{
			0, 0, 0,
			0, 1, 0,
			0, 0, 0
		},
		//2
		{
			0, 1, 0,
			0, 0, 0,
			0, 1, 0
		},
		//3
		{
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		},
		//4
		{
			0, 1, 0,
			1, 0, 1,
			0, 1, 0
		},
		//5
		{
			0, 1, 0,
			1, 1, 1,
			0, 1, 0
		},
		//6
		{
			1, 0, 1,
			1, 0, 1,
			1, 0, 1
		},
		//7
		{
			1, 0, 1,
			1, 1, 1,
			1, 0, 1
		},
		//8
		{
			1, 1, 1,
			1, 0, 1,
			1, 1, 1
		},
		//9
		{
			1, 1, 1,
			1, 1, 1,
			1, 1, 1
		}
	};
	const int Tou9x9[10][8][8] =
	{
		//0
		{
			0, 0, 0, 0, 0,0, 0, 0,
			0, 0, 0, 0, 0,0, 0, 0,
			0, 0, 0, 0, 0,0, 0, 0, 
			0, 0, 0, 0, 0,0, 0, 0,
			0, 0, 0, 0, 0,0, 0, 0, 
			0, 0, 0, 0, 0,0, 0, 0,
			0, 0, 0, 0, 0,0, 0, 0,
			0, 0, 0, 0, 0,0, 0, 0, 
		},
		//1
		{
			0,0,0,1,1,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,1,1,0,0,0
		},
		//2
		{
			0,0,0,1,1,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,1,1,0,0,0
		},
		//3
		{
			0,0,0,1,1,0,0,0,
			1,0,0,1,1,0,0,1,
			0,0,1,0,0,1,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,1,0,0,1,0,0,
			1,0,0,1,1,0,0,1,
			0,0,0,1,1,0,0,0
		},
		//4
		{
			0,0,0,1,1,0,0,0,
			1,0,0,1,1,0,0,1,
			0,0,1,0,0,1,0,0,
			1,1,0,0,0,0,1,1,
			1,1,0,0,0,0,1,1,
			0,0,1,0,0,1,0,0,
			1,0,0,1,1,0,0,1,
			0,0,0,1,1,0,0,0
		},
		//5
		{
			0,1,0,1,1,0,1,0,
			1,1,0,1,1,0,1,1,
			0,0,1,0,0,1,0,0,
			1,1,0,0,0,0,1,1,
			1,1,0,0,0,0,1,1,
			0,0,1,0,0,1,0,0,
			1,1,0,1,1,0,1,1,
			0,1,0,1,1,0,1,0
		},
		//6
		{
			1,1,0,1,1,0,1,1,
			1,1,0,1,1,0,1,1,
			0,0,1,0,0,1,0,0,
			1,1,0,1,1,0,1,1,
			1,1,0,1,1,0,1,1,
			0,0,1,0,0,1,0,0,
			1,1,0,1,1,0,1,1,
			1,1,0,1,1,0,1,1
		},
		//7
		{
			1,1,0,1,1,0,1,1,
			1,1,1,1,1,1,1,1,
			0,1,1,0,0,1,1,0,
			1,1,0,1,1,0,1,1,
			1,1,0,1,1,0,1,1,
			0,1,1,0,0,1,1,0,
			1,1,1,1,1,1,1,1,
			1,1,0,1,1,0,1,1
		},
		//8
		{
			1,1,0,1,1,0,1,1,
			1,1,1,1,1,1,1,1,
			0,1,1,1,1,1,1,0,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			0,1,1,1,1,1,1,0,
			1,1,1,1,1,1,1,1,
			1,1,0,1,1,0,1,1
		},
		//9
		{
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1

		}
	};

public:
	//构造函数
	TouDraw() : imgCols(0), imgRows(0), inWin("原图"), outWin("骰子图"), pieceSize(0), meanSize(0) {}

	//设置图片
	void setImage(string imng)
	{
		image = imread(imng);
	}

	void ShowImage()
	{
		cv::imshow(inWin, image);
	}

	void Copy(Mat& tmp)
	{
		image.copyTo(tmp);
	}

	//BGR转GRAY
	void bgr2gray()
	{
		cvtColor(image, grayImg, CV_BGR2GRAY);
	}

	//设置行列值
	void setColsAndRows()
	{
		imgCols = image.cols;
		imgRows = image.rows;
	}

	//初始化空间
	void initSize()
	{
		setColsAndRows();
		imFeSizeDecide();

		imgFeature.create(Size((imgCols + 15) / pieceSize, (imgRows + 15) / pieceSize), CV_8UC1);
		imgFeature = Scalar(0);

		result.create(Size(imgCols, imgRows), CV_8UC1);
		result = Scalar(0);
	}

	//计算特征矩阵
	void imFeSizeDecide()
	{
		if (imgRows < 2000 || imgCols < 2000)
			pieceSize = 3;
		else
			pieceSize = 9;

		meanSize = pieceSize * pieceSize;
	}

	void handle()
	{
		bgr2gray();

		//将平均值存入特征矩阵
		for (int i = 0; i < imgRows; i++)
			for (int j = 0; j < imgCols; j++)
				imgFeature.at<uchar>(i / pieceSize, j / pieceSize) += grayImg.at<uchar>(i, j) / meanSize;
		//用特征函数计算得到新特征矩阵
		for (int i = 0; i < imgRows / pieceSize; i++)
			for (int j = 0; j < imgCols / pieceSize; j++)
				imgFeature.at<uchar>(i, j) /= (255 / 10 + 1);
	}

	//输出图片
	void setOutImg()
	{
		int z = 0;
		int o = 0, p = 0;
		//映射输出矩阵
		for (int i = 0; i < imgRows - 2; i += pieceSize)
		{
			for (int j = 0; j < imgCols - 1; j++)
			{
				for (int k = 0; k < pieceSize; k++)
				{
					if (pieceSize == 3)
						result.at<uchar>(i + k, j) = Tou3x3[imgFeature.at<uchar>(o, p)][k][z];
					else
						result.at<uchar>(i + k, j) = Tou9x9[imgFeature.at<uchar>(o, p)][k][z];
				}

				if (z == pieceSize - 1)
				{
					z = 0;
					p++;
				}
				else
					z++;

			}
			o++;
			p = 0;
		}
		//阈值化
		threshold(result, result, 0, 255, THRESH_BINARY);
		imshow(outWin, result);
	}

	//运行
	void run(string imgname)
	{
		setImage(imgname);
		initSize();
		handle();
		setOutImg();
	}
};

