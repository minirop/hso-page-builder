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

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    friend class MainWindow;
    friend class GifSlider;
    friend class PageSettings;

    QVector<QPixmap> frames;
    int currentFrame = 0;
    int H = 0, S = 100, L = 100;
    QString nameOf;
    int fps = 0;
    int timerId = -1;
};

#endif // GIF_H
