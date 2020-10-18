#include "appsettings.h"
#include <QDir>

#define ROOT_PATH "paths/root"
#define MODS_PATH "config/mods"

AppSettings::AppSettings()
    : settings("settings.ini", QSettings::IniFormat)
{
    instance = this;
}

void AppSettings::SetRootPath(QString path)
{
    instance->settings.setValue(ROOT_PATH, path);
    instance->settings.sync();
}

QString AppSettings::GetRootPath()
{
    return instance->settings.value(ROOT_PATH).toString();
}

void AppSettings::SetEnabledMods(QStringList mods)
{
    instance->settings.setValue(MODS_PATH, mods);
    instance->settings.sync();
}

QStringList AppSettings::GetEnabledMods()
{
    return instance->settings.value(MODS_PATH).toStringList();
}

QString AppSettings::GetModsPath()
{
    return QDir::homePath() + "/Tendershoot/HypnOS/mods";
}

QString AppSettings::GetModPath(QString name)
{
    return GetModsPath() + "/" + name;
}

QStringList AppSettings::GetSearchPaths()
{
    QStringList paths;

    auto mods = AppSettings::GetEnabledMods();
    for (auto name : mods)
    {
        paths += AppSettings::GetModPath(name);
    }
    paths += GetRootPath();

    return paths;
}

QString AppSettings::GetFilePath(QString filename)
{
    if (filename[0] != '/') filename = "/" + filename;

    auto paths = GetSearchPaths();
    for (auto path : paths)
    {
        if (QFile(path + filename).exists())
        {
            return path + filename;
        }
    }

    return QString();
}

void AppSettings::SetPageDirty(bool dirty)
{
    instance->isDirty = dirty;
}

bool AppSettings::IsPageDirty()
{
    return instance->isDirty;
}
