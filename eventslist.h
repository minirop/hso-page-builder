#ifndef EVENTSLIST_H
#define EVENTSLIST_H

#include <QAbstractListModel>

class EventsList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EventsList(QObject *parent = nullptr);

    void clear();
    void reset();
    void addEvent(QString name);
    void setEventActive(QString name, bool active);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void activate(QString, bool);

private:
    struct EventData
    {
        QString name;
        bool active = false;
    };
    QVector<EventData> events;
};

#endif // EVENTSLIST_H
