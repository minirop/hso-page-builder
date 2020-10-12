#ifndef FONTDATABASE_H
#define FONTDATABASE_H

#include <QString>
#include <QMap>
#include <QPixmap>

class FontDatabase
{
public:
    FontDatabase();

    void load(QString directory);
    void clear();

    struct FontData {
        int spacing = 0;
        int lineheight = 0;
        QMap<char, int> widths;
        QString path;

        int getWidth(char c, int defaultWidth);
    };

    static FontData & GetFont(QString name);
    static QList<QString> GetFonts();
    static QPixmap GetFontAtlas(QString name);

private:
    static inline FontDatabase * instance = nullptr;
    QMap<QString, FontData> fonts;
};

#endif // FONTDATABASE_H
