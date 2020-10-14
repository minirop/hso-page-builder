#include "gifslider.h"
#include "gif.h"
#include <QPainter>

GifSlider::GifSlider(QWidget *parent) : QWidget(parent)
{
    gif = std::make_unique<Gif>();
}

GifSlider::~GifSlider() = default;

void GifSlider::setGif(QString name)
{
    gif->nameOf = name;
    gif->refresh();
}

void GifSlider::refresh()
{
    if (gif)
    {
        gif->refresh();
    }
}

void GifSlider::paintEvent(QPaintEvent * event)
{
    if (!gif) return;

    QPainter p(this);

    auto pixmap = gif->pixmap();
    p.drawPixmap(rect(), pixmap);
}
