#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QGraphicsView>
#include "pageelement.h"

constexpr int ZOOM = 2;

class Page : public QGraphicsView
{
    Q_OBJECT

public:
    Page(QWidget * parent);
    virtual ~Page();

    void setLineCount(int lineCount);
    void setBackground(QString image);
    void setBackgroundColor(QColor color);

    void addElement(QGraphicsItem *element);
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void selected(int id);
    void clearSelection();

public slots:
    void setSelectedName(QString name);
    void setTitle(QString newTitle);
    void setOwner(QString newOwner);
    void setDescription(QString description);

protected:
    void drawForeground(QPainter * painter, const QRectF & rect) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    friend class MainWindow;

    QString background;
    QColor backgroundColor;
    int topLine = 0;
    int linesCount = 0;
    QString title;
    QString username;
    QString music;
    QString descriptionAndTags;
    QGraphicsScene * scene;
    QGraphicsItem * selectedItem = nullptr;
    QString selectedName;
};

#endif // PAGE_H
