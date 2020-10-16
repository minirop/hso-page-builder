#include "gifslider.h"
#include "gif.h"
#include <QPainter>

GifSlider::GifSlider(QWidget *parent) : QWidget(parent)
{
    startTimer(10);
}

GifSlider::~GifSlider() = default;

void GifSlider::setGif(Gif * g)
{
    gif = g;
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
