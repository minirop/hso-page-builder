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

    void setLineCount(int lineCount);
    void setBackground(QString image);

    void addElement(QGraphicsItem *element);
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:
    void selected(int id);
    void clearSelection();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QPixmap background;
    int topLine = 0;
    int linesCount = 0;
    QGraphicsScene * scene;
};

#endif // PAGE_H
