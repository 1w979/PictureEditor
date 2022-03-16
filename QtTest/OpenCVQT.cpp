#include "OpenCVQT.h"

OpenCVQT::OpenCVQT(QWidget *parent)
    : QMainWindow(parent)
{
	//加载UI
    ui.setupUi(this);
	
	QMessageBox::warning(this, "警告", "111！");

	connect(ui.loadButton, SIGNAL(clicked()), this, SLOT(OpenImg()));
	connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(SaveButtonDown()));
	connect(ui.embossButton, SIGNAL(clicked()), this, SLOT(EmbossButtonDown()));
	connect(ui.oldTVButton, SIGNAL(clicked()), this, SLOT(OldTVButtonDown()));
	connect(ui.alignButton, SIGNAL(clicked()), this, SLOT(AlignButtonDown()));
	connect(ui.tdDrawButton, SIGNAL(clicked()), this, SLOT(TdDraw()));
}

void OpenCVQT::OpenImg()
{
	QImage motoImg;

	fileName = QFileDialog::getOpenFileName(
		this, "打开图片","../",
		"Image File(*.jpg *.png *.bmp);;All(*.*)");

	if (fileName != "")
		if (motoImg.load(fileName))
			ui.label->setPixmap(QPixmap::fromImage(motoImg).scaled(ui.label->size()));

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "警告", "未成功选择图片！");
		return;
	}

	OpenFilePath = QFileInfo(fileName).filePath();
	ui.lineEdit->setText(OpenFilePath);

	InputCodecForName("gb2312");
}
void OpenCVQT::InputCodecForName(char* inname)
{
	QTextCodec* code = QTextCodec::codecForName(inname);
	string inputFile = code->fromUnicode(fileName).data();

	if (!fileName.isEmpty())
		ipImg = imread(inputFile);

	QMessageBox::warning(this, "警告", "0");
}
void OpenCVQT::OutputCodecForName(char* outname,Mat resultchooce)
{
	QTextCodec* code = QTextCodec::codecForName(outname);
	string outputFile = code->fromUnicode(fileName).data();
	imwrite(outputFile, resultchooce);
}
/////////////////////////////////////////////
void OpenCVQT::EmbossButtonDown()
{
	QMessageBox::warning(this, "警告", "可运行");

	choiceNote = 1;
	ip.embossIsEnd = false;


	if (!fileName.isEmpty())
		ip.emboss(ipImg, embossResult);
}
void OpenCVQT::OldTVButtonDown()
{
	choiceNote = 2;
	ip.oldTVIsEnd = false;

	if (!fileName.isEmpty())
		ip.oldTVPreview(ipImg);

}
void OpenCVQT::AlignButtonDown()
{
	choiceNote = 3;

	if (!ipImg.empty() && !alignSample.empty())
	{
		alignResult = ip.alignImages(alignSample, ipImg);
	}
}
/////////////////////////////////////////////

/////////////////////////////////////////////
void OpenCVQT::AlignLoadSampleButtonDown()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,
		tr("打开文件"),
		"D:/",
		tr("images(*.png *jpeg *jpg *bmp)"));
	if (fileName.isEmpty()) {
		QMessageBox::warning(this, "警告", "未成功选取图片!");
		return;
	}
	QTextCodec* code = QTextCodec::codecForName("gb2312");

	string inputFile = code->fromUnicode(fileName).data();
	alignSample = imread(inputFile);
	if (!ipImg.empty() && !alignSample.empty())
	{
		alignResult = ip.alignImages(alignSample, ipImg);
	}
}

void OpenCVQT::TdDraw()
{
	if (!fileName.isEmpty())
		td.run(fileName.toStdString());
}

void OpenCVQT::SaveButtonDown()
{
	fileNameCopy = fileName;

	switch (choiceNote)
	{
	case 1:
	{
		SaveFile(embossResult, 0);
		OutputCodecForName("gb2312", embossResult);

		ip.embossIsEnd = true;
		break;
	}
	case 2:
	{
		SaveFile(ipImg, 1);

		ip.oldTVIsEnd = true;
		Mat empty;
		ipImg = empty.clone();
		break;
	}
	case 3:
	{
		SaveFile(alignResult, 0);
		OutputCodecForName("gb2312", alignResult);

		destroyAllWindows();
		Mat empty;
		ipImg = empty.clone();
		alignSample = empty.clone();
		alignResult = empty.clone();
		break;
	}
	}
}
//两种保存算法：.avi & .jpg/.bmp/.png
void OpenCVQT::SaveFile(Mat resultChoose, int choice)
{

	switch (choice)
	{
	case 0:	//.jpg...
	{
		if (resultChoose.empty())
		{
			QMessageBox::warning(this, "警告", "请先读取文件!");
			return;
		}

		QString slcStr, fileSaveName;
		fileSaveName = QFileDialog::getSaveFileName(this, tr("保存结果"),
			"c:/untitled.png",
			tr("PNG(*.png);;JPG(*.jpg);;BMP(*.bmp)"), &slcStr);
		if (slcStr.left(3) == "JPG")
		{
			if (fileSaveName.right(4).toLower() != ".jpg")
			{
				fileSaveName += ".jpg";
			}
		}
		if (slcStr.left(3) == "PNG")
		{
			if (fileSaveName.right(4).toLower() != ".png")
			{
				fileSaveName += ".png";
			}
		}
		if (slcStr.left(3) == "BMP")
		{
			if (fileSaveName.right(4).toLower() != ".bmp")
			{
				fileSaveName += ".bmp";
			}
		}

		break;
	}
	case 1: //.avi
	{
		if (resultChoose.empty())
		{
			QMessageBox::warning(this, "警告", "请先读取文件!");
			return;
		}
		QString slcStr, fileSaveName;
		fileSaveName = QFileDialog::getSaveFileName(this, tr("保存结果"),
			"c:/untitled.png",
			tr("AVI(*.avi)"), &slcStr);
		if (slcStr.left(3) == "AVI")
		{
			if (fileSaveName.right(4).toLower() != ".avi")
			{
				fileSaveName += ".avi";
			}
		}

		QTextCodec* code = QTextCodec::codecForName("gb2312");
		string outputFile = code->fromUnicode(fileSaveName).data();
		ip.oldTVOutPut(resultChoose, outputFile);

		QMessageBox::warning(this, "123", fileSaveName);

		break;
	}
	default:
		QMessageBox::warning(this, "警告", "未选择正确的保存方式!");
		break;
	}
	fileName = fileNameCopy;

	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "警告", "未成功选择路径!");
		return;
	}

}