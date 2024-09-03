#ifndef FACEINFO_H
#define FACEINFO_H

#include <QObject>
#include "seeta/FaceEngine.h"
#include "opencv.hpp"

class FaceInfo : public QObject
{
    Q_OBJECT
public:
    // Static method to access the single instance of the class
    static FaceInfo* instance(QObject *parent = nullptr);

    // Public methods
public slots:
    int64_t faceRegister(cv::Mat& faceImg);
    int64_t faceQuery(cv::Mat& faceImg);

signals:
    void faceQueried(int64_t faceID);

private:
    // Private constructor to prevent direct instantiation
    explicit FaceInfo(QObject *parent = nullptr);
    ~FaceInfo();

    // Singleton instance
    static FaceInfo* m_instance;

    // FaceEngine instance
    seeta::FaceEngine* m_seetaFaceEngine{};
};

#endif // FACEINFO_H
