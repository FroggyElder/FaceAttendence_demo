#include "MainWindow.h"
#include <QApplication>

#include "ElaApplication.h"

#include "opencv.hpp"

// void dbInit ();

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat> ("cv::Mat&");
    qRegisterMetaType<cv::Mat> ("cv::Mat");
    qRegisterMetaType<int64_t> ("int64_t");

    eApp->init();

    MainWindow w;
    w.show();
    return a.exec();
}


