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
    if (timerId != -1)
        killTimer(timerId);
    fps = speed;
    timerId = startTimer(1000 / speed);
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

void Gif::refresh()
{
    frames.clear();

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

    setHSL(H, S, L);
}
