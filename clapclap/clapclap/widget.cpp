#include "widget.h"
#include "ui_widget.h"
#include <QMediaPlayer>
#include <QIODevice>
#include <QAudio>
#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QDebug>
#include <QByteArray>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->backgroundFrame->setStyleSheet("background-color: black;");
    ui->gameFrame->setStyleSheet("background-color: grey;");

    QAudioFormat audioFormat;

    audioFormat.setSampleRate(8000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleSize(16);
    audioFormat.setSampleType(QAudioFormat::SignedInt);
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setCodec("audio/pcm");



    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    QAudioInput *audioInput = new QAudioInput(info, audioFormat, this);
    connect(audioInput, SIGNAL(notify()), SLOT(notified()));

//    audioInput->start();

    test = audioInput->start();


}

Widget::~Widget()
{
    delete ui;
}

void Widget::notified()
{
    QByteArray foo = test->readAll();
    qWarning() << "notified() called";
}

void Widget::on_startButton_clicked()
{
    player->setMedia(QUrl::fromLocalFile("../clapclap/music.mp3"));
    player->setVolume(50);
    player->play();
}

void Widget::on_stopButton_clicked()
{
    player->stop();
}
