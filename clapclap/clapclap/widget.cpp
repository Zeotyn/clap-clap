#include "widget.h"
#include "ui_widget.h"
#include <qendian.h>

// Buffersize for microphone data.
const int BufferSize = 14096;

/**
 * @brief Widget::Widget
 * Constructor for widget class.
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
    m_background("background: black;"),
    m_maxAmplitude(0)
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
 * @brief Widget::~Widget
 * Constructor for widget class.
 */
Widget::~Widget()
{
    delete ui;
}

/**
 * @brief Widget::initAudio
 * Setup audio needs like audioformat and prepare audioinput.
 */
void Widget::initAudio()
{
    // Set sample rate.
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

/**
 * @brief Widget::createAudio
 * Create audio reciever.
 */
void Widget::createAudio()
{
    if(m_input != 0)
    {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    // Create reciever for audio data from input device.
    m_audioinput = new QAudioInput(m_inputdevice, m_format, this);
}

/**
 * @brief Widget::readAudio
 * Audio data reading function.
 */
void Widget::readAudio()
{
    // Exit if no audioinput is present.
    if(!m_audioinput)
        return;

    // Check data available to read.
    qint64 len = m_audioinput->bytesReady();

    // Check given audioformat and setup m_maxAmplitude.
    switch (m_format.sampleSize())
    {
    case 8:
        switch (m_format.sampleType())
        {
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
        switch (m_format.sampleType())
        {
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
        switch (m_format.sampleType())
        {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff;
        default:
            break;
        }
        break;

    default:
        break;
    }

    // Setup byte-length for data to be read.
    if(len > 4096)
    {
        len = 4096;
    }

    // Read input stream and store into our buffer.
    qint64 l = m_input->read(m_buffer.data(), len);

    const int channelBytes = m_format.sampleSize() / 8;
    const int sampleBytes = m_format.channelCount() * channelBytes;
    const int numSamples = len / sampleBytes;
    quint32 maxValue = 0;
    const unsigned char *ptr = reinterpret_cast<const unsigned char *>(m_buffer.data());

    if(l > 0)
    {
        // Loop through buffer data and get the maximum value given.
        // In dependency of the given audioformat we need to convert some things.
        for (int i = 0; i < numSamples; ++i)
        {
            for (int j = 0; j < m_format.channelCount(); ++j)
            {
                quint32 value = 0;

                if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    value = *reinterpret_cast<const quint8*>(ptr);
                }
                else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    value = qAbs(*reinterpret_cast<const qint8*>(ptr));
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint16>(ptr);
                    else
                        value = qFromBigEndian<quint16>(ptr);
                }
                else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint16>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint16>(ptr));
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qFromLittleEndian<quint32>(ptr);
                    else
                        value = qFromBigEndian<quint32>(ptr);
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt)
                {
                    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
                        value = qAbs(qFromLittleEndian<qint32>(ptr));
                    else
                        value = qAbs(qFromBigEndian<qint32>(ptr));
                }
                else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float)
                {
                    value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff);
                }

                maxValue = qMax(value, maxValue);
                ptr += channelBytes;
            }
        }

        maxValue = qMin(maxValue, m_maxAmplitude);

        // Level from 0 to 1.
        qreal level = qreal(maxValue) / m_maxAmplitude;

        if(level > m_levelRequired) {
            m_isClapped = true;
        }
        else {
            m_isClapped = false;
        }
    }
}

/**
 * @brief Widget::on_startButton_clicked
 * Click-eventhandler for start button.
 */
void Widget::on_startButton_clicked()
{
    // Set active states for ui buttons.
    ui->startButton->setDisabled(true);
    ui->stopButton->setDisabled(false);

    ui->rightLabel->setText("0");
    ui->wrongLabel->setText("0");

    // Initialize the game.
    initGame();
}

/**
 * @brief Widget::on_stopButton_clicked
 * Click-eventhandler for stop button.
 */
void Widget::on_stopButton_clicked()
{
    // Set active states for ui buttons.
    ui->startButton->setDisabled(false);
    ui->stopButton->setDisabled(true);

    // Stop the game.
    stopGame();
}

/**
 * @brief Widget::initGame
 * Game setup.
 * Sets up timers and thread needed.
 */
void Widget::initGame()
{
    // Convert beats per minutes into milliseconds.
    m_bpm = (60/(qreal)ui->spinBox->value()) * 1000;
    ui->progressBar->setMaximum((int)m_bpm);

    // Setup a simple countdown.
    m_countdownTimer->setTimerType(Qt::PreciseTimer);
    m_countdownTimer->start(1000);
    connect(m_countdownTimer, SIGNAL(timeout()), this, SLOT(countdown()));

    // Updating ui elements by signal is thread safe so we use this to prevent crashes.
    connect(this,SIGNAL(counterChanged(int)), ui->progressBar, SLOT(setValue(int)));
    // Update wrong claps by signal.
    connect(this,SIGNAL(wrongClapsCountChanged(QString)), ui->wrongLabel, SLOT(setText(QString)));
    // Update correct claps by signal.
    connect(this,SIGNAL(rightClapsCountChanged(QString)), ui->rightLabel, SLOT(setText(QString)));
    // Update background for the backgroundFrame (either black, green or red).
    connect(this,SIGNAL(backgroundChanged(QString)), ui->backgroundFrame, SLOT(setStyleSheet(QString)));
}

/**
 * @brief Widget::countdown
 * Simulate a simple countdown.
 */
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

/**
 * @brief Widget::startGame
 * Run the game.
 */
void Widget::startGame()
{
    // Connect the progresstimer with the corresponding slot.
    connect(m_progressTimer, SIGNAL(timeout()), SLOT(progress()), Qt::DirectConnection);
    // Link thread and timer.
    QObject::connect(m_progressThread, SIGNAL(started()), m_progressTimer, SLOT(start()));
    // Start the thread.
    m_progressThread->start();

    // Start microphone listening.
    m_input = m_audioinput->start();
    // Connect readyRead signal to readMre slot.
    connect(m_input, SIGNAL(readyRead()), SLOT(readAudio()));
}

/**
 * @brief Widget::stopGame
 * Stop the game by terminating all slots, timers and resetting variables.
 */
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
    m_wrong = 0;
    m_right = 0;

    // Stop audioinput.
    m_audioinput->stop();

    ui->progressBar->setValue(0);
    ui->countdownLabel->setText("5");
}

/**
 * @brief Widget::progress
 * Manages all the ui updates during the game.
 */
void Widget::progress()
{
    if(m_counter == (int)m_bpm)
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
    // Broadcast signals for clap changes.
    emit wrongClapsCountChanged(wrongString);
    emit rightClapsCountChanged(rightString);
}
