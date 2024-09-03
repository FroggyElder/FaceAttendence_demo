#include "faceinfo.h"
#include <QDebug>

#include "../dbPathDefines.h"

// Initialize the static member
FaceInfo* FaceInfo::m_instance = nullptr;

FaceInfo* FaceInfo::instance(QObject *parent)
{
    if (!m_instance) {
        m_instance = new FaceInfo(parent);
    }
    return m_instance;
}

FaceInfo::FaceInfo(QObject *parent)
    : QObject{parent}
{
    // Initialization code
    seeta::ModelSetting fd_mode{"E:/qt_workspace/libs/SeetaFace2/bin/model/fd_2_00.dat", seeta::ModelSetting::GPU, 0};
    seeta::ModelSetting pd_mode{"E:/qt_workspace/libs/SeetaFace2/bin/model/pd_2_00_pts5.dat", seeta::ModelSetting::GPU, 0};
    seeta::ModelSetting fr_mode{"E:/qt_workspace/libs/SeetaFace2/bin/model/fr_2_10.dat", seeta::ModelSetting::GPU, 0};
    m_seetaFaceEngine = new seeta::FaceEngine{fd_mode, pd_mode, fr_mode};
    m_seetaFaceEngine->Load(__SEETA_DB_PATH);
}

FaceInfo::~FaceInfo()
{
    delete m_seetaFaceEngine;
}

// Register a face and return the face ID
int64_t FaceInfo::faceRegister(cv::Mat& faceImg)
{
    SeetaImageData tmpImgdata{faceImg.cols, faceImg.rows, faceImg.channels(), faceImg.data};
    int64_t faceid = m_seetaFaceEngine->Register(tmpImgdata);

    if (faceid >= 0)
    {
        m_seetaFaceEngine->Save(__SEETA_DB_PATH);
    }
    return faceid;
}

// Query a face and return the face ID
int64_t FaceInfo::faceQuery(cv::Mat& faceImg)
{
    SeetaImageData tmpImgdata{faceImg.cols, faceImg.rows, faceImg.channels(), faceImg.data};
    float tmp_sim{};
    int64_t faceid = m_seetaFaceEngine->Query(tmpImgdata, &tmp_sim);

    qDebug() << "faceQuery-> similarity:" << tmp_sim << "id:" << faceid;

    if (tmp_sim > 0.78) emit faceQueried(faceid);
    else emit faceQueried(-1);

    return faceid;
}
