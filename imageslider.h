#ifndef IMAGESLIDER_H
#define IMAGESLIDER_H

#include <QWidget>

class ImageSlider : public QWidget
{
    Q_OBJECT
public:
    explicit ImageSlider(QWidget *parent = nullptr);

    void setImage(QString image);
    void setEmptyColor(QColor color);
    void refresh();

public slots:
    void prev();
    void next();

signals:
    void backgroundChanged(QString name);

protected:
    void paintEvent(QPaintEvent * event);

private:
    QPixmap getPixmap(QString name);

    QPixmap bgPixmap;
    QStringList backgrounds;
    QColor emptyColor = Qt::black;
    int current = 0;
};

#endif // IMAGESLIDER_H
