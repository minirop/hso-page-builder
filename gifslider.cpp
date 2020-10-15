#include "gifslider.h"
#include "gif.h"
#include <QPainter>

GifSlider::GifSlider(QWidget *parent) : QWidget(parent)
{
    gif = std::make_unique<Gif>();

    startTimer(10);
}

GifSlider::~GifSlider() = default;

void GifSlider::setGif(QString name)
{
    gif->nameOf = name;
    gif->refresh();
    update();
}

void GifSlider::refresh()
{
    if (gif)
    {
        gif->refresh();
    }
    update();
}

void GifSlider::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    if (!gif) return;

    QPainter p(this);

    auto pixmap = gif->pixmap();
    p.drawPixmap(rect(), pixmap);
}

void GifSlider::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event)

    update();
}
