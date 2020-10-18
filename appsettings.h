#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QSettings>

class AppSettings
{
public:
    AppSettings();

    static void SetRootPath(QString path);
    static QString GetRootPath();

    static void SetEnabledMods(QStringList mods);
    static QStringList GetEnabledMods();

    static QString GetModsPath();
    static QString GetModPath(QString name);

    static QStringList GetSearchPaths();

    static void SetPageDirty(bool dirty = true);
    static bool IsPageDirty();

private:
    static inline AppSettings * instance = nullptr;
    QSettings settings;
    bool isDirty = false;
};

#endif // APPSETTINGS_H
