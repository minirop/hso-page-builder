#ifndef PAGEELEMENT_H
#define PAGEELEMENT_H

#include <QObject>

class PageElement : public QObject
{
    Q_OBJECT

public:
    enum class ElementType
    {
        Gif,
        Text
    };

    PageElement();
    virtual ~PageElement() = default;

    virtual ElementType elementType() const = 0;

    void setCaseTag(QString tag);
    void setBrokenLaw(int law);

protected:
    QString caseTag;
    int brokenLaw = -1;
};

#endif // PAGEELEMENT_H
