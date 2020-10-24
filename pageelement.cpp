#include "pageelement.h"
#include "appsettings.h"

PageElement::PageElement()
{
}

void PageElement::setEvent(QString name)
{
    if (!pageEvents.contains(name))
    {
        PageEventData data;
        if (pageEvents.contains(currentEvent))
        {
            data = pageEvents[currentEvent];
        }
        pageEvents[name] = data;
    }

    currentEvent = name;
}

void PageElement::clearEvent(QString name)
{
    pageEvents.remove(name);
}

void PageElement::setCaseTag(QString tag)
{
    if (tag == "0" || tag == "-1") tag.clear();
    pageEvents[currentEvent].caseTag = tag;
    AppSettings::SetPageDirty();
}

void PageElement::setBrokenLaw(int law)
{
    if (law == 0) law = -1;
    pageEvents[currentEvent].brokenLaw = law;
    AppSettings::SetPageDirty();
}

void PageElement::setScript(QString scpt)
{
    if (scpt == "0" || scpt == "-1") scpt.clear();
    pageEvents[currentEvent].script = scpt;
    AppSettings::SetPageDirty();
}

QString PageElement::caseTag() const
{
    return pageEvents[currentEvent].caseTag;
}

int PageElement::brokenLaw() const
{
    return pageEvents[currentEvent].brokenLaw;
}

QString PageElement::script() const
{
    return pageEvents[currentEvent].script;
}
