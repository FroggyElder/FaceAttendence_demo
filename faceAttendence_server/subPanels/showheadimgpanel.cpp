#include "showheadimgpanel.h"
#include <QLabel>
#include <QDebug>
#include <QDataStream>
#include <QPixmap>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>

showHeadImgPanel::showHeadImgPanel(QWidget *parent)
    : QWidget{parent}
{
    //set up avatar label
    avatarLabelInit();

    //start listening
    m_tcpServer->listen(QHostAddress::Any,9999);
    connect(m_tcpServer,&QTcpServer::newConnection,this,&showHeadImgPanel::onClientConnect);

    m_faceInfo->moveToThread(m_queryThread);
    m_queryThread->start();

    connect (this,&showHeadImgPanel::sig_query,m_faceInfo,&FaceInfo::faceQuery);
    connect (m_faceInfo,&FaceInfo::faceQueried,this,&showHeadImgPanel::readQuery);
}

showHeadImgPanel::~showHeadImgPanel()
{
    delete m_tcpServer;
    delete m_tcpSocket;
}

void showHeadImgPanel::avatarLabelInit()
{
    m_avatarLabel.setGeometry(0,0,480,480);
    m_avatarLabel.setPixmap(QPixmap(":/assets/avatarPlaceHolder.png").scaled(400,400));
    m_avatarLabel.show();
}

//client connected -> set up socket and connect slot
void showHeadImgPanel::onClientConnect()
{
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&showHeadImgPanel::onSocketReadyRead);
}

void showHeadImgPanel::readQuery(int64_t id)
{
    qDebug() << "id:" << id;
    if (id<0)
    {
        QString sndMsg =
            QString("{\"StaffID\":\"-1\",\"Name\":\"%2\",\"Dep\":\"emb\",\"AttendTime\":\"%4\"}");

        m_tcpSocket->write(sndMsg.toUtf8());
        return;
    }

    // Prepare the SQL query to find the record with the matching FaceID
    QSqlQuery query;
    query.prepare("SELECT StaffID, Name FROM staffInfo WHERE FaceID = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.next()) {
            // Retrieve the data from the record
            static QDateTime curDT{QDateTime::currentDateTime()-std::chrono::seconds(50)};

            if (QDateTime::currentDateTime()-curDT < std::chrono::seconds(10)) return;

            curDT = QDateTime::currentDateTime();

            QString staffID = query.value("StaffID").toString();
            QString name = query.value("Name").toString();
            QString attendTime = curDT.toString("yyyy-MM-dd hh:mm:ss");

            // Create the JSON string to send
            QString sndMsg = QString("{\"StaffID\":\"%1\",\"Name\":\"%2\",\"Dep\":\"emb\",\"AttendTime\":\"%3\"}")
                                 .arg(staffID)
                                 .arg(name)
                                 .arg(attendTime);

            qDebug() << sndMsg;


            QSqlQuery attendQuery {};
            attendQuery.prepare("INSERT INTO attendLog (StaffID,Name) VALUES(:id,:nm)");
            attendQuery.bindValue(":id",staffID);
            attendQuery.bindValue(":nm",name);
            attendQuery.exec();

            if (attendQuery.lastError().isValid())
            {
                qDebug () << "attendQuery error:" << attendQuery.lastError().text();
                QString sndMsg = QString("{\"StaffID\":\"-1\",\"Name\":\"\",\"Dep\":\"\",\"AttendTime\":\"\"}");
                m_tcpSocket->write(sndMsg.toUtf8());
            }
            else
            {
                // Send the message if no error
                m_tcpSocket->write(sndMsg.toUtf8());
            }

        } else {
            // If no record was found, send a default message
            QString sndMsg = QString("{\"StaffID\":\"-1\",\"Name\":\"\",\"Dep\":\"\",\"AttendTime\":\"\"}");
            m_tcpSocket->write(sndMsg.toUtf8());
        }
    } else {
        qDebug() << "Query failed:" << query.lastError().text();
        // Handle query failure with a default message
        QString sndMsg = QString("{\"StaffID\":\"-1\",\"Name\":\"\",\"Dep\":\"emb\",\"AttendTime\":\"\"}");
        m_tcpSocket->write(sndMsg.toUtf8());
    }

}

//read to read ->
void showHeadImgPanel::onSocketReadyRead()
{
    QDataStream stream{m_tcpSocket};
    stream.setVersion(QDataStream::Qt_5_7);

    if (!m_rcvSize)
    {
        if (m_tcpSocket->bytesAvailable()<static_cast<qint64>(sizeof(m_rcvSize))) return;
        stream >> m_rcvSize;
    }
    if (m_tcpSocket->bytesAvailable()<static_cast<qint64>(m_rcvSize)) return;

    QByteArray rcvData{};
    stream >> rcvData;
    if (!rcvData.size()) return;

    m_rcvSize = 0;

    //show img
    QPixmap mmp{};
    mmp.loadFromData(rcvData,"jpg");
    mmp.scaled(m_avatarLabel.size());
    m_avatarLabel.setPixmap(mmp);

    //get the Mat image
    std::vector<uchar> decode{};
    decode.resize(rcvData.size());
    memcpy(decode.data(),rcvData.data(),rcvData.size());
    cv::Mat faceImage{cv::imdecode(decode,cv::ImreadModes::IMREAD_COLOR)};


    emit sig_query(faceImage);
}
