#ifndef TEXT_H
#define TEXT_H

#include "pageelement.h"
#include "globals.h"
#include <QMap>
#include <QPixmap>
#include <QGraphicsColorizeEffect>
#include <QGraphicsItem>

enum class Animation {
    None,
    TypeWriter,
    Floating,
    Marquee
};

class QSequentialAnimationGroup;
class Text : public PageElement, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Text();
    ElementType elementType() const override { return ElementType::Text; }
    void refresh() override;

    void setHSPosition(int x, int y);
    void setWidth(int w);
    void setAnimation(int anim);
    void setAnimationSpeed(int spd);
    void setAlign(int halign);
    void setString(QString str);
    void setFontSize(int size);
    void setFontBold(bool bold);
    void setFont(QString name);
    void setFontColor(QColor color);
    void setColor(QColor color);
    void setFade(QColor color, int speed);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    void timerEvent(QTimerEvent * event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void renderText(QString string);
    void regenerateFont();

    friend class MainWindow;
    friend class PageSettings;

    QVector<QPixmap> renderedTextes;
    QString string;
    int width = 0;
    int pageWidth = 0;
    int xoffset = 0;
    int align = ALIGN_LEFT;
    qreal marquee = 0;
    qreal floating = 0;
    float typewriterProgress = 0;
    int typewriterDirection = 1;
    float typewriterTimer = 100;
    bool textIsDirty = true;
    QColor fontColor = Qt::black;
    Animation animation = Animation::None;
    int animationSpeed = 0;
    QString fontName;
    int fontSize;
    bool fontBold;
    bool fontIsDirty = true;
    QMap<QChar, QPixmap> fontChars;
    int fontWidth = 0;
    int fontHeight = 0;
    QSequentialAnimationGroup * group = nullptr;
    QColor fadeColor = Qt::black;
    int fadeSpeed = 0;
    QGraphicsColorizeEffect * colorizeEffect = nullptr;
};

#endif // TEXT_H
