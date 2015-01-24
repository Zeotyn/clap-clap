#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QIODevice>
#include <QByteArray>
#include <QTimer>
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QThread>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

signals:
    void counterChanged(int);
    void wrongClapsCountChanged(QString);
    void rightClapsCountChanged(QString);
    void backgroundChanged(QString);

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void countdown();
    void readAudio();
    void progress();


private:
    Ui::Widget *ui;

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

    int m_right;
    int m_countdown;

    QTimer * m_countdownTimer;

    // Required input-level for slapping.
    qreal m_levelRequired;

    QTimer * m_progressTimer;

    qreal m_bpm;
    int m_wrong;

    int m_counter;

    QThread* m_progressThread;
    QString m_background;
    quint32 m_maxAmplitude;

    // Audio setup.
    void initAudio();
    void createAudio();

    void initGame();
    void startGame();
    void stopGame();



};

#endif // WIDGET_H
