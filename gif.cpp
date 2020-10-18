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
    originalFrames.push_back(pix);
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
    AppSettings::SetPageDirty();
}

void Gif::setFrameOffset(int f)
{
    offsetFrame = f;
    if (originalFrames.size() > 1)
    {
        currentFrame = f;
    }
    refresh();
    AppSettings::SetPageDirty();
}

void Gif::mirror(bool active)
{
    mirrored = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
    AppSettings::SetPageDirty();
}

void Gif::flip(bool active)
{
    flipped = active;

    if (frames.size() == 1)
        timerEvent(nullptr);
    AppSettings::SetPageDirty();
}

void Gif::setSwingOrSpin(int animation)
{
    swingOrSpin = animation;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setSwingOrSpinSpeed(int speed)
{
    swingOrSpinSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipX(bool b)
{
    flip3DX = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipXSpeed(int speed)
{
    flip3DXSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipY(bool b)
{
    flip3DY = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::set3DFlipYSpeed(int speed)
{
    flip3DYSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setFade(bool b)
{
    fade = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setFadeSpeed(int speed)
{
    fadeSpeed = speed;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setSync(bool b)
{
    sync = b;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

void Gif::setGifAnimation(int animation)
{
    gifAnimation = animation;
    resetAllAnimations();
    AppSettings::SetPageDirty();
}

QPixmap Gif::unscaledPixmap() const
{
    if (currentFrame >= frames.size())
    {
        return frames[currentFrame];
    }

    return frames[0];
}

void Gif::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    float dt = 1.f / 60.f;

    if (fps > 0 && (gifAnimation == 0 || (gifAnimation == 1 && isUnderMouse())))
    {
        fpsProgress += dt;
        if (fpsProgress > fps)
        {
            currentFrame = (currentFrame + 1) % frames.size();
            fpsProgress -= fps;
        }
    }
    else if (gifAnimation == 1 && !isUnderMouse())
    {
        currentFrame = 0;
        fpsProgress = 0;
    }
    else if (gifAnimation == 3)
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

    if (mirrored)
    {
        transform.scale(-1, 1);
    }

    if (flipped)
    {
        transform.scale(1, -1);
    }

    if (flip3DX)
    {
        flip3DXProgress += flip3DXSpeed * 0.1 * dt;
        transform.scale(std::sin(flip3DXProgress), 1);
    }

    if (flip3DY)
    {
        flip3DYProgress += flip3DYSpeed * 0.1 * dt;
        transform.scale(1, std::sin(flip3DYProgress));
    }

    switch (swingOrSpin)
    {
    case 1:
        swingOrSpinProgress += swingOrSpinSpeed * 0.25 * dt;
        transform.rotate(std::sin(swingOrSpinProgress) * 20);
        break;
    case 2:
        swingOrSpinProgress += swingOrSpinSpeed * 0.1 * dt;
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
        else if (QFileInfo fi(path + "/images/wordart/" + nameOf); fi.isDir())
        {
            auto letter = "0";
            if (offsetFrame > 0 && offsetFrame < static_cast<int>(characters.size()))
            {
                letter = characters[offsetFrame];
                if (!QFile(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter)).exists())
                {
                    letter = "0";
                }
            }
            addFrame(QString("%1/%2.png").arg(fi.absoluteFilePath()).arg(letter));
            break;
        }
    }

    setHSL(H, S, L);

    // disable the timer if there is
    // a sole image with a speed set.
    if (originalFrames.size() == 1)
    {
        setSpeed(0);
        timerEvent(nullptr);
    }
}
