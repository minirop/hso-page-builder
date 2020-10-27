#ifndef PAGEELEMENT_H
#define PAGEELEMENT_H

#include <QObject>
#include <QMap>

class PageElement : public QObject
{
    Q_OBJECT

public:
    enum class ElementType {
        Gif,
        Text
    };

    PageElement();
    virtual ~PageElement() = default;

    virtual void setEvent(QString name);
    virtual void clearEvent(QString name);
    virtual ElementType elementType() const = 0;
    virtual void refresh() = 0;

    QStringList activeEvents() const;
    void moveActiveEvent(int from, int to);

    void setCaseTag(QString tag);
    void setBrokenLaw(int law);
    void setScript(QString scpt);

    QString caseTag() const;
    int brokenLaw() const;
    QString script() const;

protected:
    QString currentEvent;

private:
    struct PageEventData {
        QString caseTag;
        int brokenLaw = -1;
        QString script;
    };

    QMap<QString, PageEventData> pageEvents;
    QStringList orderedEvents;
};

#endif // PAGEELEMENT_H
