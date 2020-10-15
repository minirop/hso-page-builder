#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_pagesettings.h"
#include "modsmanager.h"
#include "appsettings.h"
#include "globals.h"
#include "gif.h"
#include "text.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHBoxLayout>
#include <QScrollArea>
#include <algorithm>

QList<int> stringsToInts(QStringList strings)
{
    QList<int> ints;
    std::transform(strings.begin(), strings.end(), std::back_inserter(ints), [](auto elem) {
        return elem.toInt();
    });

    return ints;
}

int colorToInt(QColor color)
{
    return color.red() | (color.green() << 8) | (color.blue() << 16);
}

QColor intToColor(int color)
{
    int r = (color >> 0) & 0xFF;
    int g = (color >> 8) & 0xFF;
    int b = (color >> 16) & 0xFF;
    return QColor(r, g, b);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    area = new QWidget;
    area->setFixedWidth(PAGE_WIDTH * ZOOM);

    settings = new PageSettings;
    connect(settings, &PageSettings::selectionChanged, [&](int newSel, int oldSel) {
        if (oldSel != -1) pageElements[oldSel]->setSelected(false);
        if (newSel != -1)
        {
            pageElements[newSel]->setSelected(true);
            settings->select(newSel);
        }
    });
    connect(settings, &PageSettings::createElement, this, &MainWindow::createElement);
    connect(settings, &PageSettings::updateZOrder, this, &MainWindow::updateZOrder);
    connect(settings, &PageSettings::pageTitleChanged, [&](QString title) {
        setWindowTitle(title);
    });
    connect(settings, &PageSettings::duplicateElement, this, &MainWindow::duplicateElement);

    auto widget = new QWidget;
    setCentralWidget(widget);
    auto layout = new QHBoxLayout(widget);
    layout->addWidget(area);
    layout->addWidget(settings);

    connect(ui->action_New_page, &QAction::triggered, this, &MainWindow::newPage);
    connect(ui->action_Open_Page, &QAction::triggered, this, &MainWindow::openPage);
    connect(ui->action_Save_Page, &QAction::triggered, this, &MainWindow::savePage);
    connect(ui->action_Quit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->action_Mods, &QAction::triggered, this, &MainWindow::openModsWindow);
    connect(ui->action_Refresh, &QAction::triggered, this, &MainWindow::refresh);

    refresh();

    newPage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newPage()
{
    QJsonObject emptyPage;

    QJsonArray element;
    auto definition = emptyArray();
    definition[DefType] = TYPE_WEBPAGE;
    element.append(definition);

    auto metadata = emptyArray();
    metadata[WebEvent] = EVENT_DEFAULT;
    metadata[WebTitle] = "Empty page";
    metadata[WebUsername] = "";
    metadata[WebHeight] = "10";
    metadata[WebMusic] = "";
    metadata[WebBGImage] = "";
    metadata[WebMouseFX] = "0";
    metadata[WebBGColor] = "0";
    metadata[WebDescriptionAndTags] = "";
    metadata[WebPageStyle] = "";
    metadata[WebUserHOME] = "0";
    metadata[WebOnLoadScript] = "";
    element.append(metadata);

    QJsonArray dataArray;
    dataArray.push_back(element);
    emptyPage["data"] = dataArray;

    parseJSON(QJsonDocument(emptyPage).toJson(QJsonDocument::Compact));
}

void MainWindow::openPage()
{
    auto filename = QFileDialog::getOpenFileName(this, "Open page", QString(), "Hypnospace pages (*.hsp *.js)");
    if (!filename.isEmpty())
    {
        QFile f(filename);
        if (f.open(QFile::ReadOnly))
        {
            auto contents = f.readAll();
            f.close();

            parseJSON(contents);
        }
    }
}

void MainWindow::savePage()
{
    QJsonObject object;
    object["c2array"] = true;
    QJsonArray size;
    size.append(settings->ui->elementsList->count() + 1);
    size.append(21);
    size.append(21);
    object["size"] = size;

    QJsonArray data;

    for (auto i = -1; i < settings->ui->elementsList->count(); i++)
    {
        QJsonArray element;

        if (i == -1)
        {
            auto definition = emptyArray();
            definition[DefType] = TYPE_WEBPAGE;
            element.append(definition);

            auto metadata = emptyArray();
            metadata[WebEvent] = EVENT_DEFAULT;
            metadata[WebTitle] = webpage->title;
            metadata[WebUsername] = webpage->username;
            metadata[WebHeight] = QString::number(webpage->linesCount);
            metadata[WebMusic] = webpage->music;
            metadata[WebBGImage] = webpage->background;
            metadata[WebMouseFX] = "0";
            metadata[WebBGColor] = "0";
            metadata[WebDescriptionAndTags] = webpage->descriptionAndTags;
            metadata[WebPageStyle] = "";
            metadata[WebUserHOME] = "0";
            metadata[WebOnLoadScript] = "";
            element.append(metadata);
        }
        else
        {
            auto item = settings->ui->elementsList->item(i);
            auto id = item->data(ROLE_ID).toInt();
            auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

            QString typeName;
            switch (pageElement->elementType())
            {
            case PageElement::ElementType::Gif:
            {
                typeName = TYPE_GIF;
                element.append(gifToJson(dynamic_cast<Gif*>(pageElement)));
                break;
            }
            case PageElement::ElementType::Text:
            {
                typeName = TYPE_TEXT;
                element.append(textToJson(dynamic_cast<Text*>(pageElement)));
                break;
            }
            default:
                assert(false);
            }

            auto definition = emptyArray();
            definition[DefType] = typeName;
            definition[DefId] = id;
            definition[DefName] = item->text();
            element.prepend(definition);
        }

        while (element.size() < 21)
        {
            element.push_back(emptyArray());
        }

        data.append(element);
    }

    object["data"] = data;

    QFile f("debug.json");
    if (f.open(QFile::WriteOnly))
    {
        f.write(QJsonDocument(object).toJson(QJsonDocument::Compact));
        f.close();
    }
}

void MainWindow::openModsWindow()
{
    ModsManager manager;
    if (manager.exec() == QDialog::Accepted)
    {
        refresh();
    }
}

void MainWindow::refresh()
{
    auto paths = AppSettings::GetSearchPaths();

    fontDatabase.clear();
    for (auto path : paths)
    {
        fontDatabase.load(path + "/images/fonts");
    }

    settings->refresh();
}

void MainWindow::createElement(QString type, QJsonArray definition, QStringList eventData)
{
    auto element = addElement(type, eventData);

    auto id = definition[1].toInt();
    if (id == 0)
    {
        auto keys = pageElements.keys();
        id = *std::max_element(keys.begin(), keys.end()) + 10;
    }
    auto name = definition[2].toString();
    auto item = new QListWidgetItem(name);
    item->setData(ROLE_ID, id);
    settings->ui->elementsList->addItem(item);

    if (element)
    {
        element->setData(ROLE_ID, id);
        QVariant ptr;
        ptr.setValue(dynamic_cast<PageElement*>(element));
        item->setData(ROLE_ELEMENT, ptr);
        pageElements[id] = element;
    }
}

void MainWindow::clearEverything()
{
    auto oldPage = area->findChild<Page*>();
    if (oldPage)
    {
        oldPage->deleteLater();
    }

    webpage = new Page(area);
    connect(webpage, &Page::selected, [&](int id) {
        settings->select(id);
    });
    connect(webpage, &Page::clearSelection, [&]() {
        settings->clearSelection();
    });
    connect(settings, &PageSettings::selectedNameChanged, webpage, &Page::setSelectedName);
    connect(settings, &PageSettings::pageTitleChanged, webpage, &Page::setTitle);
    connect(settings, &PageSettings::pageOwnerChanged, webpage, &Page::setOwner);
    connect(settings, &PageSettings::pageDescriptionChanged, webpage, &Page::setDescription);
    connect(settings, &PageSettings::backgroundChanged, webpage, &Page::setBackground);
    connect(settings, &PageSettings::backgroundColorChanged, webpage, &Page::setBackgroundColor);
    connect(settings, &PageSettings::lineCountChanged, webpage, &Page::setLineCount);

    webpage->move(0, 0);
    webpage->show();

    settings->ui->elementsList->clear();
}

void MainWindow::parseJSON(QByteArray data)
{
    clearEverything();

    auto doc = QJsonDocument::fromJson(data);
    auto obj = doc.object();
    auto pageData = obj["data"].toArray();
    webpage->setLineCount(pageData.size());
    webpage->setSceneRect(area->rect());

    for (auto line : pageData)
    {
        auto eventList = line.toArray();
        auto definition = eventList[0].toArray();
        auto type = definition.first().toString();
        auto eventData = eventList[1].toVariant().toStringList();

        if (type == TYPE_WEBPAGE)
        {
            addElement(type, eventData);
        }
        else
        {
            createElement(type, definition, eventData);
        }
    }

    updateZOrder();
}

QGraphicsItem * MainWindow::addElement(QString type, QStringList arguments)
{
    QGraphicsItem * returnedElement = nullptr;

    if (type == TYPE_WEBPAGE)
    {
        webpage->setBackground(arguments[WebBGImage]);
        webpage->setLineCount(arguments[WebHeight].toInt());

        webpage->setBackgroundColor(intToColor(arguments[WebBGColor].toInt()));

        webpage->title = arguments[WebTitle];
        setWindowTitle(webpage->title);

        webpage->username = arguments[WebUsername];
        webpage->music = arguments[WebMusic];
        webpage->descriptionAndTags = arguments[WebDescriptionAndTags];

        settings->ui->pageTitleLineEdit->setText(webpage->title);
        settings->ui->pageOwnerLineEdit->setText(webpage->username);
        settings->ui->pageDescriptionAndTags->setPlainText(webpage->descriptionAndTags);
        settings->setBackgroundColorButton(webpage->backgroundColor);
        settings->setBackground(webpage->background);
        settings->setLineCounts(webpage->linesCount);
    }
    else if (type == TYPE_TEXT)
    {
        auto x = arguments[TextX].toInt();
        auto y = arguments[TextY].toInt();
        auto width = arguments[TextWidth].toInt();
        auto caseTag = arguments[TextCaseTag];
        auto string = arguments[TextString];
        auto color = arguments[TextColor].toInt();
        auto font = arguments[TextFont];
        auto style = arguments[TextStyle];
        auto align = arguments[TextAlign].toInt();
        auto script = arguments[TextLinkOrScript];
        auto law = arguments[TextLawBroken].toInt();
        auto animation = arguments[TextAnimation].toInt();
        auto animationSpeed = arguments[TextAnimSpeed].toInt();
        auto fadeColor = arguments[TextColorFadeTo].toInt();
        auto fadeSpeed = arguments[TextColorFadeSpeed].toInt();

        auto text = new Text;
        text->setAlign(align);
        text->setHSPosition(x, y);
        text->setWidth(width);
        text->setFontSize(QString(style[0]).toInt());
        text->setFontBold(style[1] == 'b');
        text->setFont(font.toLower());
        text->setAnimation(animation);
        text->setAnimationSpeed(animationSpeed);
        text->setCaseTag(caseTag);
        text->setBrokenLaw(law);

        if (color >= 0)
        {
            text->setFontColor(intToColor(color));
        }

        if (fadeColor >= 0)
        {
            text->setFade(intToColor(fadeColor), fadeSpeed);
        }

        text->setString(string);

        returnedElement = text;
    }
    else if (type == TYPE_GIF)
    {
        auto gif = new Gif;

        auto x = arguments[GifX].toInt();
        auto y = arguments[GifY].toInt();
        auto color = arguments[GifHSL].split(',');
        auto image = arguments[GifNameOf];
        auto scale = arguments[GifScale].toDouble();
        auto rotation = arguments[GifRotation].toInt();
        auto mirrored = arguments[GifMirror].toInt() != 0;
        auto flipped = arguments[GifFlip].toInt() != 0;

        gif->nameOf = image;
        gif->refresh();

        gif->setScale(scale);
        gif->setRotation(rotation);
        gif->mirror(mirrored);
        gif->flip(flipped);

        gif->setPos(x, y);

        if (color.size() == 3)
        {
            auto h = color[0].toInt();
            auto s = color[1].toInt();
            auto l = color[2].toInt();
            gif->setHSL(h, s, l);
        }

        returnedElement = gif;
    }

    if (returnedElement)
    {
        webpage->addElement(returnedElement);
    }

    return returnedElement;
}

QJsonArray MainWindow::gifToJson(Gif * gif)
{
    QJsonArray array = emptyArray();

    array[GifEvent] = "DEFAULT";
    array[GifX] = QString::number((int)gif->pos().x());
    array[GifY] = QString::number((int)gif->pos().y());
    array[GifHSL] = QString("%1,%2,%3").arg(gif->H).arg(gif->S).arg(gif->L);
    array[GifCaseTag] = gif->caseTag;
    array[GifNameOf] = gif->nameOf;
    array[GifScale] = "1";
    array[GifRotation] = "0";
    array[GifMirror] = "0";
    array[GifFlip] = "0";
    array[GifLinkOrScript] = "-1";
    array[GifLawBroken] = "";
    array[GifAnimFlipX] = "-1";
    array[GifAnimFlipY] = "-1";
    array[GifAnimFade] = "-1";
    array[GifAnimTurn] = "0";
    array[GifAnimTurnSpeed] = "0";
    array[GifFPS] = QString::number(gif->fps);
    array[GifOffset] = "0";
    array[GifSync] = "0";
    array[GifAnimMouseOver] = "0";

    return array;
}

QJsonArray MainWindow::textToJson(Text * text)
{
    QJsonArray array = emptyArray();

    array[TextEvent] = EVENT_DEFAULT;
    array[TextX] = QString::number((((int)text->x() * 100) / PAGE_WIDTH) - 50);
    array[TextY] = QString::number((int)text->y());
    array[TextWidth] = QString::number(text->width);
    array[TextCaseTag] = text->caseTag;
    array[TextString] = text->string;
    array[TextColor] = QString::number(colorToInt(text->fontColor));
    array[TextFont] = text->fontName;
    array[TextStyle] = QString("%1%2").arg(text->fontSize).arg(text->fontBold ? 'b' : 'n');
    array[TextAlign] = QString::number(text->align);
    array[TextLinkOrScript] = "-1";
    array[TextLawBroken] = QString::number(text->brokenLaw);
    array[TextAnimation] = QString::number(static_cast<int>(text->animation));
    array[TextAnimSpeed] = QString::number(text->animationSpeed);
    array[TextColorFadeTo] = QString::number(colorToInt(text->fadeColor));
    array[TextColorFadeSpeed] = QString::number(text->fadeSpeed);

    return array;
}

QJsonArray MainWindow::emptyArray()
{
    return QJsonArray {QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString(), QString()};
}

QStringList MainWindow::pageElementToStringList(PageElement * pageElement)
{
    QJsonArray array;

    switch (pageElement->elementType())
    {
    case PageElement::ElementType::Gif:
        array = gifToJson(dynamic_cast<Gif*>(pageElement));
        break;
    case PageElement::ElementType::Text:
        array = textToJson(dynamic_cast<Text*>(pageElement));
        break;
    default:
        assert(false);
    }

    QStringList list;
    for (auto value : array)
    {
        list += value.toString();
    }
    return list;
}

void MainWindow::updateZOrder()
{
    for (int i = 0; i < settings->ui->elementsList->count(); i++)
    {
        auto item = settings->ui->elementsList->item(i);
        auto id = item->data(ROLE_ID).toInt();

        pageElements[id]->setZValue(i * -1);
    }
}

void MainWindow::duplicateElement(QString name, PageElement * pageElement)
{
    QString type;

    switch (pageElement->elementType())
    {
    case PageElement::ElementType::Gif:
        type = TYPE_GIF;
        break;
    case PageElement::ElementType::Text:
        type = TYPE_TEXT;
        break;
    default:
        assert(false);
    }

    QJsonArray definition;
    definition.append(0);
    definition.append(0);
    definition.append(name);

    auto eventData = pageElementToStringList(pageElement);

    createElement(type, definition, eventData);

    updateZOrder();
}
