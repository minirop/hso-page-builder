#ifndef GIFSLIDER_H
#define GIFSLIDER_H

#include <QWidget>
#include <memory>

class Gif;
class GifSlider : public QWidget
{
    Q_OBJECT
public:
    explicit GifSlider(QWidget *parent = nullptr);
    ~GifSlider();

    void setGif(Gif * g);

protected:
    void paintEvent(QPaintEvent * event) override;
    void timerEvent(QTimerEvent * event) override;

private:
    Gif * gif = nullptr;
    int timerId = -1;
};

#endif // GIFSLIDER_H
