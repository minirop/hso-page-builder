#include "eventslistfiltermodel.h"
#include "globals.h"

EventsListFilterModel::EventsListFilterModel(bool active, QObject *parent)
    : QSortFilterProxyModel(parent), activeFilter(active)
{
}

void EventsListFilterModel::setSourceModel(QAbstractItemModel * sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    if (activeFilter)
    {
        connect(sourceModel, &QAbstractItemModel::dataChanged, [this](const QModelIndex & index, const QModelIndex &, const QVector<int> & roles) {
            if (roles.size() == 1 && roles[0] == EVENT_ACTIVE_ROLE)
            {
                auto name = index.data().toString();
                auto active = index.data(EVENT_ACTIVE_ROLE).toBool();
                if (active)
                {
                    if (!eventsOrder.contains(name))
                    {
                        eventsOrder.append(name);
                    }
                }
                else
                {
                    eventsOrder.removeAll(name);
                }
            }
        });
        connect(sourceModel, &QAbstractItemModel::modelReset, [&]() {
            eventsOrder.clear();
        });
    }
}

void EventsListFilterModel::moveEventUp(QString name)
{
    Q_UNUSED(name)
}

void EventsListFilterModel::moveEventDown(QString name)
{
    Q_UNUSED(name)
}

void EventsListFilterModel::moveEventToTop(QString name)
{
    Q_UNUSED(name)
}

void EventsListFilterModel::moveEventToBottom(QString name)
{
    Q_UNUSED(name)
}

bool EventsListFilterModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    Q_UNUSED(source_parent)

    return sourceModel()->data(sourceModel()->index(source_row, 0), EVENT_ACTIVE_ROLE).toBool() == activeFilter;
}
