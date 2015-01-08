#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>

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

private:
    Ui::Widget *ui;
    QMediaPlayer *player = new QMediaPlayer;
};

#endif // WIDGET_H
