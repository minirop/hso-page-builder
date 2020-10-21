#include "gif.h"
#include "utils.h"
#include "appsettings.h"
#include <QPainter>
#include <QBitmap>
#include <QFileInfo>
#include <QDir>
#include <array>
#include <cmath>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QApplication>

static const constexpr std::array<const char *, 41> characters = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
    "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
    "u", "v", "w", "x", "y", "z", "zz0exclam",
    "zz1quest", "zz2apost", "zz3colon", "zz4quote"
};


Gif::Gif()
{
    setFlags(ItemIsMovable | ItemIsSelectable);
    timerId = startTimer(1000 / 60);
}

void Gif::addFrame(QString filename)
{
    QPixmap pix(filename);
    events[currentEvent].originalFrames.push_back(pix);
    frames.push_back(pix);
}

void Gif::setSpeed(int speed)
{
    fps = 1.f / speed;
    fpsProgress = 0;
    AppSettings::SetPageDirty();
}

void Gif::setHSL(int h, int s, int l)
{
    events[currentEvent].H = h;
    events[currentEvent].S = s;
    events[currentEvent].L = l;

    frames.clear();
    for (auto pix : events[currentEvent].originalFrames)
    {
        frames.push_back(Utils::ChangeHSL(pix, h / 100.0f, s / 100.0f, l / 100.0f));
    }

    if (frames.size() == 1)
        timerEvent(nullptr);
    AppSettings::SetPageDirty();
}

void Gif::setFrameOffset(int f)
{
    events[currentEvent].offsetFrame = f;
    if (events[currentEvent].originalFrames.size() > 1)
    {
        currentFrame = f;
    }
    refresh();
    AppSettings::SetPageDirty();
}

void Gif::setNameOf(QString name)
{
    events[currentEvent].nameOf = name;
    AppSettings::SetPageDirty();
}

void Gif::mirror(bool active)
{
    events[currentEvent].mirrored = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
    AppSettings::SetPageDirty();
}

void Gif::flip(bool active)
{
    events[currentEvent].flipped = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
    AppSettings::SetPageDirty();
}

void Gif::setSwingOrSpin(int animation)
{
    events[currentEvent].swingOrSpin = animation;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setSwingOrSpinSpeed(int speed)
{
    events[currentEvent].swingOrSpinSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipX(bool b)
{
    events[currentEvent].flip3DX = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipXSpeed(int speed)
{
    events[currentEvent].flip3DXSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipY(bool b)
{
    events[currentEvent].flip3DY = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipYSpeed(int speed)
{
    events[currentEvent].flip3DYSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setFade(bool b)
{
    events[currentEvent].fade = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setFadeSpeed(int speed)
{
    events[currentEvent].fadeSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setSync(bool b)
{
    events[currentEvent].sync = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setGifAnimation(int animation)
{
    events[currentEvent].gifAnimation = animation;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

QPixmap Gif::unscaledPixmap() const
{
    if (currentFrame < frames.size())
    {
        return frames[currentFrame];
    }

    return frames[0];
}

bool Gif::mirrored() const
{
    return events[currentEvent].mirrored;
}

bool Gif::flipped() const
{
    return events[currentEvent].flipped;
}

int Gif::H() const
{
    return events[currentEvent].H;
}

int Gif::S() const
{
    return events[currentEvent].S;
}

int Gif::L() const
{
    return events[currentEvent].L;
}

QString Gif::nameOf() const
{
    return events[currentEvent].nameOf;
}

int Gif::swingOrSpin() const
{
    return events[currentEvent].swingOrSpin;
}

int Gif::swingOrSpinSpeed() const
{
    return events[currentEvent].swingOrSpinSpeed;
}

bool Gif::flip3DX() const
{
    return events[currentEvent].flip3DX;
}

int Gif::flip3DXSpeed() const
{
    return events[currentEvent].flip3DXSpeed;
}

bool Gif::flip3DY() const
{
    return events[currentEvent].flip3DY;
}

int Gif::flip3DYSpeed() const
{
    return events[currentEvent].flip3DYSpeed;
}

bool Gif::fade() const
{
    return events[currentEvent].fade;
}

int Gif::fadeSpeed() const
{
    return events[currentEvent].fadeSpeed;
}

bool Gif::sync() const
{
    return events[currentEvent].sync;
}

int Gif::offsetFrame() const
{
    return events[currentEvent].offsetFrame;
}

int Gif::gifAnimation() const
{
    return events[currentEvent].gifAnimation;
}

void Gif::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    float dt = 1.f / 60.f;

    if (fps > 0 && (events[currentEvent].gifAnimation == 0 || (events[currentEvent].gifAnimation == 1 && isUnderMouse())))
    {
        fpsProgress += dt;
        if (fpsProgress > fps)
        {
            currentFrame = (currentFrame + 1) % frames.size();
            fpsProgress -= fps;
        }
    }
    else if (events[currentEvent].gifAnimation == 1 && !isUnderMouse())
    {
        currentFrame = 0;
        fpsProgress = 0;
    }
    else if (events[currentEvent].gifAnimation == 3)
    {
        if (isUnderMouse())
        {
            if (QApplication::mouseButtons() == Qt::LeftButton)
            {
                currentFrame = 2;
            }
            else
            {
                currentFrame = 1;
            }
        }
        else
        {
            currentFrame = 0;
        }
    }

    if (currentFrame >= frames.size())
    {
        currentFrame = 0;
    }

    QPixmap frame = frames[currentFrame];
    QTransform transform;

    if (events[currentEvent].mirrored)
    {
        transform.scale(-1, 1);
    }

    if (events[currentEvent].flipped)
    {
        transform.scale(1, -1);
    }

    if (events[currentEvent].flip3DX)
    {
        flip3DXProgress += events[currentEvent].flip3DXSpeed * 0.1 * dt;
        transform.scale(std::sin(flip3DXProgress), 1);
    }

    if (events[currentEvent].flip3DY)
    {
        flip3DYProgress += events[currentEvent].flip3DYSpeed * 0.1 * dt;
        transform.scale(1, std::sin(flip3DYProgress));
    }

    switch (events[currentEvent].swingOrSpin)
    {
    case 1:
        swingOrSpinProgress += events[currentEvent].swingOrSpinSpeed * 0.25 * dt;
        transform.rotate(std::sin(swingOrSpinProgress) * 20);
        break;
    case 2:
        swingOrSpinProgress += events[currentEvent].swingOrSpinSpeed * 0.1 * dt;
        transform.rotateRadians(swingOrSpinProgress);
        break;
    }

    frame = frame.transformed(transform);

    setPixmap(frame);
    setOffset(-frame.width() / 2, -frame.height() / 2);
    update();
}

void Gif::resetAllAnimations()
{
    swingOrSpinProgress = 0;
    flip3DXProgress = 0;
    flip3DYProgress = 0;
    fadeProgress = 0;
}

void Gif::refresh()
{
    events[currentEvent].originalFrames.clear();
    frames.clear();
    setSpeed(0);

    auto searchPaths = AppSettings::GetSearchPaths();
    for (auto path : searchPaths)
    {
        if (QFileInfo fi(path + "/images/gifs/" + events[currentEvent].nameOf); fi.isDir())
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
        else if (QFile(path + "/images/static/" + events[currentEvent].nameOf + ".png").exists())
        {
            addFrame(path + "/images/static/" + events[currentEvent].nameOf + ".png");
            break;
        }
        else if (QFile(path + "/images/shapes/" + events[currentEvent].nameOf + ".png").exists())
        {
            addFrame(path + "/images/shapes/" + events[currentEvent].nameOf + ".png");
            break;
        }
        else if (QFileInfo fi(path + "/images/wordart/" + events[currentEvent].nameOf.toLower()); fi.isDir())
        {
            auto letter = "0";
            if (events[currentEvent].offsetFrame > 0 && events[currentEvent].offsetFrame < static_cast<int>(characters.size()))
            {
                letter = characters[events[currentEvent].offsetFrame];
                if (!QFile(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter)).exists())
                {
                    letter = "0";
                }
            }
            addFrame(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter));
            break;
        }
    }

    setHSL(events[currentEvent].H, events[currentEvent].S, events[currentEvent].L);

    // disable the timer if there is
    // a sole image with a speed set.
    if (events[currentEvent].originalFrames.size() == 1)
    {
        setSpeed(0);
        timerEvent(nullptr);
    }
}

void Gif::setEvent(QString name)
{
    if (!events.contains(name))
    {
        EventData data;
        if (events.contains(currentEvent))
        {
            data = events[currentEvent];
        }
        events[name] = data;
    }

    PageElement::setEvent(name);
}

QList<QString> Gif::activeEvents() const
{
    return events.keys();
}
