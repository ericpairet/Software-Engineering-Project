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
}

void CSegmenterGTester::TearDown() {
    delete testPxm;
    delete testImg;
    delete segTool;
    delete testMon;
    delete inputImage;
}

TEST_F(CSegmenterGTester, graphLapMat) {
    double betha = 0.005;
    double sigma = 0.1;
    SparseMatrix<double> outPut( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    loadMarket(outPut, "../gTestData/graphOutput.gtd");
    sparseHash<SparseMatrix<double> > hashGenerator;
    SparseMatrix<double> dummy( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator(outPut), segTool->GraphLaplacianMatrix( *inputImage, betha, sigma, dummy, true));
}

TEST_F(CSegmenterGTester, seedDepMat) {
    SparseMatrix<double> out( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    loadMarket(out, "../gTestData/matISOutput.gtd");
    sparseHash<SparseMatrix<double> > hashGenerator;
    SparseMatrix<double> dummy( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator(out), segTool->SeedsDependentMatrixIs( dummy, true));
}

TEST_F(CSegmenterGTester, seedDepVec) {
    double Xf = 0.0;
    double Xb = 1.0;
    VectorXd outPut( inputImage->rows *inputImage->cols);
    ifstream ifs("../gTestData/vecBOutput.gtd", ios::binary);
    string s;
    int ind = 0;
    while( ifs >> s)
        outPut( ind++) = stod( s);
    vectorHash<VectorXd> hashGenerator;
    VectorXd dummy( inputImage->rows *inputImage->cols);
    dummy = VectorXd::Zero( inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator( outPut), segTool->SeedsDependentVectorb( Xf, Xb, dummy, "Foreground", true));
}

TEST_F(CSegmenterGTester, l2Mat) {
    SparseMatrix<double> outPut( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    SparseMatrix<double> inPut( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    loadMarket(outPut, "../gTestData/l2Output.gtd");
    loadMarket(inPut, "../gTestData/l2Input.gtd");
    sparseHash<SparseMatrix<double> > hashGenerator;
    SparseMatrix<double> dummy( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator( outPut), segTool->GraphLaplacianMatrixSquare( inPut, dummy, true));
}

TEST_F(CSegmenterGTester, systemSolver) {
    VectorXd outPut( inputImage->rows *inputImage->cols);
    ifstream ifs("../gTestData/systemSolverOutput.gtd", ios::binary);
    string s;
    int ind = 0;
    while( ifs >> s)
        outPut( ind++) = stod( s);
    SparseMatrix<double> l2Is( inputImage->rows *inputImage->cols, inputImage->rows *inputImage->cols);
    loadMarket(l2Is, "../gTestData/l2+Is.gtd");
    VectorXd inPut( inputImage->rows *inputImage->cols);
    ifstream ifss("../gTestData/systemSolverInput.gtd", ios::binary);
    ind = 0;
    while( ifss >> s)
        inPut( ind++) = stod( s);
    vectorHash<VectorXd> hashGenerator;
    VectorXd dummy( inputImage->rows *inputImage->cols);
    dummy = VectorXd::Zero( inputImage->rows *inputImage->cols);
    EXPECT_EQ( hashGenerator( outPut), segTool->ComputeLinearSystem( l2Is, inPut, dummy, true));
}
