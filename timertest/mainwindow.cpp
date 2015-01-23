#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
//    foo(0)
{
    ui->setupUi(this);



    qWarning() << "hello";

    QThread* somethread = new QThread(this);
    QTimer *timer = new QTimer(0);
    timer->moveToThread(somethread);
    timer->setInterval(1);
    timer->setTimerType(Qt::PreciseTimer);
//    timer->start(1);
    connect(timer, SIGNAL(timeout()), this, SLOT(gogo()));
//    somethread->start();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gogo()
{

    a = foo / 1000;
//    qWarning() << a;
    QString b = QString::number(a);
    ui->label->setText(b);
    foo++;
}
