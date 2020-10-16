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

    void setGif(QString name);
    void setFrameOffset(int f);
    void refresh();

protected:
    void paintEvent(QPaintEvent * event) override;
    void timerEvent(QTimerEvent * event) override;

private:
    std::unique_ptr<Gif> gif;
    int timerId = -1;
};

#endif // GIFSLIDER_H
