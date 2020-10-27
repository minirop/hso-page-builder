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
    for (int row = 0; auto & ev : events)
    {
        if (ev.active)
        {
            ev.active = false;
            auto idx = index(row);
            emit dataChanged(idx, idx, { EVENT_ACTIVE_ROLE });
        }
        row++;
    }
    events.first().active = true;
    auto idx = index(0);
    emit dataChanged(idx, idx, { EVENT_ACTIVE_ROLE });
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
        if (ev.name == name)
        {
            if (ev.active != active)
            {
                ev.active = active;
                auto idx = index(row);
                emit dataChanged(idx, idx, { EVENT_ACTIVE_ROLE });
            }
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
