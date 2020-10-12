#include "pageelement.h"

PageElement::PageElement()
{
}

void PageElement::setCaseTag(QString tag)
{
    caseTag = tag;
}

void PageElement::setBrokenLaw(int law)
{
    if (law == 0) law = -1;
    brokenLaw = law;
}
