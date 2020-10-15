#include "gif.h"
#include "utils.h"
#include "appsettings.h"
#include <QPainter>
#include <QBitmap>
#include <QFileInfo>
#include <QDir>

#include <QGraphicsView>
#include <QGraphicsScene>

Gif::Gif()
{
    setFlags(ItemIsMovable | ItemIsSelectable);
}

void Gif::addFrame(QString filename)
{
    QPixmap pix(filename);
    originalFrames.push_back(pix);
    frames.push_back(pix);

    if (originalFrames.size() == 1)
        timerEvent(nullptr);
}

void Gif::setSpeed(int speed)
{
    if (timerId != -1)
        killTimer(timerId);
    fps = speed;
    if (speed > 0)
        timerId = startTimer(1000 / speed);
    else
        timerId = -1;
}

void Gif::setHSL(int h, int s, int l)
{
    H = h;
    S = s;
    L = l;

    frames.clear();
    for (auto pix : originalFrames)
    {
        frames.push_back(Utils::ChangeHSL(pix, h / 100.0f, s / 100.0f, l / 100.0f));
    }

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::mirror(bool active)
{
    mirrored = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::flip(bool active)
{
    flipped = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
}

void Gif::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    currentFrame = (currentFrame + 1) % frames.size();
    QPixmap frame = frames[currentFrame];

    if (mirrored || flipped)
    {
        frame = frame.transformed(QTransform().scale(mirrored ? -1 : 1, flipped ? -1 : 1));
    }

    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);
    update();
}

void Gif::refresh()
{
    originalFrames.clear();
    frames.clear();
    setSpeed(0);

    auto searchPaths = AppSettings::GetSearchPaths();
    for (auto path : searchPaths)
    {
        if (QFileInfo fi(path + "/images/gifs/" + nameOf); fi.isDir())
        {
            QDir dir(fi.absoluteFilePath());
            for (auto entry : dir.entryInfoList(QDir::Files, QDir::Name))
            {
                if (entry.suffix() == "speed")
                {
                    auto spd = entry.baseName().toInt();
                    setSpeed(spd);
                }
                else
                {
                    addFrame(entry.absoluteFilePath());
                }
            }
            break;
        }
        else if (QFile(path + "/images/static/" + nameOf + ".png").exists())
        {
            addFrame(path + "/images/static/" + nameOf + ".png");
            break;
        }
        else if (QFile(path + "/images/shapes/" + nameOf + ".png").exists())
        {
            addFrame(path + "/images/shapes/" + nameOf + ".png");
            break;
        }
    }

    // disable the timer if there is
    // a sole image with a speed set.
    if (originalFrames.size() == 1)
    {
        setSpeed(0);
    }

    setHSL(H, S, L);
}
