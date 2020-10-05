#include "gif.h"
#include <QPainter>
#include <QBitmap>

#include <QGraphicsView>
#include <QGraphicsScene>

Gif::Gif()
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void Gif::addFrame(QString filename)
{
    frames.push_back(QPixmap(filename));

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::setSpeed(int speed)
{
    startTimer(speed);
}

void Gif::setColor(QColor c)
{/*
    QVector<QPixmap> newFrames;

    for (auto pix : frames)
    {
        QImage image = pix.toImage();

        static int i = 0;
        image.save(QString("zzz-%1-before.png").arg(i));

        for(int i = 0; i < image.width(); i++)
        {
            for(int j = 0; j < image.height(); j++)
            {
                QColor color = image.pixelColor(i, j);
                color.setHsl(c.hue(), c.saturation(), c.lightness(), color.alpha());
                image.setPixelColor(i, j, color.convertTo(QColor::Rgb));
            }
        }

        newFrames.push_back(QPixmap::fromImage(image));
    }

    frames = newFrames;*/
}

void Gif::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    currentFrame = (currentFrame + 1) % frames.size();
    const QPixmap & frame = frames[currentFrame];
    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);
    update();
}
