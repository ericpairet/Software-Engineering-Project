#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T16:38:16
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CIS
TEMPLATE = app

win32:{
INCLUDEPATH += "C:\Program Files (x86)\Eigen\include\eigen3" \
            "C:\opencv\opencv\sources\build\install\include" \
            .
LIBS += -L"C:\opencv\opencv\sources\build\install\x86\mingw\bin" \
    -lopencv_shape300 \
    -lopencv_stitching300 \
    -lopencv_objdetect300 \
    -lopencv_superres300 \
    -lopencv_videostab300 \
    -lopencv_calib3d300 \
    -lopencv_features2d300 \
    -lopencv_highgui300 \
    -lopencv_videoio300 \
    -lopencv_imgcodecs300 \
    #-lopencv_video300 \
    -lopencv_photo300 \
    -lopencv_ml300 \
    -lopencv_imgproc300 \
    -lopencv_flann300 \
    -lopencv_core300
#    -lopencv_hal300
}

unix:!macx: {
INCLUDEPATH += /usr/include \
            /usr/include/eigen3
LIBS += -L/usr/lib/ \
    #-lopencv_shape \
    -lopencv_stitching \
    -lopencv_objdetect \
    -lopencv_superres \
    -lopencv_videostab \
    -lopencv_calib3d \
    -lopencv_features2d \
    -lopencv_highgui \
    #-lopencv_videoio \
    #-lopencv_imgcodecs \
    -lopencv_video \
    -lopencv_photo \
    -lopencv_ml \
    -lopencv_imgproc \
    -lopencv_flann \
    -lopencv_core \
    #-lopencv_hal

}

macx {
INCLUDEPATH += /usr/local/include
            .
LIBS += -L/usr/local/lib/ \
    -lopencv_shape \
    -lopencv_stitching \
    -lopencv_objdetect \
    -lopencv_superres \
    -lopencv_videostab \
    -lopencv_calib3d \
    -lopencv_features2d \
    -lopencv_highgui \
    -lopencv_videoio \
    -lopencv_imgcodecs \
    -lopencv_video \
    -lopencv_photo \
    -lopencv_ml \
    -lopencv_imgproc \
    -lopencv_flann \
    -lopencv_core \
    -lopencv_hal
}

SOURCES += main.cpp \
    mainwindow.cpp \
    widgets.cpp \
    segmentation.cpp

HEADERS += \
    mathtools.h \
    mainwindow.h \
    widgets.h \
    segmentation.h
