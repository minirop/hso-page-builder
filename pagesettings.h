#ifndef PAGESETTINGS_H
#define PAGESETTINGS_H

#include <QWidget>

namespace Ui {
class PageSettings;
}

class QListWidgetItem;
class Gif;
class Text;
class PageSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PageSettings(QWidget *parent = nullptr);
    ~PageSettings();

    void select(int idSel);
    void clearSelection();

signals:
    void selectionChanged(int newSelection, int oldSelection);
    void selectedNameChanged(QString name);

private slots:
    void itemChanged(QListWidgetItem * item, QListWidgetItem * previous);

private:
    void updateProperties(QListWidgetItem * item);
    void updateGifProperties(Gif * gif);
    void updateTextProperties(Text * text);

    void setFontColorButton(QColor color);

    friend class MainWindow;
    Ui::PageSettings *ui;
};

#endif // PAGESETTINGS_H
