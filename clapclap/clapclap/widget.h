#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QIODevice>
#include <QByteArray>
#include <QTimer>
#include <QtDebug>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();    
    void startGame();
    void stopGame();


private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();
    void isClapped();
    void delayChange();
    void readAudio();

private:
    Ui::Widget *ui;
    QMediaPlayer *m_player;

    // Audiodevice for input.
    QAudioDeviceInfo m_inputdevice;
    // Audioformat to be used.
    QAudioFormat m_format;
    // Audioinput interface.
    QAudioInput *m_audioinput;
    // Inputdevice used to read the bytes recoreded.
    QIODevice *m_input;
    // Read buffer for bytes recorded.
    QByteArray m_buffer;
    // Simple variable to check whether it had been slapped.
    bool m_isClapped;
    int m_score;
    QTimer * m_clapTimer;

    // Audio setup.
    void initAudio();
    void createAudio();

};

#endif // WIDGET_H
