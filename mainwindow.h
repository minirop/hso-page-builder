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

private slots:
    void openPage();
    void savePage();
    void createElement(QString type, QJsonArray definition, QStringList eventData);
    void updateZOrder();

private:
    void clearEverything();
    void parseJSON(QByteArray data);
    QGraphicsItem * addElement(QString type, QStringList arguments);
    QJsonArray gifToJson(Gif * gif);
    QJsonArray textToJson(Text * text);
    QJsonArray emptyArray();

    QHash<int, QGraphicsItem*> pageElements;
    Ui::MainWindow * ui;
    Page * webpage;
    PageSettings * settings;
    FontDatabase fontDatabase;
    QWidget * area = nullptr;
};

#endif // MAINWINDOW_H
