#include "pageelement.h"
#include "appsettings.h"

PageElement::PageElement()
{
}

void PageElement::setCaseTag(QString tag)
{
    if (tag == "0" || tag == "-1") tag.clear();
    caseTag = tag;
    AppSettings::SetPageDirty();
}

void PageElement::setBrokenLaw(int law)
{
    if (law == 0) law = -1;
    brokenLaw = law;
    AppSettings::SetPageDirty();
}

void PageElement::setScript(QString scpt)
{
    if (scpt == "0" || scpt == "-1") scpt.clear();
    script = scpt;
    AppSettings::SetPageDirty();
}
