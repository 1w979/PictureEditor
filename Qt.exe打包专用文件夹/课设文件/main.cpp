#include "OpenCVQT.h"
#include <QtWidgets/QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OpenCVQT w;
    w.show();
    string fileName;
    return a.exec();
}
