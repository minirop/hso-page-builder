#ifndef GIF_H
#define GIF_H

#include "pageelement.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>

class Gif : public PageElement, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Gif();
    ElementType elementType() const override { return ElementType::Gif; }
    void refresh() override;

    void addFrame(QString filename);
    void setSpeed(int speed);
    void setHSL(int h, int s, int l);
    void setFrameOffset(int f);

    void mirror(bool active);
    void flip(bool active);

    void setSwingOrSpin(int animation);
    void setSwingOrSpinSpeed(int speed);

    void set3DFlipX(bool b);
    void set3DFlipXSpeed(int speed);

    void set3DFlipY(bool b);
    void set3DFlipYSpeed(int speed);

    void setFade(bool b);
    void setFadeSpeed(int speed);

    void setSync(bool b);
    void setGifAnimation(int animation);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    friend class MainWindow;
    friend class GifSlider;
    friend class PageSettings;

    QVector<QPixmap> frames;
    int currentFrame = 0;
    bool mirrored = false;
    bool flipped = false;
    int H = 0, S = 100, L = 100;
    QString nameOf;
    int swingOrSpin = 0;
    int swingOrSpinSpeed = 0;
    bool flip3DX = false;
    int flip3DXSpeed = 0;
    bool flip3DY = false;
    int flip3DYSpeed = 0;
    bool fade = false;
    int fadeSpeed = 0;
    bool sync = false;
    int gifAnimation = 0;
    int fps = 0;
    int offsetFrame = 0;
    int timerId = -1;
    QVector<QPixmap> originalFrames;
};

#endif // GIF_H
