#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T16:38:16
#
#-------------------------------------------------

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CIS
TEMPLATE = app

CONFIG += c++11

win32:{
INCLUDEPATH += "C:\Program Files (x86)\Eigen\include\eigen3" \
            "C:\opencv\opencv\sources\build\install\include" \
            "C:\GTEST\gtest-1.7.0\include"
            .
LIBS += -L"C:\opencv\opencv\sources\build\install\x86\mingw\bin" \
    -lopencv_imgproc300 \
    -lopencv_core300

LIBS += -L"C:\GTEST\gtest-1.7.0\build" \
    -lgtest \
    -lgtest_main
}

unix:!macx: {
INCLUDEPATH += /usr/include \
            /usr/include/eigen3
LIBS += -L/usr/lib/ \
    -lopencv_imgproc \
    -lopencv_core
LIBS += -L/usr/local/lib/ \
    -lgtest \
    -lgtest_main
}

macx {
INCLUDEPATH += /usr/local/include
            .
LIBS += -L/usr/local/lib/ \   
    -lopencv_imgproc \    
    -lopencv_core \s
}

SOURCES += main.cpp \
    mainwindow.cpp \
    widgets.cpp \
    segmentation.cpp \
    tester.cpp

HEADERS += \
    mathtools.h \
    mainwindow.h \
    widgets.h \
    segmentation.h \
    tester.h
