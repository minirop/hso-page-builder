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

    void setEvent(QString name) override;
    void clearEvent(QString name) override;
    QList<QString> activeEvents() const override;

    void addFrame(QString filename);
    void setSpeed(int speed);
    void setHSL(int h, int s, int l);
    void setFrameOffset(int f);

    void setNameOf(QString name);

    void setHSRotation(int angle);
    int HSRotation() const;

    void setHSScale(float scale);
    float HSScale() const;

    void setPosition(int x, int y);
    int HSX() const;
    int HSY() const;

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

    QPixmap unscaledPixmap() const;

    bool mirrored() const;
    bool flipped() const;
    int H() const;
    int S() const;
    int L() const;
    QString nameOf() const;
    int swingOrSpin() const;
    int swingOrSpinSpeed() const;
    bool flip3DX() const;
    int flip3DXSpeed() const;
    bool flip3DY() const;
    int flip3DYSpeed() const;
    bool fade() const;
    int fadeSpeed() const;
    bool sync() const;
    int offsetFrame() const;
    int gifAnimation() const;

protected:
    void timerEvent(QTimerEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    friend class MainWindow;
    friend class GifSlider;
    friend class PageSettings;

    void resetAllAnimations();
    void resetProgress();

    struct EventData {
        int x = 0;
        int y = 0;
        bool mirrored = false;
        bool flipped = false;
        int H = 0, S = 100, L = 100;
        QString nameOf;
        int angle = 0;
        float scale = 1;
        int swingOrSpin = 0;
        int swingOrSpinSpeed = 0;
        bool flip3DX = false;
        int flip3DXSpeed = 0;
        bool flip3DY = false;
        int flip3DYSpeed = 0;
        bool fade = false;
        int fadeSpeed = 0;
        bool sync = false;
        int offsetFrame = 0;
        int gifAnimation = 0;
        QVector<QPixmap> originalFrames;
    };

    QMap<QString, EventData> events;

    QVector<QPixmap> frames;
    int currentFrame = 0;
    float swingOrSpinProgress = 0;
    float flip3DXProgress = 0;
    float flip3DYProgress = 0;
    float fadeProgress = 0;
    float fps = 0;
    float fpsProgress = 0;
    int timerId = -1;
};

#endif // GIF_H
