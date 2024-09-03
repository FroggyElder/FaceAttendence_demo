#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "conditional_defines.h"

#include <QDebug>
#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include <QNetworkProxy>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());

    ui->reconSuccessLabel->hide();

    //set up camera
    m_cap.open(__CAMERA_OPEN);
    m_cameraTimer.setInterval(__CAMERA_INTERVAL);
    m_cameraTimer.start();
    connect(&m_cameraTimer,&QTimer::timeout,this,&MainWindow::capImg);
    //load sasade
    m_casade.load(__CAS_PATH);

    //set up tcp timer
    m_tcpTimer.setInterval(5000);
    connect(&m_tcpTimer,&QTimer::timeout,this,&MainWindow::timerConnect);
    m_tcpTimer.start();

    //connect tcp socket slots (reconnect if disconnected)
    m_tcpSocket.setProxy(QNetworkProxy::NoProxy);
    connect(&m_tcpSocket,&QTcpSocket::disconnected,this,[this]()
    {
        qDebug() << "server disconnected";
        m_tcpTimer.start();
    });
    connect(&m_tcpSocket,&QTcpSocket::connected,this,[this]()
    {
        qDebug() << "server connected";
        m_tcpTimer.stop();
    });
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&MainWindow::rcvMsg);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::rcvMsg()
{
    QByteArray msg =  m_tcpSocket.readAll();

    QJsonParseError err{};
    QJsonDocument doc {QJsonDocument::fromJson(msg, &err)};

    if (err.error != QJsonParseError::NoError)
    {
        qDebug() << "json error:" << err.errorString();
        return;
    }

    QJsonObject obj = doc.object();
    QString staffID {obj.value("StaffID").toString()};
    QString name {obj.value("Name").toString()};
    QString dep {obj.value("Dep").toString()};
    QString attendTime {obj.value("AttendTime").toString()};

    if (staffID.toInt()<0) return;

    ui->idShow->setText(staffID);
    ui->nameShow->setText(name);
    ui->idShow->setText(staffID);
    ui->depShow->setText(dep);
    ui->timeShow->setText(attendTime);

    if(m_avatarMat.empty()) return;
    static QImage aratar_QImg{};
    aratar_QImg = QImage{m_avatarMat.data,m_avatarMat.cols,m_avatarMat.rows,static_cast<int>(m_avatarMat.step1()),QImage::Format_RGB888};
    QPixmap avatar_QPixMap {QPixmap::fromImage(aratar_QImg).scaled(ui->avatarLabel->width(),ui->avatarLabel->height())};
    ui->avatarLabel->setPixmap(avatar_QPixMap);
    ui->reconSuccessLabel->show();

    QTimer::singleShot(2000,ui->reconSuccessLabel,&QLabel::hide);
}


void MainWindow::capImg()
{
    static cv::Mat srcImg{};
    if (!m_cap.grab())
    {
        throw "cap grab error";
        return;
    }
    m_cap.read(srcImg);
    if (!srcImg.data)
    {
        throw "img broken";
        return;
    }
    cv::resize(srcImg,srcImg,cv::Size(ui->faceShow->width(),ui->faceShow->height()));

    //from Mat(BGR) to QPixmap(RGB)
    cv::cvtColor(srcImg,srcImg,cv::COLOR_BGR2RGB);

    static QImage tmp_Qimg{};
    tmp_Qimg = QImage{srcImg.data,srcImg.cols,srcImg.rows,static_cast<int>(srcImg.step1()),QImage::Format_RGB888};

    QPixmap tmp_Qmap {QPixmap::fromImage(tmp_Qimg)};
    ui->faceShow->setPixmap(tmp_Qmap);

    static cv::Mat grayImg{};
    cv::cvtColor(srcImg,grayImg,cv::COLOR_BGR2GRAY);
    static std::vector<cv::Rect> faceRects{};
    m_casade.detectMultiScale(grayImg,faceRects,1.1,3,0,cv::Size(150,150));

    static int opt_flg{0};
    if (opt_flg>=0) {
        if (!faceRects.empty())
        {
            auto rect = faceRects.at(0);

            //move and show it
            ui->halo->move(rect.x,rect.y);
            ui->halo->show();

            if (opt_flg>=2&&m_tcpSocket.isWritable()) {
                // Convert QImage to QByteArray using QBuffer
                QByteArray imageData;
                QBuffer buffer(&imageData);
                buffer.open(QIODevice::WriteOnly);
                tmp_Qimg.save(&buffer, "JPG");  // Save as JPEG format
                buffer.close();

                QByteArray sendData{};
                quint64 dataSize = imageData.size();
                QDataStream stream{&sendData,QIODevice::WriteOnly};
                stream.setVersion(QDataStream::Qt_5_7);
                stream << dataSize << imageData;
                m_tcpSocket.write(sendData);

                m_avatarMat = srcImg(rect);

                opt_flg = -2;

            }
        }
        else
        {
            //just hide it
            ui->halo->hide();
        }
    }
    ++opt_flg;
}

void MainWindow::timerConnect()
{
    //connect to server
    qDebug() << "connecting to server";
    m_tcpSocket.connectToHost("192.168.172.113",9999);

}
