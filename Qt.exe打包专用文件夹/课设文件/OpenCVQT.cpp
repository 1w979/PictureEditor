#include "OpenCVQT.h"

OpenCVQT::OpenCVQT(QWidget *parent)
    : QMainWindow(parent)
{
	//加载UI
    ui.setupUi(this);
	choiceNote = 0;

	connect(ui.loadButton, SIGNAL(clicked()), this, SLOT(OpenImg()));
	connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(SaveButtonDown()));
	connect(ui.embossButton, SIGNAL(clicked()), this, SLOT(EmbossButtonDown()));
	connect(ui.oldTVButton, SIGNAL(clicked()), this, SLOT(OldTVButtonDown()));
	connect(ui.alignButton, SIGNAL(clicked()), this, SLOT(AlignButtonDown()));
	connect(ui.tdDrawButton, SIGNAL(clicked()), this, SLOT(TdDraw()));
	connect(ui.alignLoad1, SIGNAL(clicked()), this, SLOT(AlignLoadSample()));
	connect(ui.alignLoad2, SIGNAL(clicked()), this, SLOT(AlignLoadTarget()));
	connect(ui.repairBtn, SIGNAL(clicked()), this, SLOT(ImageRepairButtonDown()));
	connect(ui.repairFFM, SIGNAL(clicked()), this, SLOT(ImageRepair_FFM_ButtonDown()));
	connect(ui.repairNS, SIGNAL(clicked()), this, SLOT(ImageRepair_NS_ButtonDown()));
	connect(ui.repairReset, SIGNAL(clicked()), this, SLOT(ImageRepair_Reset_ButtonDown()));

	ui.repairFFM->setVisible(false);
	ui.repairNS->setVisible(false);

	ui.repairReset->setVisible(false);

	ui.alignLoad1->setVisible(false);
	ui.alignLoad2->setVisible(false);

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
void OpenCVQT::InputCodecForName(char* codeName)
{
	QTextCodec* code = QTextCodec::codecForName(codeName);
	string inputFile = code->fromUnicode(fileName).data();

	if (!fileName.isEmpty())
		ipImg = imread(inputFile);

}
void OpenCVQT::outPut(string saveName, Mat output)
{
	
	imwrite(saveName, output);
}


/////////////////////////////////////////////
void OpenCVQT::EmbossButtonDown()
{
	ip.oldTVIsEnd = true;
	ip.embossIsEnd = false;

	ui.repairFFM->setVisible(false);
	ui.repairNS->setVisible(false);

	ui.repairReset->setVisible(false);

	ui.alignLoad1->setVisible(false);
	ui.alignLoad2->setVisible(false);
	if (ipImg.empty())
	{
		QMessageBox::warning(this, "警告", "请先读取图片!");
		return;

	}
	choiceNote = 1;

	
	ip.emboss(ipImg, embossResult);
}
void OpenCVQT::OldTVButtonDown()
{
	ip.embossIsEnd = true;
	ip.oldTVIsEnd = false;

	ui.repairFFM->setVisible(false);
	ui.repairNS->setVisible(false);

	ui.repairReset->setVisible(false);

	ui.alignLoad1->setVisible(false);
	ui.alignLoad2->setVisible(false);
	if (ipImg.empty())
	{
		QMessageBox::warning(this, "警告", "请先读取图片!");
		return;

	}
	choiceNote = 2;
	

	ip.oldTVPreview(ipImg);

}
void OpenCVQT::AlignButtonDown()
{
	ip.oldTVIsEnd = true;
	ui.repairFFM->setVisible(false);
	ui.repairNS->setVisible(false);

	ui.repairReset->setVisible(false);
	choiceNote = 3;
	ui.alignLoad1->setVisible(true);
	ui.alignLoad2->setVisible(true);
	if (!ipImg.empty() && !alignSample.empty())
	{
		alignResult = ip.alignImages(alignSample, ipImg);
	}
}
/////////////////////////////////////////////

/////////////////////////////////////////////
void OpenCVQT::AlignLoadSample()
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
	if (!alignTarget.empty() && !alignSample.empty())
	{
		alignResult = ip.alignImages(alignSample, alignTarget);
	}
}
void OpenCVQT::AlignLoadTarget()
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
	alignTarget = imread(inputFile);
	if (!alignTarget.empty() && !alignSample.empty())
	{
		alignResult = ip.alignImages(alignSample, alignTarget);
	}
}
void OpenCVQT::TdDraw()
{
	ip.oldTVIsEnd = true;

	ui.repairFFM->setVisible(false);
	ui.repairNS->setVisible(false);

	ui.repairReset->setVisible(false);

	ui.alignLoad1->setVisible(false);
	ui.alignLoad2->setVisible(false);
	if (ipImg.empty())
	{
		QMessageBox::warning(this, "警告", "请先读取图片!");
		return;
	}
	choiceNote = 4;

	
	td.run(ipImg);
}

void OpenCVQT::SaveButtonDown()
{

	
	switch (choiceNote)
	{
	case 0:
		if (ipImg.empty())
		{
			QMessageBox::warning(this, "警告", "请先读取文件!");
			return;
		}

		SaveFile(ipImg, 0);
		
		outPut(outputFile, ipImg);

		break;

	case 1:
	
		SaveFile(embossResult, 0);
		
		outPut(outputFile, embossResult);

		ip.embossIsEnd = true;
		choiceNote = 0;
		destroyAllWindows();
		break;
	
	case 2:
	
		SaveFile(ipImg, 1);

		ip.oldTVIsEnd = true;

		choiceNote = 0;

		break;
	
	case 3:
	
		SaveFile(alignResult, 0);
		outPut(outputFile, alignResult);
		destroyAllWindows();
		
		choiceNote = 0;

		alignSample = empty.clone();
		alignTarget = empty.clone();
		alignResult = empty.clone();

		break;
	
	case 4:
		SaveFile(td.result, 0);
		outPut(outputFile, td.result);
		destroyAllWindows();
		choiceNote = 0;

		break;
	case 5:
		SaveFile(ImageRepairResult, 0);
		outPut(outputFile, ImageRepairResult);
		ip.ImgRepairIsEnd = true;
		destroyAllWindows();
		choiceNote = 0;
		ui.repairFFM->setVisible(false);
		ui.repairNS->setVisible(false);
		ui.repairReset->setVisible(false);
		ImageRepairResult = empty.clone();
		break;
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
			"D:/Untitled.png",
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
		QTextCodec* code = QTextCodec::codecForName("gb2312");
		outputFile = code->fromUnicode(fileSaveName).data();
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
			"D:/untitled.avi",
			tr("AVI(*.avi)"), &slcStr);
		if (slcStr.left(3) == "AVI")
		{
			if (fileSaveName.right(4).toLower() != ".avi")
			{
				fileSaveName += ".avi";
			}
		}

		QTextCodec* code = QTextCodec::codecForName("gb2312");
		outputFile = code->fromUnicode(fileSaveName).data();
		ip.oldTVOutPut(resultChoose, outputFile);
		destroyWindow("Preview");

		break;
	}
	default:	
		QMessageBox::warning(this, "警告", "未选择正确的保存方式!");
		break;
	}

	if (outputFile.empty())
	{
		QMessageBox::warning(this, "警告", "未成功选择路径!");
		return;
	}

}
/////////////////////////
void OpenCVQT::ImageRepairButtonDown()
{
	ip.oldTVIsEnd = true;
	ip.embossIsEnd = true;
	if (ipImg.empty())
	{
		QMessageBox::warning(this, "警告", "请先读取图片!");
		return;
	}
	choiceNote = 5;
	ip.ImgRepairIsEnd = false;
	
	ui.repairFFM->setVisible(true);
	ui.repairNS->setVisible(true);

	ui.repairReset->setVisible(true);
	ip.ImgRepair(ipImg, "FormerPhoto");
}

//FFM技术修复
void OpenCVQT::ImageRepair_FFM_ButtonDown()
{
	ip.imgRepairFFM(ipImg, ImageRepairResult);
}
//NS技术修复
void OpenCVQT::ImageRepair_NS_ButtonDown()
{
	ip.imgRepairNS(ipImg, ImageRepairResult);
}
//重置原图
void OpenCVQT::ImageRepair_Reset_ButtonDown()
{
	ip.imgRepairReset(ipImg);
}
