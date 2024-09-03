#include "registerpanel.h"
#include "../dbPathDefines.h"

#include "ElaLineEdit.h"
#include "ElaText.h"
#include "ElaComboBox.h"
#include "ElaPushButton.h"

#include <QDateEdit>
#include <QFileDialog>
#include <QImageCapture>
#include <QEvent>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <ElaMessageBar.h>
#include <QSqlTableModel>
#include <QSqlRecord>

#include "faceinfo.h"

registerPanel::registerPanel(QWidget *parent)
    : QWidget{parent}
{
    dbInit ();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // Initialize input widgets and add them to the layout
    QGridLayout *inputLayout = new QGridLayout();
    inputWidgetInit(inputLayout);

    // Create a widget to hold camera-related controls
    QWidget *cameraWidget = new QWidget(this);
    QVBoxLayout *cameraLayout = new QVBoxLayout(cameraWidget);
    cameraWidgetInit(cameraLayout);

    // Add input fields and camera controls to the main layout
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(cameraWidget);

    setLayout(mainLayout);
}

registerPanel::~registerPanel()
{
    m_db->close();
    delete m_db;
}

void registerPanel::cameraWidgetInit(QVBoxLayout *layout)
{
    m_avatarShow = new QLabel{this};
    m_avatarShow->setPixmap(QPixmap(":/assets/avatarPlaceHolder.png").scaled(400,400));
    layout->addWidget(m_avatarShow);

    m_imgPathEdit = new ElaLineEdit{this};
    m_imgPathEdit->setEnabled(false);
    layout->addWidget(m_imgPathEdit);

    m_setImgPathBtn = new ElaPushButton{"open", this};
    layout->addWidget(m_setImgPathBtn);
    connect (m_setImgPathBtn, &ElaPushButton::clicked, this, [=]() {
        QString filePath {QFileDialog::getOpenFileName(this, "open avatar img", "./", "img files(*.jpg *.png)")};
        if (filePath.isEmpty()) return;
        m_imgPathEdit->setText(filePath);
        m_avatarShow->setPixmap(QPixmap{filePath}.scaled(m_avatarShow->width(), m_avatarShow->height()));
        if (m_videoWidget) m_videoWidget->hide();
    });

    m_startCameraBtn = new ElaPushButton{"start", this};
    layout->addWidget(m_startCameraBtn);
    connect (m_startCameraBtn, &ElaPushButton::clicked, this, [=]() {
        ElaMessageBar::success(ElaMessageBarType::BottomLeft,"","camera opened",0);

        SAFE_DELETE(m_camera);
        SAFE_DELETE(m_captureSession);
        SAFE_DELETE(m_videoWidget);
        SAFE_DELETE(m_imageCapture);
        SAFE_DELETE(m_captureImage);

        m_setImgPathBtn->setEnabled(false);
        m_startCameraBtn->setEnabled(false);

        m_camera = new QCamera(this);
        m_captureSession = new QMediaCaptureSession(this);
        m_captureSession->setCamera(m_camera);

        m_videoWidget = new QVideoWidget(m_avatarShow);
        m_videoWidget->setGeometry(0,0,m_avatarShow->width(),m_avatarShow->height());
        m_captureSession->setVideoOutput(m_videoWidget);

        m_imageCapture = new QImageCapture{this};
        m_captureSession->setImageCapture(m_imageCapture);
        connect (m_imageCapture, &QImageCapture::imageCaptured, this, [=](int, const QImage &preview) {
            m_captureImage = new QImage(preview);
            qDebug() << m_captureImage;
            SAFE_DELETE(m_camera);
        });

        m_camera->start();
        m_videoWidget->show();
        m_captureBtn->setEnabled(true);
    });

    m_captureBtn = new ElaPushButton{"capture", this};
    m_captureBtn->setEnabled(false);
    layout->addWidget(m_captureBtn);
    connect (m_captureBtn, &ElaPushButton::clicked, this, [=]() {
        m_imageCapture->capture();

        QString savePath{QFileDialog::getSaveFileName(this, "save img", "./", "img (*.png)")};
        if (!savePath.isEmpty()) {
            m_imgPathEdit->setText(savePath);
            m_captureImage->save(savePath);
            delete m_captureImage;
            m_captureImage = nullptr;
        } else {
            emit m_startCameraBtn->clicked();
            return;
        }

        m_startCameraBtn->setEnabled(true);
        m_setImgPathBtn->setEnabled(true);
        m_captureBtn->setEnabled(false);
    });
}

void registerPanel::resizeEvent(QResizeEvent *event)
{
    if (m_videoWidget) {
        delete m_videoWidget;
        // Adjust the geometry of m_videoWidget to match m_avatarShow's size
        m_videoWidget = new QVideoWidget(m_avatarShow);
        m_videoWidget->setGeometry(0,0,m_avatarShow->width(),m_avatarShow->height());
        // Set the video output to the video widget
        m_captureSession->setVideoOutput(m_videoWidget);
        m_videoWidget->show();
    }
    // Call the base class implementation
    QWidget::resizeEvent(event);
}

void registerPanel::inputWidgetInit(QGridLayout *layout)
{
    // Name field
    ElaText* nameText{new ElaText{"Name:", this}};
    nameText->setTextPixelSize(20);
    layout->addWidget(nameText, 0, 0);
    m_nameEdit = new ElaLineEdit{this};
    layout->addWidget(m_nameEdit, 0, 1);

    // Sex field
    ElaText* sexText{new ElaText{"Sex:", this}};
    sexText->setTextPixelSize(20);
    layout->addWidget(sexText, 1, 0);
    m_sexEdit = new ElaComboBox{this};
    m_sexEdit->addItem("Male");
    m_sexEdit->addItem("Female");
    layout->addWidget(m_sexEdit, 1, 1);

    // Birthday field
    ElaText* birthdayText{new ElaText{"Birthday:", this}};
    birthdayText->setTextPixelSize(20);
    layout->addWidget(birthdayText, 2, 0);
    m_birthdayEdit = new QDateEdit{this};
    m_birthdayEdit->setDate(QDate::currentDate());
    m_birthdayEdit->setDisplayFormat("yyyy/MM/dd");
    layout->addWidget(m_birthdayEdit, 2, 1);

    // Address field
    ElaText* addressText{new ElaText{"Address:", this}};
    addressText->setTextPixelSize(20);
    layout->addWidget(addressText, 3, 0);
    m_addressEdit = new ElaLineEdit{this};
    layout->addWidget(m_addressEdit, 3, 1);

    // PhoneNum field
    ElaText* phoneNumText{new ElaText{"PhoneNum:", this}};
    phoneNumText->setTextPixelSize(20);
    layout->addWidget(phoneNumText, 4, 0);
    m_phoneNumEdit = new ElaLineEdit{this};
    layout->addWidget(m_phoneNumEdit, 4, 1);

    // Register button
    ElaPushButton* registerBtn{new ElaPushButton{"Register", this}};
    layout->addWidget(registerBtn, 5, 0, 1, 2);
    connect (registerBtn,&ElaPushButton::clicked,this,&registerPanel::registerFace);

    // Reset button
    ElaPushButton* resetBtn{new ElaPushButton{"Reset", this}};
    layout->addWidget(resetBtn, 6, 0, 1, 2);
    connect(resetBtn, &ElaPushButton::clicked, this, [=]() {
        // Reset all input fields
        m_nameEdit->clear();
        m_sexEdit->setCurrentIndex(0);  // Reset to first option
        m_birthdayEdit->setDate(QDate::currentDate());
        m_addressEdit->clear();
        m_phoneNumEdit->clear();
    });
}

void registerPanel::registerFace()
{
    //get face id
    cv::Mat mat{cv::imread(m_imgPathEdit->text().toStdString())};
    if (mat.empty()) return;

    FaceInfo* info{FaceInfo::instance()};
    int faceID = info->faceRegister(mat);
    qDebug () << "id:" << faceID;

    //create new img
    QString newImg = QString("E:/qt_workspace/projects/faceAttendence_server/avatars/%1.png").arg(m_nameEdit->text());
    m_imgPathEdit->setText(newImg);
    cv::imwrite(newImg.toUtf8().data(),mat);

    // add data to database
    QSqlTableModel model{};
    model.setTable("staffInfo");
    QSqlRecord record {model.record()};

    // Set the fields in the record
    record.setValue("Name", m_nameEdit->text());
    record.setValue("Sex", m_sexEdit->currentText());
    record.setValue("Birthday", m_birthdayEdit->date().toString("yyyy/MM/dd"));
    record.setValue("Address", m_addressEdit->text());
    record.setValue("PhoneNum", m_phoneNumEdit->text());
    record.setValue("FaceID", faceID);
    record.setValue("AvatarPath", m_imgPathEdit->text());

    // Insert the record into the model
    if (!model.insertRecord(-1, record)) {
        qDebug() << "Failed to insert record:" << model.lastError().text();
        return;
    }

    // Submit the changes to the database
    if (!model.submitAll()) {
        qDebug() << "Failed to submit changes to the database:" << model.lastError().text();
    } else {
        qDebug() << "Record added successfully!";
    }
}

void registerPanel::dbInit ()
{
    m_db = new QSqlDatabase{QSqlDatabase::addDatabase("QSQLITE")};

    m_db->setDatabaseName(__SERVER_DB_PATH);
    if (!m_db->open())
    {
        qDebug() <<  "db open error" << m_db->lastError().text();
        return;
    }

    QSqlQuery createStaffTable{
        "CREATE TABLE IF NOT EXISTS staffInfo ("
        "StaffID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Name VARCHAR(128), "
        "Sex VARCHAR(56), "
        "Birthday VARCHAR(128), "
        "Address TEXT, "
        "PhoneNum VARCHAR(56), "
        "FaceID INTEGER UNIQUE, "
        "AvatarPath TEXT)"
    };
    if(!createStaffTable.exec())
    {
        qDebug() << "create staff table error" << createStaffTable.lastError().text();
        m_db->close();
    }

    QSqlQuery createAttendLogTable {
        "CREATE TABLE IF NOT EXISTS attendLog ("
        "AttendID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "StaffID INTEGER,"
        "Name VARCHAR(128),"
        "AttendTime TimeStamp NOT NULL DEFAULT(datetime('now','localtime')),"
        "Notes TEXT)"
    };
    if(!createAttendLogTable.exec())
    {
        qDebug() << "create attend log table error" << createAttendLogTable.lastError().text();
        m_db->close();
    }
}

