#ifndef SHOWHEADIMGPANEL_H
#define SHOWHEADIMGPANEL_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QLabel>
#include "faceinfo.h"

#include <QThread>


class showHeadImgPanel : public QWidget
{
    Q_OBJECT
public:
    explicit showHeadImgPanel(QWidget *parent = nullptr);
    ~showHeadImgPanel();

signals:
    void sig_query(cv::Mat &faceImg);

private:
    QTcpServer* m_tcpServer{new QTcpServer};
    QTcpSocket* m_tcpSocket{};

    QLabel m_avatarLabel{this};
    void avatarLabelInit ();

    quint64 m_rcvSize{};

    FaceInfo* m_faceInfo{FaceInfo::instance()};
    QThread* m_queryThread {new QThread{}};

private slots:
    void onClientConnect();
    void onSocketReadyRead();
    void readQuery(int64_t id);
};

#endif // SHOWHEADIMGPANEL_H
