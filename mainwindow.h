#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include "page.h"
#include "pagesettings.h"
#include "fontdatabase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent * event);

private slots:
    void newPage();
    void openPage();
    void savePage();
    void savePageAs();
    void openModsWindow();
    void refresh();
    QGraphicsItem * createElement(QString type, QJsonArray definition, QStringList eventData);
    void updateZOrder();
    void duplicateElement(QString name, PageElement * pageElement);

private:
    void clearEverything();
    void parseJSON(QByteArray data);
    QGraphicsItem * addElement(QString type, QStringList arguments, PageElement * pageElement = nullptr);
    QJsonArray gifToJson(Gif * gif, QString eventName);
    QJsonArray textToJson(Text * text, QString eventName);
    QJsonArray emptyArray();
    QJsonArray pageElementToJSON(PageElement * pageElement, QString eventName);
    QStringList pageElementToStringList(PageElement * pageElement, QString eventName);
    void updateSettingsFromPage(Page * webpage);
    void updateCurrentPageElement(PageElement * pageElement);

    QString getRealEventName(QString name);

    QHash<int, QGraphicsItem*> pageElements;
    Ui::MainWindow * ui;
    Page * webpage;
    PageSettings * settings;
    FontDatabase fontDatabase;
    QWidget * area = nullptr;
    QString openedFilename;
};

#endif // MAINWINDOW_H
