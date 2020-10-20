#include "eventslistfiltermodel.h"
#include "globals.h"

EventsListFilterModel::EventsListFilterModel(bool active, QObject *parent)
    : QSortFilterProxyModel(parent), activeFilter(active)
{
}

bool EventsListFilterModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    Q_UNUSED(source_parent)

    return sourceModel()->data(sourceModel()->index(source_row, 0), EVENT_ACTIVE_ROLE).toBool() == activeFilter;
}
