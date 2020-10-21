#ifndef EVENTSLISTFILTERMODEL_H
#define EVENTSLISTFILTERMODEL_H

#include <QSortFilterProxyModel>

class EventsListFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventsListFilterModel(bool active, QObject *parent = nullptr);
    void setSourceModel(QAbstractItemModel * sourceModel) override;

    void moveEventUp(QString name);
    void moveEventDown(QString name);
    void moveEventToTop(QString name);
    void moveEventToBottom(QString name);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:
    bool activeFilter;
    QStringList eventsOrder;
};

#endif // EVENTSLISTFILTERMODEL_H
