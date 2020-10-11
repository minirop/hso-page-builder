#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_pagesettings.h"
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

const QString HYPNO_PATH = "/home/minirop/.local/share/Steam/steamapps/common/Hypnospace Outlaw/data/";

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

    auto widget = new QWidget;
    setCentralWidget(widget);
    auto layout = new QHBoxLayout(widget);
    layout->addWidget(area);
    layout->addWidget(settings);

    connect(ui->action_Open_Page, &QAction::triggered, this, &MainWindow::openPage);

    fontDatabase.load(HYPNO_PATH + "images/fonts/");
}

MainWindow::~MainWindow()
{
    delete ui;
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
        item->setData(ITEM_ID, ptr);
        pageElements[id] = element;
    }
}

void MainWindow::clearEverything()
{
    area->findChild<Page*>()->deleteLater();
    webpage = new Page(area);
    connect(webpage, &Page::selected, [&](int id) {
        settings->select(id);
    });
    connect(webpage, &Page::clearSelection, [&]() {
        settings->clearSelection();
    });
    connect(settings, &PageSettings::selectedNameChanged, webpage, &Page::setSelectedName);
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

        if (type == "Webpage")
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

    if (type == "Webpage")
    {
        auto background = arguments[5];
        webpage->setBackground(HYPNO_PATH + "images/bgs/" + background);
        webpage->setLineCount(arguments[3].toInt());

        setWindowTitle(arguments[1]);
    }
    else if (type == "Text")
    {
        auto x = (PAGE_WIDTH / 2) + (arguments[1].toInt() / PAGE_WIDTH);
        auto y = arguments[2].toInt();
        auto width = arguments[3].toInt() * PAGE_WIDTH / 100;
        auto caseTag = arguments[4];
        auto string = arguments[5];
        auto color = arguments[6].toInt();
        auto font = arguments[7];
        auto style = arguments[8];
        auto align = arguments[9].toInt();
        auto script = arguments[10];
        auto animation = arguments[12].toInt();
        auto animationSpeed = arguments[13].toInt();
        auto fadeColor = arguments[14].toInt();
        auto fadeSpeed = arguments[15].toInt();

        auto text = new Text;
        text->setAlign(align);
        text->setPos(x, y);
        text->setWidth(width);
        text->setFontSize(QString(style[0]).toInt());
        text->setFontBold(style[1] == 'b');
        text->setFont(font.toLower());
        text->setAnimation(animation);
        text->setAnimationSpeed(animationSpeed);

        if (color >= 0)
        {
            int r = (color >> 0) & 0xFF;
            int g = (color >> 8) & 0xFF;
            int b = (color >> 16) & 0xFF;
            text->setFontColor(QColor(r, g, b));
        }

        if (fadeColor >= 0)
        {
            int r = (fadeColor >> 0) & 0xFF;
            int g = (fadeColor >> 8) & 0xFF;
            int b = (fadeColor >> 16) & 0xFF;
            text->setFade(QColor(r, g, b), fadeSpeed);
        }

        text->setString(string);

        returnedElement = text;
    }
    else if (type == "Gif")
    {
        auto gif = new Gif;

        auto image = arguments[5];
        if (QFileInfo fi(HYPNO_PATH + "images/gifs/" + image); fi.isDir())
        {
            QDir dir(fi.absoluteFilePath());
            for (auto entry : dir.entryInfoList(QDir::Files, QDir::Name))
            {
                if (entry.suffix() == "speed")
                {
                    auto spd = entry.baseName().toInt();
                    gif->setSpeed(1000 / spd);
                }
                else
                {
                    gif->addFrame(entry.absoluteFilePath());
                }
            }
        }
        else if (QFile(HYPNO_PATH + "images/static/" + image + ".png").exists())
        {
            gif->addFrame(HYPNO_PATH + "images/static/" + image + ".png");
        }
        else if (QFile(HYPNO_PATH + "images/shapes/" + image + ".png").exists())
        {
            gif->addFrame(HYPNO_PATH + "images/shapes/" + image + ".png");
        }
        else
        {
            delete gif;
            gif = nullptr;
        }

        if (gif)
        {
            auto x = arguments[1].toInt();
            auto y = arguments[2].toInt();
            gif->setPos(x, y);

            auto color = arguments[3].split(',');

            if (color.size() == 3)
            {
                auto h = color[0].toInt() / 100.0f;
                auto s = color[1].toInt() / 100.0f;
                auto l = color[2].toInt() / 100.0f;
                gif->setHSL(h, s, l);
            }

            returnedElement = gif;
        }
    }

    if (returnedElement)
    {
        webpage->addElement(returnedElement);
    }

    return returnedElement;
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
