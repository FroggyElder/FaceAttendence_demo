#ifndef CONDITIONAL_DEFINES_H
#define CONDITIONAL_DEFINES_H

#if defined(_WIN32) || defined(_WIN64)
// Windows platform
#define __CAS_PATH ("E:/qt_workspace/libs/opencv_452/etc/haarcascades/haarcascade_frontalface_alt2.xml")
#define __CAMERA_OPEN (0)
#define __CAMERA_INTERVAL 1

#elif defined(__linux__) && defined(__arm__)
// Linux ARM platform
#define __CAS_PATH ("/mnt/lib/opencv4-arm/etc/haarcascades/haarcascade_frontalface_alt2.xml")
#define __CAMERA_OPEN ("/dev/video7")
#define __CAMERA_INTERVAL 10

#else
// Default path or error handling
#define __CAS_PATH ("E:/qt_workspace/libs/opencv_452/etc/haarcascades/haarcascade_frontalface_alt2.xml")
#define __CAMERA_OPEN (0)
#define __CAMERA_INTERVAL 1
#endif

#endif // CONDITIONAL_DEFINES_H
