#ifndef EVENTSLIST_H
#define EVENTSLIST_H

#include <QAbstractListModel>

class EventsList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EventsList(QObject *parent = nullptr);

    void clear();
    void addEvent(QString name, Qt::CheckState state = Qt::Unchecked);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    bool setData(const QModelIndex & index, const QVariant & value, int role) override;

signals:
    void activate(QString, bool);

private:
    void moveItemIfNecessary(int row, Qt::CheckState state);

    struct EventData {
        Qt::CheckState state;
        QString name;
        int id = 0;
    };
    QVector<EventData> events;
};

#endif // EVENTSLIST_H
