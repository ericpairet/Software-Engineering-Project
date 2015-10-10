#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T16:38:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CIS
TEMPLATE = app

unix:!macx: {
INCLUDEPATH += /usr/include
            .
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

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h
