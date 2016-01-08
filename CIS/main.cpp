#include "mainwindow.h"
#include <QApplication>
#include "test.h"

#define Testing false

int main(int argc, char *argv[]) {
    if( Testing)
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
    QApplication a(argc, argv);
    mainWindow w;
    w.show();
    return a.exec();
}
