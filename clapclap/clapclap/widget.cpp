#include "widget.h"
#include "ui_widget.h"
#include <QMediaPlayer>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->backgroundFrame->setStyleSheet("background-color: black;");
    ui->gameFrame->setStyleSheet("background-color: grey;");

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_startButton_clicked()
{
    player->setMedia(QUrl::fromLocalFile("../clapclap/music.mp3"));
    player->setVolume(50);
    player->play();
}

void Widget::on_stopButton_clicked()
{
    player->pause();
}
