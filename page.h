#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QGraphicsView>
#include <QMap>
#include "pageelement.h"

constexpr int ZOOM = 2;

class Page : public QGraphicsView
{
    Q_OBJECT

public:
    Page(QWidget * parent);
    virtual ~Page();

    void addElement(QGraphicsItem *element);
    bool eventFilter(QObject *watched, QEvent *event) override;

    QStringList activeEvents() const;
    void moveActiveEvent(int from, int to);

    QString background();
    QColor backgroundColor();
    int linesCount();
    QString title();
    QString owner();
    QString music();
    QString description();
    QString onLoadScript();
    int cursor();
    int pageStyle();

signals:
    void selected(int id);

public slots:
    void setEvent(QString name);
    void setSelectedName(QString name);
    void setTitle(QString newTitle);
    void setOwner(QString newOwner);
    void setDescription(QString description);
    void setMusic(QString newMusic);
    void setPageCursor(int newCursor);
    void setLineCount(int lineCount);
    void setBackground(QString image);
    void setBackgroundColor(QColor color);
    void setHomePage(bool b);
    void setOnLoadScript(QString scpt);
    void setPageStyle(int style);
    void clearEvent(QString name);

protected:
    void drawForeground(QPainter * painter, const QRectF & rect) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseDoubleClickEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

private:
    friend class MainWindow;

    struct EventData {
        QString background {};
        QColor backgroundColor = Qt::black;
        int linesCount = 0;
        QString title {};
        QString music {};
        QString descriptionAndTags {};
        QString onLoadScript {};
        int cursor = 0;
        int pageStyle = 0;
    };

    QMap<QString, EventData> events;
    QStringList orderedEvents;

    QString currentEvent;
    int topLine = 0;
    QString username;
    bool isUserHomePage = false;
    QGraphicsScene * scene;
    QGraphicsItem * selectedItem = nullptr;
    QString selectedName;
    QPointF lastMousePosition;
};

#endif // PAGE_H
