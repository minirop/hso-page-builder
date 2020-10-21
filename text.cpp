#include "text.h"
#include "fontdatabase.h"
#include "globals.h"
#include "appsettings.h"
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

void Text::refresh()
{
}

void Text::setEvent(QString name)
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

void Text::setHSPosition(int x, int y)
{
    events[currentEvent].xoffset = x;
    setPos((x + 50) * (PAGE_WIDTH / 100), y);
    AppSettings::SetPageDirty();
}

void Text::setWidth(int w)
{
    events[currentEvent].width = w;
    events[currentEvent].renderedWidth = w * PAGE_WIDTH / 100;
    textIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setAnimation(int anim)
{
    events[currentEvent].animation = static_cast<Animation>(anim);

    switch (events[currentEvent].animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        typewriterProgress = 0;
        events[currentEvent].typewriterDirection = 1;
        break;
    case Animation::Floating:
        events[currentEvent].floating = 0;
        break;
    case Animation::Marquee:
        events[currentEvent].marquee = 0;
        break;
    }
    textIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setAnimationSpeed(int spd)
{
    events[currentEvent].animationSpeed = spd;
    AppSettings::SetPageDirty();
}

void Text::setAlign(int halign)
{
    switch (halign)
    {
    case 0:
        events[currentEvent].align = ALIGN_LEFT;
        break;
    case 1:
        events[currentEvent].align = ALIGN_CENTRE;
        break;
    case 2:
        events[currentEvent].align = ALIGN_RIGHT;
        break;
    }
    textIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setString(QString str)
{
    events[currentEvent].string = str;
    textIsDirty = true;

    renderText(events[currentEvent].string);
    AppSettings::SetPageDirty();
}

void Text::setFontSize(int size)
{
    events[currentEvent].fontSize = size;
    fontIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setFontBold(bool bold)
{
    events[currentEvent].fontBold = bold;
    fontIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setFont(QString name)
{
    events[currentEvent].fontName = name;
    fontIsDirty = true;
    AppSettings::SetPageDirty();
}

void Text::setFontColor(QColor color)
{
    events[currentEvent].fontColor = color;
    setColor(color);
    if (events[currentEvent].fadeSpeed > 0)
    {
        setFade(events[currentEvent].fadeColor, events[currentEvent].fadeSpeed);
    }
    AppSettings::SetPageDirty();
}

void Text::setColor(QColor color)
{
    colorizeEffect->setColor(color);
    textIsDirty = true;
}

void Text::setFade(QColor color, int speed)
{
    events[currentEvent].fadeColor = color;
    events[currentEvent].fadeSpeed = speed;

    if (group)
    {
        group->deleteLater();
        group = nullptr;
    }

    if (speed == 0)
    {
        setColor(events[currentEvent].fontColor);
        AppSettings::SetPageDirty();
        return;
    }

    group = new QSequentialAnimationGroup(this);
    auto fadeIn = new QVariantAnimation;
    fadeIn->setStartValue(events[currentEvent].fontColor);
    fadeIn->setEndValue(color);
    fadeIn->setDuration(1000 * speed / 60.f / 2);
    connect(fadeIn, &QVariantAnimation::valueChanged, [&](const QVariant & value) {
        setColor(value.value<QColor>());
    });

    auto fadeOut = new QVariantAnimation;
    fadeOut->setStartValue(color);
    fadeOut->setEndValue(events[currentEvent].fontColor);
    fadeOut->setDuration(1000 * speed / 60.f / 2);
    connect(fadeOut, &QVariantAnimation::valueChanged, [&](const QVariant & value) {
        setColor(value.value<QColor>());
    });

    group->addAnimation(fadeIn);
    group->addAnimation(fadeOut);
    group->setLoopCount(-1);

    group->start();
    AppSettings::SetPageDirty();
}

void Text::setNoContent(bool b)
{
    events[currentEvent].noContent = b;
    AppSettings::SetPageDirty();
}

QString Text::string() const
{
    return events[currentEvent].string;
}

int Text::width() const
{
    return events[currentEvent].width;
}

int Text::renderedWidth() const
{
    return events[currentEvent].renderedWidth;
}

int Text::xoffset() const
{
    return events[currentEvent].xoffset;
}

int Text::align() const
{
    return events[currentEvent].align;
}

qreal Text::marquee() const
{
    return events[currentEvent].marquee;
}

qreal Text::floating() const
{
    return events[currentEvent].floating;
}

int Text::typewriterDirection() const
{
    return events[currentEvent].typewriterDirection;
}

float Text::typewriterTimer() const
{
    return events[currentEvent].typewriterTimer;
}

QColor Text::fontColor() const
{
    return events[currentEvent].fontColor;
}

Animation Text::animation() const
{
    return events[currentEvent].animation;
}

int Text::animationSpeed() const
{
    return events[currentEvent].animationSpeed;
}

QString Text::fontName() const
{
    return events[currentEvent].fontName;
}

int Text::fontSize() const
{
    return events[currentEvent].fontSize;
}

bool Text::fontBold() const
{
    return events[currentEvent].fontBold;
}

QMap<QChar, QPixmap> Text::fontChars() const
{
    return events[currentEvent].fontChars;
}

int Text::fontWidth() const
{
    return events[currentEvent].fontWidth;
}

int Text::fontHeight() const
{
    return events[currentEvent].fontHeight;
}

QColor Text::fadeColor() const
{
    return events[currentEvent].fadeColor;
}

int Text::fadeSpeed() const
{
    return events[currentEvent].fadeSpeed;
}

bool Text::noContent() const
{
    return events[currentEvent].noContent;
}

QRectF Text::boundingRect() const
{
    qreal floatingOffset = 0.0;
    if (events[currentEvent].animation == Animation::Floating)
    {
        constexpr auto PI_180 = M_PI / 180;
        floatingOffset = (std::sin(events[currentEvent].floating * PI_180) * renderedTextes.size() * events[currentEvent].fontHeight * 0.25);
    }
    return QRectF { -events[currentEvent].renderedWidth / 2.0, floatingOffset, qreal(events[currentEvent].renderedWidth), qreal(renderedTextes.size() * events[currentEvent].fontHeight) };
}

void Text::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    auto rect = option->rect;

    switch (events[currentEvent].animation)
    {
    case Animation::None:
    case Animation::TypeWriter:
    case Animation::Floating:
    {
        int y = 0;
        for (auto & renderedText : renderedTextes)
        {
            int x = 0;
            switch (events[currentEvent].align)
            {
            case ALIGN_LEFT:
                x = rect.left();
                break;
            case ALIGN_CENTRE:
                x = (rect.width() - renderedText.width()) / 2 + rect.left();
                break;
            case ALIGN_RIGHT:
                x = rect.right() - renderedText.width();
                break;
            }

            painter->drawPixmap(x, rect.top() + y, renderedText);
            y += events[currentEvent].fontHeight;
        }
        break;
    }
    case Animation::Marquee:
        painter->drawPixmap(events[currentEvent].marquee, rect.top(), renderedTextes.first());
        break;
    }
}

void Text::timerEvent(QTimerEvent * event)
{
    Q_UNUSED(event)

    switch (events[currentEvent].animation)
    {
    case Animation::None:
        break;
    case Animation::TypeWriter:
        events[currentEvent].typewriterTimer -= events[currentEvent].animationSpeed;

        if (events[currentEvent].typewriterTimer < 0)
        {
            typewriterProgress += events[currentEvent].typewriterDirection;

            if (typewriterProgress >= events[currentEvent].string.length() && events[currentEvent].typewriterDirection > 0)
            {
                events[currentEvent].typewriterTimer = 800;
                events[currentEvent].typewriterDirection = -1;
            }
            else if (typewriterProgress < 0 && events[currentEvent].typewriterDirection < 0)
            {
                events[currentEvent].typewriterTimer = 800;
                events[currentEvent].typewriterDirection = 1;
            }
            else
            {
                events[currentEvent].typewriterTimer = 100;
            }
        }

        typewriterProgress = std::clamp(typewriterProgress, 0.0f, float(events[currentEvent].string.length()));
        textIsDirty = true;
        break;
    case Animation::Floating:
        events[currentEvent].floating += events[currentEvent].animationSpeed * 0.4;
        break;
    case Animation::Marquee:
    {
        events[currentEvent].marquee -= events[currentEvent].animationSpeed / 10.0;

        if (events[currentEvent].marquee < -events[currentEvent].renderedWidth/2 - renderedTextes[0].width())
        {
            events[currentEvent].marquee = x() + events[currentEvent].renderedWidth / 2;
        }
        break;
    }
    }

    renderText(events[currentEvent].string);

    update();
}

QVariant Text::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        QPointF newPos = value.toPointF();
        newPos.setX((events[currentEvent].xoffset + 50) * (PAGE_WIDTH / 100));
        return newPos;
    }
    return QGraphicsItem::itemChange(change, value);
}

void Text::renderText(QString string)
{
    if (fontIsDirty)
    {
        regenerateFont();
    }

    if (!textIsDirty) return;

    auto & font = FontDatabase::GetFont(QString("%1%2%3").arg(events[currentEvent].fontName).arg(events[currentEvent].fontSize).arg(events[currentEvent].fontBold ? 'b' : 'n'));

    QStringList lines;

    if (events[currentEvent].animation != Animation::Marquee)
    {
        auto str = string;
        if (events[currentEvent].animation == Animation::TypeWriter)
        {
            str = str.left(typewriterProgress);
        }
        for (int xx = 0, www = events[currentEvent].renderedWidth, index = 0; index < str.size(); )
        {
            auto c = str[index];
            xx += font.getWidth(c.toLatin1(), events[currentEvent].fontWidth);
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
        auto newText = QPixmap(events[currentEvent].fontWidth * line.length(), events[currentEvent].fontHeight);
        newText.fill(Qt::transparent);

        QPainter painter;
        painter.begin(&newText);
        int xx = 0;
        for (int yy = 0; auto c : line)
        {
            painter.drawPixmap(xx, yy, events[currentEvent].fontChars[c.toLatin1()]);
            xx += font.getWidth(c.toLatin1(), events[currentEvent].fontWidth);
        }
        painter.end();
        renderedTextes.push_back(newText.copy(0, 0, xx, events[currentEvent].fontHeight));
    }

    textIsDirty = false;
}

void Text::regenerateFont()
{
    static const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,;:?!-_~#\"'&()[]|`\\/@°+=*€$$<> ";
    QPixmap pix = FontDatabase::GetFontAtlas(QString("%1%2%3").arg(events[currentEvent].fontName).arg(events[currentEvent].fontSize).arg(events[currentEvent].fontBold ? 'b' : 'n'));
    assert(!pix.isNull());
    events[currentEvent].fontWidth = pix.width() / 8;
    events[currentEvent].fontHeight = pix.height() / 12;

    for (int xx = 0, yy = 0; auto c : alphabet)
    {
        events[currentEvent].fontChars[c] = pix.copy(xx * events[currentEvent].fontWidth, yy * events[currentEvent].fontHeight, events[currentEvent].fontWidth, events[currentEvent].fontHeight);

        xx++;
        if (xx >= 8)
        {
            yy++;
            xx = 0;
        }
    }

    textIsDirty = true;
    fontIsDirty = false;
}
