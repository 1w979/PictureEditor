#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_OpenCVQT.h"
#include<opencv2/opencv.hpp>
#include<QFileDialog>
#include<iostream>
#include"ImgProc.h"
#include"QMessageBox"
#include"QTextCodec"

using namespace cv;
using namespace std;

class OpenCVQT : public QMainWindow
{
    Q_OBJECT

public:
    OpenCVQT(QWidget* parent = Q_NULLPTR);

private:
    Ui::OpenCVQTClass ui;
    Mat embossResult;
    ImgProc ip;
    Mat alignSample;
    Mat alignTarget;
    Mat alignResult;
    Mat ImageRepairResult;
    Mat ipImg;
    string outputFile;
    Mat empty;
    QString OpenFilePath;
    QString fileNameCopy;
    QString fileName;

    TouDraw td;

    bool isAVI = false;
    int choiceNote = 0;

public slots:
    //浮雕相关UI回调函数
    void EmbossButtonDown();

    //老电视相关UI回调函数
    void OldTVButtonDown();

    //图片对齐相关UI回调函数
    void AlignButtonDown();
    void AlignLoadSample();
    void AlignLoadTarget();

    //骰子
    void TdDraw();

    //文件流
    void outPut(string saveName, Mat output);
    void SaveButtonDown();
    void SaveFile(Mat resultChoose, int choice);
    void OpenImg();
    void InputCodecForName(char* codeName);

	//图片修复
    void ImageRepairButtonDown();
    void ImageRepair_FFM_ButtonDown();
    void ImageRepair_NS_ButtonDown();
    void ImageRepair_Reset_ButtonDown();
};
