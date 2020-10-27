#ifndef PAGESETTINGS_H
#define PAGESETTINGS_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class PageSettings;
}

class QListWidgetItem;
class Gif;
class Text;
class PageElement;
class EventsList;
class EventsListFilterModel;
class PageSettings : public QWidget
{
    Q_OBJECT

public:
    explicit PageSettings(QWidget *parent = nullptr);
    ~PageSettings();

    void select(int idSel);
    void clearSelection();

    void setBackground(QString image);
    void setBackgroundColorButton(QColor color);
    void setLineCounts(int count);
    void setPageCursor(int pageCursor);
    void setMusic(QString music);
    void setOnLoadScript(QString script);
    void setPageStyle(int style);

signals:
    void selectionChanged(int newSelection, int oldSelection);
    void selectedNameChanged(QString name);
    void createElement(QString type, QJsonArray definition, QStringList eventData);
    void updateZOrder();
    void pageTitleChanged(QString title);
    void pageOwnerChanged(QString owner);
    void pageDescriptionChanged(QString description);
    void backgroundColorChanged(QColor color);
    void backgroundChanged(QString image);
    void lineCountChanged(int count);
    void duplicateElement(QString name, PageElement * pageElement);
    void musicChanged(QString music);
    void cursorChanged(int cursor);
    void pageStyleChanged(int style);
    void homePageChanged(bool b);
    void onLoadScriptChanged(QString script);
    void webpageEventActivated(QString name);
    void webpageEventDeactivated(QString name);
    void webpageEventSelected(QString name);
    void elementsEventActivated(QString name);
    void elementsEventDeactivated(QString name);
    void elementsEventSelected(QString name);

private slots:
    void itemChanged(QListWidgetItem * item, QListWidgetItem * previous);

private:
    void updateProperties(QListWidgetItem * item);
    void updateProperties(PageElement * elem);
    void updateGifProperties(Gif * gif);
    void updateTextProperties(Text * text);

    void setFontColorButton(QColor color);
    void setFadeColorButton(QColor color);
    void setBackgroundColor(QWidget * widget, QColor color);

    void refresh();
    void refreshGifsList();
    void refreshMusicList();
    void refreshUsers();
    void refreshEvents();
    void reset();

    friend class MainWindow;
    Ui::PageSettings *ui;
    EventsList * webpageEventsList = nullptr;
    EventsListFilterModel * webpageActiveEvents = nullptr;
    EventsListFilterModel * webpageInactiveEvents = nullptr;
    EventsList * elementsEventsList = nullptr;
    EventsListFilterModel * elementsInactiveEvents = nullptr;

    QColor bgColor;
};

#endif // PAGESETTINGS_H
