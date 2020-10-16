#include "page.h"
#include "globals.h"
#include "appsettings.h"
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
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setResizeAnchor(QGraphicsView::NoAnchor);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene(this);
    connect(scene, &QGraphicsScene::selectionChanged, [&]() {
        auto items = scene->selectedItems();
        if (items.size())
        {
            auto item = items.first();
            selectedItem = item;
        }
        else
        {
            selectedItem = nullptr;
            selectedName.clear();
        }
    });
    setScene(scene);

    parent->installEventFilter(this);
}

Page::~Page()
{
    // crash without
    scene->disconnect();
}

void Page::setLineCount(int lineCount)
{
    setFixedHeight(lineCount * 32 * ZOOM);
    update();

    linesCount = lineCount;
}

void Page::setBackground(QString image)
{
    background.clear();

    if (!image.isEmpty())
    {
        auto searchPaths = AppSettings::GetSearchPaths();
        for (auto path : searchPaths)
        {
            auto img = path + "/images/bgs/" + image;
            if (QFile(img).exists())
            {
                background = image;
                setBackgroundBrush(QPixmap(img));
                assert(!QPixmap(img).isNull());
                break;
            }
        }
    }

    if (background.isEmpty())
    {
        setBackgroundColor(backgroundColor);
    }

    update();
}

void Page::setBackgroundColor(QColor color)
{
    backgroundColor = color;

    if (background.isEmpty())
    {
        setBackgroundBrush(color);
    }

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

void Page::setSelectedName(QString name)
{
    selectedName = name;
}

void Page::setTitle(QString newTitle)
{
    title = newTitle;
}

void Page::setOwner(QString newOwner)
{
    username = newOwner;
}

void Page::setDescription(QString description)
{
    descriptionAndTags = description;
}

void Page::drawForeground(QPainter * painter, const QRectF & rect)
{
    Q_UNUSED(rect)

    if (selectedItem)
    {
        auto f = font();
        f.setPixelSize(8);

        QFontMetrics fm(f);
        int width = fm.horizontalAdvance(selectedName);

        // draw background
        painter->save();
        QPen p(QColor(0x94, 0xc4, 0xf0));
        p.setWidth(2);
        painter->setPen(p);
        auto bounds = selectedItem->sceneBoundingRect();
        auto b = bounds.adjusted(-2, -2, 2, 2);
        painter->drawRect(b);
        painter->fillRect(b.left() - 1, b.top() - 10, width + 10, 10, p.color());
        painter->restore();

        // draw text
        painter->save();
        painter->setPen(Qt::black);
        painter->setFont(f);
        painter->drawText(b.left() + 5, b.top() - 3, selectedName);
        painter->restore();
    }
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

void Page::mousePressEvent(QMouseEvent * event)
{
    event->accept();

    lastMousePosition = event->localPos();
}

void Page::mouseReleaseEvent(QMouseEvent * event)
{
    event->accept();
}

void Page::mouseDoubleClickEvent(QMouseEvent * event)
{
    event->accept();

    auto foundItems = scene->items(mapToScene(event->pos()), Qt::IntersectsItemBoundingRect);
    if (foundItems.size())
    {
        auto item = foundItems.first();
        selectedItem = item;
        auto id = item->data(ROLE_ID).toInt();
        emit selected(id);
    }
}

void Page::mouseMoveEvent(QMouseEvent * event)
{
    event->accept();

    if (selectedItem)
    {
        auto pos = event->localPos();
        auto diff = (pos - lastMousePosition) / 2;
        selectedItem->moveBy(diff.x(), diff.y());
        lastMousePosition = pos;
    }
}
