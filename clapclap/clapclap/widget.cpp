#include "widget.h"
#include "ui_widget.h"
#include <QMediaPlayer>

//#include <QIODevice>
//#include <QAudio>
//#include <QAudioInput>
//#include <QAudioDeviceInfo>
//#include <QAudioFormat>
#include <QDebug>
//#include <QByteArray>

const int BufferSize = 14096;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_inputdevice(QAudioDeviceInfo::defaultInputDevice()),
    m_audioinput(0),
    m_input(0),
    m_buffer(BufferSize,0)

{
    ui->setupUi(this);
    ui->backgroundFrame->setStyleSheet("background-color: black;");
    ui->gameFrame->setStyleSheet("background-color: grey;");

    initAudio();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initAudio()
{
    m_format.setSampleRate(8000);

    m_format.setChannelCount(1); //set channels to mono
    m_format.setSampleSize(16); //set sample sze to 16 bit
    m_format.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    m_format.setByteOrder(QAudioFormat::LittleEndian); //Byte order
    m_format.setCodec("audio/pcm"); //set codec as simple audio/pcm


    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format)) {
        //Default format not supported - trying to use nearest
        m_format = infoIn.nearestFormat(m_format);
    }

    createAudio();
}

void Widget::createAudio()
{
    if(m_input != 0) {
        disconnect(m_input,0,this,0);
        m_input = 0;
    }

    m_audioinput = new QAudioInput(m_inputdevice, m_format, this);
}

void Widget::readMore()
{

    if(!m_audioinput)
        return;

    //Check the number of samples in input buffer
    qint64 len = m_audioinput->bytesReady();

    if(len > 4096) {
        len = 4096;
    }

    qint64 l = m_input->read(m_buffer.data(), len);
    QByteArray test = m_input->readAll();



    if(l > 0) {
        //Assign sound samples to short array
        short* resultingData = (short*)m_buffer.data();
        short *outdata=resultingData;

        qWarning() << resultingData;
    }
}

void Widget::on_startButton_clicked()
{
//    player->setMedia(QUrl::fromLocalFile("../clapclap/music.mp3"));
//    player->setVolume(50);
//    player->play();

    // Start microphone listening.
    m_input = m_audioinput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
}

void Widget::on_stopButton_clicked()
{
    player->stop();
}
