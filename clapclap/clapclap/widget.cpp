#include "widget.h"
#include "ui_widget.h"
#include <QMediaPlayer>
#include <QDebug>

const int BufferSize = 14096;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_inputdevice(QAudioDeviceInfo::defaultInputDevice()),
    m_audioinput(0),
    m_input(0),
    m_buffer(BufferSize,0),
    m_isClapped(false)
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
    // Channels set to mono.
    m_format.setChannelCount(1);
    // Set sample size.
    m_format.setSampleSize(8);
    //    m_format.setSampleType(QAudioFormat::UnSignedInt ); //Sample type as usigned integer sample
    m_format.setSampleType(QAudioFormat::SignedInt);
    // Byte order.
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    // Codec as simple audio/pcm.
    m_format.setCodec("audio/pcm");

    // Check if configured audioformat is supported.
    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest.";
        m_format = infoIn.nearestFormat(m_format);
    }

    // Create audioinput.
    createAudio();
}

void Widget::createAudio()
{
    if(m_input != 0)
    {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioinput = new QAudioInput(m_inputdevice, m_format, this);
}

void Widget::readMore()
{
    // Exit if no audioinput is present.
    if(!m_audioinput)
        return;

    // Check data available to read.
    qint64 len = m_audioinput->bytesReady();

    if(len > 4096) {
        len = 4096;
    }

    // Read input stream and store into our buffer.
    qint64 l = m_input->read(m_buffer.data(), len);

    /*************************************************************************************************
     * @TODO: Clean up and documentation the following lines.
     */



    const int channelBytes = m_format.sampleSize() / 8;
    const int sampleBytes = m_format.channelCount() * channelBytes;
    const int numSamples = len / sampleBytes;
    quint32 maxValue = 0;
    const unsigned char *ptr = reinterpret_cast<const unsigned char *>(m_buffer.data());

    if(l > 0) {

        for (int i = 0; i < numSamples; ++i) {
            for (int j = 0; j < m_format.channelCount(); ++j) {
                quint32 value = 0;

                value = qAbs(*reinterpret_cast<const qint8*>(ptr));


                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }
        qWarning() << maxValue;
        maxValue = qMin(maxValue, quint32(127));
        qreal level = qreal(maxValue) / 127;
        qWarning() << level;

        if(level > 0.7) {
            m_isClapped = true;
        }
        else {
            m_isClapped = false;
        }

    }
}

void Widget::on_startButton_clicked()
{
    //    player->setMedia(QUrl::fromLocalFile("../clapclap/music.mp3"));
    //    player->setVolume(50);
    //    player->play();

    // Start microphone listening.
    m_input = m_audioinput->start();

    // Connect readyRead signal to readMre slot.
    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));
}

void Widget::on_stopButton_clicked()
{
    //    player->stop();

    // Stop audioinput.
    m_audioinput->stop();
}
