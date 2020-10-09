#include "text.h"
#include "fontdatabase.h"
#include "globals.h"
#include <QPainter>
#include <QFileInfo>
#include <QBitmap>
#include <cmath>
#include <QStyleOptionGraphicsItem>
#include <QSequentialAnimationGroup>
#include <QVariantAnimation>

Text::Text()
{
    startTimer(16);

    colorizeEffect = new QGraphicsColorizeEffect(this);
    colorizeEffect->setColor(Qt::black);
    setGraphicsEffect(colorizeEffect);

    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

void Text::setWidth(int w)
{
    width = w;
}

void Text::setAnimation(int anim)
{
    animation = static_cast<Animation>(anim);

    switch (animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        typewriterProgress = 0;
        typewriterDirection = 1;
        break;
    case Animation::Floating:
        floating = 0;
        break;
    case Animation::Marquee:
        marquee = 0;
        break;
    }
    textIsDirty = true;
}

void Text::setAnimationSpeed(int spd)
{
    animationSpeed = spd;
}

void Text::setAlign(int halign)
{
    switch (halign)
    {
    case 0:
        align = Qt::AlignLeft;
        break;
    case 1:
        align = Qt::AlignHCenter;
        break;
    case 2:
        align = Qt::AlignRight;
        break;
    }
    textIsDirty = true;
}

void Text::setString(QString str)
{
    string = str;
    textIsDirty = true;

    renderText(string);
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
    qreal floatingOffset = 0.0;
    if (animation == Animation::Floating)
    {
        constexpr auto PI_180 = M_PI / 180;
        floatingOffset = (std::sin(floating * PI_180) * renderedTextes.size() * fontHeight * 0.25);
    }
    return QRectF { -width / 2.0, floatingOffset, qreal(width), qreal(renderedTextes.size() * fontHeight) };
}

void Text::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    auto rect = option->rect;

    switch (animation)
    {
    case Animation::None:
    case Animation::TypeWriter:
    case Animation::Floating:
    {
        int y = 0;
        for (auto & renderedText : renderedTextes)
        {
            int x = 0;
            switch (align)
            {
            case Qt::AlignLeft:
                x = rect.left();
                break;
            case Qt::AlignHCenter:
                x = (rect.width() - renderedText.width()) / 2 + rect.left();
                break;
            case Qt::AlignRight:
                x = rect.right() - renderedText.width();
                break;
            }

            painter->drawPixmap(x, rect.top() + y, renderedText);
            y += fontHeight;
        }
        break;
    }
    case Animation::Marquee:
        painter->drawPixmap(marquee, rect.top(), renderedTextes.first());
        break;
    }
}

void Text::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event)

    switch (animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        typewriterProgress += typewriterDirection * 0.5f;

        if (typewriterProgress >= string.length() && typewriterDirection > 0)
        {
            typewriterDirection = -1;
        }
        else if (typewriterProgress < 0 && typewriterDirection < 0)
        {
            typewriterDirection = 1;
        }

        typewriterProgress = std::clamp(typewriterProgress, 0.0f, float(string.length()));
        textIsDirty = true;
        break;
    case Animation::Floating:
        floating += animationSpeed * 0.4;
        break;
    case Animation::Marquee:
    {
        marquee -= animationSpeed / 10.0;

        if (marquee < -width/2 - renderedTextes[0].width())
        {
            marquee = x() + width / 2;
        }
        break;
    }
    }

    renderText(string);

    update();
}

QVariant Text::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        QPointF newPos = value.toPointF();
        newPos.setX((xoffset + 100) * (PAGE_WIDTH / 200.0));
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}

void Text::renderText(QString string)
{
    if (!textIsDirty) return;

    auto & font = FontDatabase::GetFont(fontName);

    QStringList lines;

    if (animation != Animation::Marquee)
    {
        auto str = string;
        if (animation == Animation::TypeWriter)
        {
            str = str.left(typewriterProgress);
        }
        for (int xx = 0, www = width, index = 0; auto c : str)
        {
            xx += font.getWidth(c.toLatin1(), fontWidth);
            index++;

            if (xx >= www)
            {
                auto space = str.lastIndexOf(' ', index);
                if (space != -1)
                {
                    lines.push_back(str.left(space).trimmed());
                    str = str.mid(space).trimmed();
                    xx = 0;
                    index = 0;
                }
            }
        }
        lines.push_back(str);
    }
    else
    {
        lines.push_back(string);
    }

    renderedTextes.clear();
    for (auto line : lines)
    {
        if (line.length() == 0)
        {
            // so the selected marker doesn't collapse and stays at "1 line minimum".
            line = " ";
        }
        auto newText = QPixmap(fontWidth * line.length(), fontHeight);
        newText.fill(Qt::transparent);

        QPainter painter;
        painter.begin(&newText);
        int xx = 0;
        for (int yy = 0; auto c : line)
        {
            painter.drawPixmap(xx, yy, fontChars[c.toLatin1()]);
            xx += font.getWidth(c.toLatin1(), fontWidth);
        }
        painter.end();
        renderedTextes.push_back(newText.copy(0, 0, xx, fontHeight));
    }

    textIsDirty = false;
}
