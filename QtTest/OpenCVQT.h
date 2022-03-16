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
    Mat alignResult;

    Mat ipImg;

    Mat Input;
    Mat Output;

    QString OpenFilePath;
    QString fileNameCopy;
    QString fileName;

    TouDraw td;

    bool isAVI = false;
    int choiceNote = 0;

public slots:
    //浮雕相关UI回调函数

    //老电视相关UI回调函数

    //图片对齐相关UI回调函数

    void AlignLoadSampleButtonDown();

    //骰子
    void TdDraw();

    void OpenImg();
    void InputCodecForName(char* inname);
    void OutputCodecForName(char* outname, Mat resultchooce);

    void EmbossButtonDown();

    void OldTVButtonDown();

    void AlignButtonDown();

    void SaveButtonDown();
    void SaveFile(Mat resultChoose, int choice);
};
