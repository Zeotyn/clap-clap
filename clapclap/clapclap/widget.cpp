#include "widget.h"
#include "ui_widget.h"
#include "foobar.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

foobar *blubb = new foobar();
blubb->getTest();

