#ifndef GIF_H
#define GIF_H

#include "pageelement.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>

class Gif : public PageElement, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Gif();
    ElementType elementType() const override { return ElementType::Gif; }

    void addFrame(QString filename);
    void setSpeed(int speed);
    void setColor(QColor c);

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    QVector<QPixmap> frames;
    int currentFrame = 0;
};

#endif // GIF_H
