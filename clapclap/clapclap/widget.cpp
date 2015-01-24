#include "widget.h"
#include "ui_widget.h"
#include <qendian.h>

const int BufferSize = 14096;

/**
 * Constructor for widget class.
 * @brief Widget::Widget
 * @param parent
 */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_inputdevice(QAudioDeviceInfo::defaultInputDevice()),
    m_audioinput(0),
    m_input(0),
    m_buffer(BufferSize,0),
    m_isClapped(false),
    m_right(0),
    m_countdown(4),
    m_levelRequired(0.05),
    m_bpm(0),
    m_wrong(0),
    m_counter(0),
    m_progressThread(new QThread(parent)),
    m_background("background: black;")
{
    m_progressTimer = new QTimer(0);
    m_progressTimer->setInterval(1);
    m_progressTimer->setTimerType(Qt::PreciseTimer);
    m_progressTimer->moveToThread(m_progressThread);

    m_countdownTimer = new QTimer(this);
    // Setup ui elements.
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    // Setup audio.
    initAudio();
}

/**
 * Deconstrutor.
 * @brief Widget::~Widget
 */
Widget::~Widget()
{
    delete ui;
}

/**
 * Setup audio needs like audioformat and prepare audioinput.
 * @brief Widget::initAudio
 */
void Widget::initAudio()
{
    m_format.setSampleRate(8000);
    // Channels set to mono.
    m_format.setChannelCount(1);
    // Set sample size.
    m_format.setSampleSize(16);
    // Sample type.
    m_format.setSampleType(QAudioFormat::SignedInt);
    // Byte order.
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    // Codec as simple audio/pcm.
    m_format.setCodec("audio/pcm");

    // Check if configured audioformat is supported.
    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format))
    {
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

void Widget::readAudio()
{
    // Exit if no audioinput is present.
    if(!m_audioinput)
        return;

    // Check data available to read.
    qint64 len = m_audioinput->bytesReady();
    quint32 m_maxAmplitude;

    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        break;
    }

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

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    value = *reinterpret_cast<const quint8*>(ptr);
                } else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                } else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                } else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
                    value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
                }


                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }
        maxValue = qMin(maxValue, m_maxAmplitude);
        qreal level = qreal(maxValue) / m_maxAmplitude;

        if(level > m_levelRequired) {
            m_isClapped = true;
        }
        else {
            m_isClapped = false;
        }
    }
}

void Widget::on_startButton_clicked()
{
    // Set active states for ui buttons.
    ui->startButton->setDisabled(true);
    ui->stopButton->setDisabled(false);
    initGame();
}

void Widget::on_stopButton_clicked()
{
    // Set active states for ui buttons.
    ui->startButton->setDisabled(false);
    ui->stopButton->setDisabled(true);
    stopGame();
}

void Widget::initGame()
{
    m_bpm = (60/ui->spinBox->value()) * 1000;
    ui->progressBar->setMaximum(m_bpm);
    m_countdownTimer->setTimerType(Qt::PreciseTimer);
    m_countdownTimer->start(1000);
    connect(m_countdownTimer, SIGNAL(timeout()), this, SLOT(countdown()));

    connect(this,SIGNAL(counterChanged(int)), ui->progressBar, SLOT(setValue(int)));
    connect(this,SIGNAL(wrongClapsCountChanged(QString)), ui->wrongLabel, SLOT(setText(QString)));
    connect(this,SIGNAL(rightClapsCountChanged(QString)), ui->rightLabel, SLOT(setText(QString)));

    connect(this,SIGNAL(backgroundChanged(QString)), ui->backgroundFrame, SLOT(setStyleSheet(QString)));
}

void Widget::countdown() {
    QString countdownString = QString::number(m_countdown);
    ui->countdownLabel->setText(countdownString);
    ui->countdownLabel->setStyleSheet("color: white;");

    if(m_countdown == 0)
    {
        ui->countdownLabel->setText(" ");
        // Stop countdown and disconnect.
        m_countdownTimer->stop();
        disconnect(m_countdownTimer, 0, 0, 0);
        startGame();
    }
    else
    {
        m_countdown--;
    }
}

void Widget::startGame()
{
    connect(m_progressTimer, SIGNAL(timeout()), SLOT(progress()), Qt::DirectConnection);
    QObject::connect(m_progressThread, SIGNAL(started()), m_progressTimer, SLOT(start()));

    m_progressThread->start();

    // Start microphone listening.
    m_input = m_audioinput->start();
    // Connect readyRead signal to readMre slot.
    connect(m_input, SIGNAL(readyRead()), SLOT(readAudio()));
}

void Widget::stopGame()
{
    // Disconnect all signals and exit thread.
    disconnect(m_input, 0, 0, 0);
    disconnect(m_progressTimer, 0, 0, 0);
    QObject::disconnect(m_progressThread, 0,0,0);
    m_progressThread->exit();

    m_countdown = 4;
    m_counter = 0;
    m_countdownTimer->stop();

    // Stop audioinput.
    m_audioinput->stop();

    ui->progressBar->setValue(0);
    ui->countdownLabel->setText("5");
}

void Widget::progress()
{
    if(m_counter == m_bpm)
    {
        if(m_isClapped)
        {
            m_right++;
            m_background = "background: green;";
            emit backgroundChanged(m_background);
        }
        else
        {
            m_wrong++;
            m_background = "background: red;";
            emit backgroundChanged(m_background);
        }
        m_counter = 0;
    }
    else {
        m_counter++;

        if(m_counter == 100) {
            m_background = "background: black;";
            emit backgroundChanged(m_background);
        }
    }

    QString rightString = QString::number(m_right);
    QString wrongString = QString::number(m_wrong);

    // Broadcast signal that counter has changed.
    emit counterChanged(m_counter);
    emit wrongClapsCountChanged(wrongString);
    emit rightClapsCountChanged(rightString);
}


