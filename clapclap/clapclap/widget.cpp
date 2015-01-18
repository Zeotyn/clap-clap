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
    test = audioInput->start();


}

Widget::~Widget()
{
    delete ui;
}

void Widget::notified()
{
    qWarning() << "______________________________";
    qWarning() << "notified() called";
    qWarning() << "is open: " << test->isOpen();
    qWarning() << "is readable: " << test->isReadable();
    qWarning() << "Bytes to write: " << test->bytesToWrite();
    qWarning() << "Bytes to available (to read): " << test->bytesAvailable();
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
