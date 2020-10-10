#ifndef FONTDATABASE_H
#define FONTDATABASE_H

#include <QString>
#include <QMap>

class FontDatabase
{
public:
    FontDatabase();

    void load(QString directory);

    struct FontData {
        int spacing = 0;
        int lineheight = 0;
        QMap<char, int> widths;

        int getWidth(char c, int defaultWidth);
    };

    static FontData & GetFont(QString name);
    static QList<QString> GetFonts();

private:
    static inline FontDatabase * instance = nullptr;
    QMap<QString, FontData> fonts;
};

#endif // FONTDATABASE_H
