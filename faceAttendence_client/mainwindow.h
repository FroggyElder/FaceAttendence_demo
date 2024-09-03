#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv.hpp>

#include <QTimer>
#include "QTcpSocket"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTimer m_cameraTimer{};
    cv::VideoCapture m_cap{};
    cv::CascadeClassifier m_casade{};

    QTcpSocket m_tcpSocket{};
    QTimer m_tcpTimer{};

    cv::Mat m_avatarMat{};

private slots:
    void capImg();

    void timerConnect();
    void rcvMsg();

};
#endif // MAINWINDOW_H
