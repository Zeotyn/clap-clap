#include "mainwindow.h"
#include <QApplication>
#include "mytimer.h"
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    QThread* somethread = new QThread(this);

//    MyTimer timer;

    return a.exec();
}
