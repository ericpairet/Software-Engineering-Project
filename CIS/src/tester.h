#ifndef CGTESTER_H
#define CGTESTER_H

#include "gtest/gtest.h"
#include "segmentation.h"
#include "widgets.h"
#include <cstdio>
#include <fstream>
#include <sstream>

/**
 * @brief Google test class : a class for creating several unit test for testing
 * all the functions of code.
 *
 */
class CSegmenterGTester : public ::testing::Test {
protected:
    CSegmentation *segTool; /**< An instance of segmentation tools being used for testing it */
    QPixmap *testPxm; /**< Test image in QPixmap format */
    QImage *testImg; /**< Test image in QImage format */
    Mat *inputImage; /**< Test image in openCV matrix format */
    QMap<QString, QColor> testSeedsColors; /**< Hardcocde seeds for tesing on particular test image */
    QMap<QString, QSet<QPair<int, int> > > testSeedsPos; /**< Position of the hardcoded seeds */
    CMonitorWidget *testMon; /**< Dummy monitor wigdet made to be passed to segmentation class */
    /**
     * @brief Google Test setup : this virtual function is a part of GTest structure for
     * setting up the testing environmet, working as the constructor of GTest based class
     *
     */
    virtual void SetUp();     // gtest construction
    /**
     * @brief Google Test tearDown : this virtual function is a part of GTest structure for
     * destroying the testing environmet, working as the destractor of GTest based class
     *
     */
    virtual void TearDown();  // gtest destruction
};
#endif // CGTESTER_H
