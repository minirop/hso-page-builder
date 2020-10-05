#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_pagesettings.h"
#include "gif.h"
#include "text.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QHBoxLayout>
#include <QScrollArea>
#include <algorithm>

constexpr int PAGE_WIDTH = 300;

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
        pageElements[newSel]->setSelected(true);
        settings->select(newSel);
    });

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

void MainWindow::clearEverything()
{
    webpage = new Page(area);
    connect(webpage, &Page::selected, [&](int id) {
        settings->select(id);
    });
    connect(webpage, &Page::clearSelection, [&]() {
        settings->clearSelection();
    });
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
        auto element = addElement(type, eventData);

        auto id = definition[1].toInt();
        auto name = definition[2].toString();
        if (type == "Webpage") name = QString("[%1]").arg(type);
        auto item = new QListWidgetItem(name);
        if (type == "Webpage") item->setFlags(Qt::NoItemFlags);
        item->setData(Qt::UserRole, id);
        settings->ui->elementsList->addItem(item);

        if (element && type != "Webpage")
        {
            element->setData(Qt::UserRole, id);
            QVariant ptr;
            ptr.setValue(dynamic_cast<PageElement*>(element));
            item->setData(Qt::UserRole + 1, ptr);
            pageElements[id] = element;
        }
    }

    updateZOrder();
}

QGraphicsItem * MainWindow::addElement(QString name, QStringList arguments)
{
    QGraphicsItem * returnedElement = nullptr;

    if (name == "Webpage")
    {
        auto background = arguments[5];
        webpage->setBackground(HYPNO_PATH + "images/bgs/" + background);
        webpage->setLineCount(arguments[3].toInt());

        setWindowTitle(arguments[1]);
    }
    else if (name == "Text")
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
        text->setString(string);
        text->setAlign(align);
        text->setPos(x, y);
        text->setWidth(width);
        text->setFont(QString("%1images/fonts/%2%3.png").arg(HYPNO_PATH, font.toLower(), style));
        text->setAnimation(animation, animationSpeed);

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

        returnedElement = text;
    }
    else if (name == "Gif")
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
                int h = color[0].toInt();
                int s = color[1].toInt();
                int l = color[2].toInt();
                gif->setColor(QColor::fromHsl(h, s, l));
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
    for (int i = 1; i < settings->ui->elementsList->count(); i++)
    {
        auto item = settings->ui->elementsList->item(i);
        auto id = item->data(Qt::UserRole).toInt();

        pageElements[id]->setZValue(i * -1);
    }
}
