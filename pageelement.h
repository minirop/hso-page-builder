#ifndef PAGEELEMENT_H
#define PAGEELEMENT_H

#include <QObject>

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

    virtual ElementType elementType() const = 0;
    virtual void refresh() = 0;

    void setCaseTag(QString tag);
    void setBrokenLaw(int law);
    void setScript(QString scpt);

protected:
    QString caseTag;
    int brokenLaw = -1;
    QString script;
};

#endif // PAGEELEMENT_H
