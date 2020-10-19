#include "imageslider.h"
#include "appsettings.h"
#include <QPainter>
#include <QSet>
#include <QDir>

ImageSlider::ImageSlider(QWidget *parent) : QWidget(parent)
{
    setFixedSize(150, 100);
    backgrounds.push_back(QString());

    refresh();
}

void ImageSlider::setImage(QString image)
{
    auto oldCurrent = current;
    current = backgrounds.indexOf(image);
    if (current != oldCurrent)
    {
        auto name = backgrounds[current];
        bgPixmap = getPixmap(name);
        emit backgroundChanged(name);
    }
    update();
}

void ImageSlider::setEmptyColor(QColor color)
{
    emptyColor = color;

    // update only if no backgrounds selected
    if (current == 0)
        update();
}

void ImageSlider::refresh()
{
    auto currentName = backgrounds[current];

    QSet<QString> uniqueBG;
    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        auto files = QDir(path + "/images/bgs").entryList(QStringList() << "*.png", QDir::Files);
        for (auto f : files)
        {
            uniqueBG.insert(f);
        }
    }

    backgrounds = uniqueBG.values();
    backgrounds.sort();
    backgrounds.prepend(QString()); // "no backgrounds"

    current = backgrounds.indexOf(currentName);
    if (current == -1)
    {
        current = 0;
    }

    update();
}

void ImageSlider::prev()
{
    current--;
    if (current < 0)
    {
        current = backgrounds.size() - 1;
    }

    auto name = backgrounds[current];
    bgPixmap = getPixmap(name);
    emit backgroundChanged(name);

    update();
}

void ImageSlider::next()
{
    current = (current + 1) % backgrounds.size();

    auto name = backgrounds[current];
    bgPixmap = getPixmap(name);
    emit backgroundChanged(name);

    update();
}

void ImageSlider::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)

    QPainter p(this);
    if (current)
    {
        p.drawPixmap(0, 0, 150, 100, bgPixmap);
    }
    else
    {
        p.fillRect(0, 0, 150, 100, emptyColor);
    }
}

QPixmap ImageSlider::getPixmap(QString name)
{
    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        auto img = path + "/images/bgs/" + name;
        if (QFile(img).exists())
        {
            return QPixmap(img);
        }
    }

    return QPixmap();
}
