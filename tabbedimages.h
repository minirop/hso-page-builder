#ifndef TABBEDIMAGES_H
#define TABBEDIMAGES_H

#include <QWidget>

namespace Ui {
class TabbedImages;
}

class QAbstractButton;
class QListWidget;
class QListWidgetItem;
class TabbedImages : public QWidget
{
    Q_OBJECT

public:
    enum class Type {
        Gif,
        Static,
        Shape,
        Wordart
    };

    explicit TabbedImages(QWidget *parent = nullptr);
    ~TabbedImages();

    void addImage(Type type, QString name);
    void clear();

    void select(QString name);

    Type type() const;

signals:
    void currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);

private:
    friend class PageSettings;
    Ui::TabbedImages *ui;
    std::array<QListWidget*, 4> lists;
    std::array<QAbstractButton*, 4> buttons;
};

#endif // TABBEDIMAGES_H
