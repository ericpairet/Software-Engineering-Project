#include "tester.h"

void CSegmenterGTester::SetUp() {
    testPxm = new QPixmap("../gTestData/testImage.jpg");
    testImg = new QImage("../gTestData/testImage.jpg");
    testSeedsColors["Background"] = QColor("red");
    testSeedsColors["Foreground"] = QColor("blue");
    QSet<QPair<int, int> > bgSeeds, fgSeeds;
    bgSeeds.insert( QPair<int, int>( 1, 6));
    bgSeeds.insert( QPair<int, int>( 1, 7));
    bgSeeds.insert( QPair<int, int>( 2, 7));
    bgSeeds.insert( QPair<int, int>( 3, 7));
    bgSeeds.insert( QPair<int, int>( 4, 7));
    bgSeeds.insert( QPair<int, int>( 4, 7));
    fgSeeds.insert( QPair<int, int>( 2, 3));
    fgSeeds.insert( QPair<int, int>( 2, 4));
    fgSeeds.insert( QPair<int, int>( 3, 3));
    testSeedsPos["Background"] = bgSeeds;
    testSeedsPos["Foreground"] = fgSeeds;
    Mat * I = new Mat( testImg->height() , testImg->width() , CV_8UC4 , testImg->bits() , testImg->bytesPerLine() );
    inputImage = new Mat( I->rows , I->cols , CV_32FC3 );
    cvtColor( *I , *inputImage , CV_RGBA2RGB );
    testMon = new CMonitorWidget( testPxm, testSeedsColors, testSeedsPos);
    segTool = new CSegmentation( testMon);
    segTool->setInputImage( *testImg);
//    segTool->run();
}

void CSegmenterGTester::TearDown() {
    delete segTool;
    delete testMon;
    delete inputImage;
}

TEST_F(CSegmenterGTester, graphLapMat) {
    double betha = 0.005;
    double sigma = 0.1;
    SparseMatrix<double> outPut( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    loadMarket(outPut, "graphOutput.gtd");
    sparseHash<SparseMatrix<double> > hashGenerator;
    SparseMatrix<double> dummy( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator(outPut), segTool->GraphLaplacianMatrix( *inputImage, betha, sigma, dummy, true));
}

//TEST_F(CSegmenterGTester, seedDepVec) {
//    int Xf = 0;
//    int Xb = 1;
//    VectorXd outPut( inputImage->rows *inputImage->cols);
//    ifstream ifs("vecBOutput.gtd", ios::binary);
//    ifs.read((char *)&outPut, sizeof( outPut));
//    cout << "saved : \n\n" << outPut << "\n\n\n";
//    vectorHash<VectorXd> hashGenerator;
//    VectorXd dummy( inputImage->rows *inputImage->cols);
//    EXPECT_EQ( hashGenerator( outPut), segTool->SeedsDependentVectorb( Xf, Xb, dummy, "Foreground", true));
//}
