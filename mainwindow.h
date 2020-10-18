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
    void openModsWindow();
    void refresh();
    void createElement(QString type, QJsonArray definition, QStringList eventData);
    void updateZOrder();
    void duplicateElement(QString name, PageElement * pageElement);

private:
    void clearEverything();
    void parseJSON(QByteArray data);
    QGraphicsItem * addElement(QString type, QStringList arguments);
    QJsonArray gifToJson(Gif * gif);
    QJsonArray textToJson(Text * text);
    QJsonArray emptyArray();
    QStringList pageElementToStringList(PageElement * pageElement);

    QHash<int, QGraphicsItem*> pageElements;
    Ui::MainWindow * ui;
    Page * webpage;
    PageSettings * settings;
    FontDatabase fontDatabase;
    QWidget * area = nullptr;
    QString openedFilename;
};

#endif // MAINWINDOW_H
