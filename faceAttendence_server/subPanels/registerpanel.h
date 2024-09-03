#ifndef REGISTERPANEL_H
#define REGISTERPANEL_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "ElaLineEdit.h"
#include "ElaComboBox.h"
#include <QDateEdit>
#include "QLabel"
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include "ElaPushButton.h"
#include <QImageCapture>
#include <QResizeEvent>

class QGridLayout;
class QVBoxLayout;

#define SAFE_DELETE(ptr) \
do { \
        if (ptr) { \
            delete ptr; \
            ptr = nullptr; \
    } \
} while (0)


class registerPanel : public QWidget
{
    Q_OBJECT
public:
    explicit registerPanel(QWidget *parent = nullptr);
    ~registerPanel();

    void inputWidgetInit(QGridLayout *layout);

signals:

private:
    QSqlDatabase* m_db{};
    void dbInit();

    //layouts

    //info input widgets
    ElaLineEdit* m_nameEdit{};
    ElaComboBox* m_sexEdit{};
    QDateEdit* m_birthdayEdit{};
    ElaLineEdit* m_addressEdit{};
    ElaLineEdit* m_phoneNumEdit{};

    //img set widgets
    ElaLineEdit* m_imgPathEdit{};
    QLabel* m_avatarShow;
    ElaPushButton* m_setImgPathBtn{};
    ElaPushButton* m_captureBtn{};
    ElaPushButton* m_startCameraBtn{};
    QCamera* m_camera{};
    QMediaCaptureSession* m_captureSession{};
    QVideoWidget* m_videoWidget{};
    QImageCapture* m_imageCapture{};
    QImage* m_captureImage{};

    void resizeEvent(QResizeEvent* event);

    void cameraWidgetInit(QVBoxLayout *layout);


private slots:
    void registerFace ();
};

#endif // REGISTERPANEL_H
