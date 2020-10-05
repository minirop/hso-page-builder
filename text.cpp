#include "text.h"
#include "fontdatabase.h"
#include <QPainter>
#include <QFileInfo>
#include <QBitmap>
#include <cmath>
#include <QStyleOptionGraphicsItem>
#include <QSequentialAnimationGroup>
#include <QVariantAnimation>

static const int pad = 1.0f;

void qt_graphicsItem_highlightSelected(QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
        fgcolor.red()   > 127 ? 0 : 255,
        fgcolor.green() > 127 ? 0 : 255,
        fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

Text::Text()
{
    startTimer(16);

    colorizeEffect = new QGraphicsColorizeEffect(this);
    colorizeEffect->setColor(Qt::black);
    setGraphicsEffect(colorizeEffect);

    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void Text::setWidth(int w)
{
    width = w;
}

void Text::setAnimation(int anim, int spd)
{
    animation = static_cast<Animation>(anim);
    animationSpeed = spd;

    switch (animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        break;
    case Animation::Floating:
        break;
    case Animation::Marquee:
        marquee = x() - width / 2;
        break;
    }
}

void Text::setAlign(int align)
{
    switch (align)
    {
    case 0:
        align = Qt::AlignLeft;
        break;
    case 1:
        align = Qt::AlignVCenter;
        break;
    case 2:
        align = Qt::AlignRight;
        break;
    }
}

void Text::setString(QString str)
{
    string = str;
}

void Text::setFont(QString filename)
{
    static const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,;:?!-_~#\"'&()[]|`\\/@°+=*€$$<> ";
    QPixmap pix(filename);
    assert(!pix.isNull());
    fontWidth = pix.width() / 8;
    fontHeight = pix.height() / 12;
    fontName = QFileInfo(filename).baseName();

    for (int xx = 0, yy = 0; auto c : alphabet)
    {
        fontChars[c] = pix.copy(xx * fontWidth, yy * fontHeight, fontWidth, fontHeight);

        xx++;
        if (xx >= 8)
        {
            yy++;
            xx = 0;
        }
    }
}

void Text::setFontColor(QColor color)
{
    fontColor = color;
    setColor(color);
}

void Text::setColor(QColor color)
{
    colorizeEffect->setColor(color);
    textIsDirty = true;
}

void Text::setFade(QColor color, int speed)
{
    auto group = new QSequentialAnimationGroup(this);
    auto fadeIn = new QVariantAnimation;
    fadeIn->setStartValue(fontColor);
    fadeIn->setEndValue(color);
    fadeIn->setDuration(1000 * speed / 60.f / 2);
    connect(fadeIn, &QVariantAnimation::valueChanged, [&](const QVariant & value) {
        setColor(value.value<QColor>());
    });

    auto fadeOut = new QVariantAnimation;
    fadeOut->setStartValue(color);
    fadeOut->setEndValue(fontColor);
    fadeOut->setDuration(1000 * speed / 60.f / 2);
    connect(fadeOut, &QVariantAnimation::valueChanged, [&](const QVariant & value) {
        setColor(value.value<QColor>());
    });

    group->addAnimation(fadeIn);
    group->addAnimation(fadeOut);
    group->setLoopCount(-1);

    group->start();
}

QRectF Text::boundingRect() const
{
    qreal pw = 0;
    if (isSelected())
        pw = pad;
    return QRectF { -width / 2.0, 0 + (std::cos(floating)) * 5, qreal(width), qreal(renderedText.height()) }.adjusted(-pw, -pw, pw, pw);
}

void Text::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    auto rect = option->rect.adjusted(pad, pad, -pad, -pad);

    switch (animation)
    {
    case Animation::None:
    case Animation::TypeWriter:
        painter->drawPixmap(rect.left(), rect.top(), renderedText);
        break;
    case Animation::Floating:
        painter->drawPixmap(rect.left(), rect.top(), renderedText);
        break;
    case Animation::Marquee:
        painter->drawPixmap(marquee, rect.top(), renderedText);
        break;
    }

    if (isSelected())
        qt_graphicsItem_highlightSelected(this, painter, option);
}

void Text::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event)

    switch (animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        break;
    case Animation::Floating:
        floating += 0.1f;
        break;
    case Animation::Marquee:
    {
        marquee -= animationSpeed / 2;
        if (marquee + renderedText.width() < x() - width/2)
        {
            marquee = x() + width/2;
        }
        break;
    }
    }

    renderText(string);

    update();
}

void Text::renderText(QString string)
{
    if (!textIsDirty) return;

    auto newText = QPixmap(width, 1000);
    newText.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&newText);

    auto & font = FontDatabase::GetFont(fontName);

    QVector<int> splits;

    if (animation != Animation::Marquee)
    {
        QVector<int> cumulativeWidths;
        int www = width;
        for (int xx = 0, index = 0; auto c : string)
        {
            xx += font.getWidth(c.toLatin1(), fontWidth);
            cumulativeWidths.push_back(xx);
            index++;

            if (xx >= www)
            {
                auto space = string.lastIndexOf(' ', index);
                if (space == -1)
                {
                    space = index;
                }

                splits.push_back(space + 1);
                www = cumulativeWidths[space - 1] + width;
            }
        }
    }

    for (int xx = 0, yy = 0, index = 0; auto c : string)
    {
        painter.drawPixmap(xx, yy, fontChars[c.toLatin1()]);
        index++;
        xx += font.getWidth(c.toLatin1(), fontWidth);
        if (splits.contains(index))
        {
            yy += fontHeight;
            xx = 0;
        }
    }

    painter.end();

    renderedText = newText.copy(0, 0, newText.width(), (splits.size() + 1) * fontHeight);

    textIsDirty = false;
}
