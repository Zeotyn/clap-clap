#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
//    foo(0)
{
    ui->setupUi(this);



    QThread* somethread = new QThread(this);
    QTimer* timer = new QTimer(0); //parent must be null
    timer->setInterval(1);
    timer->moveToThread(somethread);
    //connect what you want
    connect(timer, SIGNAL(timeout()), SLOT(gogo()), Qt::DirectConnection);
    QObject::connect(somethread, SIGNAL(started()), timer, SLOT(start()));
    somethread->start();



//    QThread* m_thread = new QThread(this);
//    QTimer* timer = new QTimer(0); // _not_ this!
//    timer->setInterval(1);
//    timer->moveToThread(m_thread);
//    // Use a direct connection to make sure that doIt() is called from m_thread.
//    connect(timer, SIGNAL(timeout()), SLOT(gogo()), Qt::DirectConnection);
//    // Make sure the timer gets started from m_thread.
//    QObject::connect(m_thread, SIGNAL(started()), timer, SLOT(start()));
//    m_thread->start();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gogo()
{
    qWarning() << "gogogo";
//    a = foo / 1000;
////    qWarning() << a;
//    QString b = QString::number(a);
//    ui->label->setText(b);
//    foo++;
}
