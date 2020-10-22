#include "eventslist.h"
#include "globals.h"

EventsList::EventsList(QObject *parent)
    : QAbstractListModel(parent)
{
}

void EventsList::clear()
{
    beginResetModel();

    events.clear();

    endResetModel();
}

void EventsList::reset()
{
    beginResetModel();

    for (auto & ev : events)
    {
        ev.active = false;
    }
    events.first().active = true;

    endResetModel();
}

void EventsList::addEvent(QString name)
{
    auto size = events.size();
    beginInsertRows(QModelIndex(), size, size);

    events.append({ name });

    endInsertRows();
}

void EventsList::setEventActive(QString name, bool active)
{
    for (int row = 0; auto & ev : events)
    {
        if (ev.name.compare(name, Qt::CaseInsensitive) == 0)
        {
            ev.active = active;
            auto idx = index(row);
            emit dataChanged(idx, idx, { EVENT_ACTIVE_ROLE });
            break;
        }

        row++;
    }
}

int EventsList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return events.size();
}

QVariant EventsList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        return events[index.row()].name;
    case EVENT_ACTIVE_ROLE:
        return events[index.row()].active;
    }

    return QVariant();
}
