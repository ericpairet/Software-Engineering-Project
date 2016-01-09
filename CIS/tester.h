#ifndef CGTESTER_H
#define CGTESTER_H

#include "gtest/gtest.h"
#include "segmentation.h"
#include "widgets.h"
#include <fstream>

class CSegmenterGTester : public ::testing::Test {
protected:
    CSegmentation *segTool;
    QThread *segmentationThread;
    QPixmap *testPxm;
    QImage *testImg;
    Mat *inputImage;
    QMap<QString, QColor> testSeedsColors;
    QMap<QString, QSet<QPair<int, int> > > testSeedsPos;
    CMonitorWidget *testMon;
    virtual void SetUp();     // gtest construction
    virtual void TearDown();  // gtest destruction

};
#endif // CGTESTER_H