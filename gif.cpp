#include "gif.h"
#include "utils.h"
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

void Gif::setHSL(float h, float s, float l)
{
    QVector<QPixmap> newFrames;

    for (auto pix : frames)
    {
        newFrames.push_back(Utils::ChangeHSL(pix, h, s, l));
    }

    frames = newFrames;

    if (frames.size() == 1)
        timerEvent(nullptr);
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
