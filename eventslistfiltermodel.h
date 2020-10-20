#ifndef EVENTSLISTFILTERMODEL_H
#define EVENTSLISTFILTERMODEL_H

#include <QSortFilterProxyModel>

class EventsListFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventsListFilterModel(bool active, QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:
    bool activeFilter;
};

#endif // EVENTSLISTFILTERMODEL_H
