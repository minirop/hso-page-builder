#ifndef GIFSLIDER_H
#define GIFSLIDER_H

#include <QWidget>

class Gif;
class GifSlider : public QWidget
{
    Q_OBJECT
public:
    explicit GifSlider(QWidget *parent = nullptr);
    ~GifSlider();

    void setGif(QString name);
    void refresh();

protected:
    void paintEvent(QPaintEvent * event);

private:
    std::unique_ptr<Gif> gif;
};

#endif // GIFSLIDER_H
