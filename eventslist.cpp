#include "eventslist.h"

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

void EventsList::addEvent(QString name, Qt::CheckState state)
{
    auto size = events.size();
    beginInsertRows(QModelIndex(), size, size);

    events.append(EventData { state, name, size });

    endInsertRows();
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

    const EventData & eventData = events[index.row()];
    switch (role)
    {
    case Qt::DisplayRole:
        return eventData.name;
    case Qt::CheckStateRole:
        return eventData.state;
    }

    return QVariant();
}

Qt::ItemFlags EventsList::flags(const QModelIndex & index) const
{
    auto checked = index.data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Checked;
    if (!checked)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    else if (index.row() == 0)
    {
        if (index.siblingAtRow(1).data(Qt::CheckStateRole).value<Qt::CheckState>() == Qt::Unchecked)
            return Qt::ItemIsUserCheckable;
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

bool EventsList::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    switch (role)
    {
    case Qt::CheckStateRole:
    {
        auto state = value.value<Qt::CheckState>();
        auto & ev = events[index.row()];
        if (ev.state != state)
        {
            ev.state = state;
            emit dataChanged(index, index, { Qt::CheckStateRole });

            moveItemIfNecessary(index.row(), state);
            return true;
        }
        break;
    }
    }

    return false;
}

void EventsList::moveItemIfNecessary(int row, Qt::CheckState state)
{
    int destIndex = 0;
    int destRow = 0;

    if (state == Qt::Checked)
    {
        destRow = std::distance(events.begin(), std::find_if(events.begin(), events.end(), [](const EventData & data) {
            return data.state == Qt::Unchecked;
        }));

        // if the destination is below the current item's row, don't move.
        //
        if (destRow >= row)
        {
            return;
        }

        destIndex = destRow;
    }
    else
    {
        for (int i = 0; i < events.size(); i++)
        {
            if (events[i].state == Qt::Unchecked && events[i].id > events[row].id)
            {
                destRow = i - 1;
                break;
            }
        }

        if (destRow <= row)
        {
            return;
        }

        destIndex = destRow + 1;
    }

    beginMoveRows(QModelIndex(), row, row, QModelIndex(), destIndex);

    auto eventData = events.takeAt(row);
    events.insert(destRow, eventData);

    endMoveRows();
}
