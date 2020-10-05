#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private:
    void clearEverything();
    void parseJSON(QByteArray data);
    QGraphicsItem * addElement(QString name, QStringList arguments);
    void updateZOrder();

    QHash<int, QGraphicsItem*> pageElements;
    Ui::MainWindow * ui;
    Page * webpage;
    PageSettings * settings;
    FontDatabase fontDatabase;
    QWidget * area = nullptr;
};

#endif // MAINWINDOW_H
