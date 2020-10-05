#include "page.h"
#include <QPainter>
#include <QPushButton>
#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsItem>

Page::Page(QWidget * parent)
    : QGraphicsView(parent)
{
    setFixedWidth(300 * ZOOM);
    scale(ZOOM, ZOOM);
    setLineCount(10);
    startTimer(60);

    setCacheMode(QGraphicsView::CacheBackground);
    setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
    setResizeAnchor(QGraphicsView::NoAnchor);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene;
    connect(scene, &QGraphicsScene::selectionChanged, [&]() {
        auto items = scene->selectedItems();
        if (items.size())
        {
            auto item = items.first();
            auto id = item->data(Qt::UserRole).toInt();

            emit selected(id);
        }
    });
    setScene(scene);

    parent->installEventFilter(this);
}

void Page::setLineCount(int lineCount)
{
    setFixedHeight(lineCount * 32 * ZOOM);
    update();

    linesCount = lineCount;
}

void Page::setBackground(QString image)
{
    setBackgroundBrush(QPixmap(image));

    update();
}

void Page::addElement(QGraphicsItem * element)
{
    scene->addItem(element);
}

bool Page::eventFilter(QObject * watched, QEvent * event)
{
    if (event->type() == QEvent::Wheel)
    {
        auto wheel = static_cast<QWheelEvent*>(event);
        wheelEvent(wheel);
        return true;
    }

    return QGraphicsView::eventFilter(watched, event);
}

void Page::wheelEvent(QWheelEvent * event)
{
    if (event->angleDelta().y() > 0)
    {
        if (topLine > 0)
        {
            topLine--;
            move(0, topLine * -32 * ZOOM);
        }
    }
    else if (event->angleDelta().y() < 0)
    {
        if (topLine < linesCount - 1)
        {
            topLine++;
            move(0, topLine * -32 * ZOOM);
        }
    }

    event->accept();
}
