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
    fps = speed;
    startTimer(1000 / speed);
}

void Gif::setHSL(int h, int s, int l)
{
    H = h;
    S = s;
    L = l;

    QVector<QPixmap> newFrames;

    for (auto pix : frames)
    {
        newFrames.push_back(Utils::ChangeHSL(pix, h / 100.0f, s / 100.0f, l / 100.0f));
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
