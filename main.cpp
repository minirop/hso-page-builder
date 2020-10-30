#include "mainwindow.h"
#include "appsettings.h"
#include <QStyleFactory>
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

int main(int argc, char *argv[])
{
#ifndef Q_OS_WINDOWS
    auto style = QStyleFactory::create("Windows");
    if (style)
        QApplication::setStyle(style);
#endif

    QApplication a(argc, argv);

    AppSettings settings;

    auto root = AppSettings::GetRootPath();
    QDir dir(root);
    if (root.isEmpty() || !dir.exists())
    {
#if defined(Q_OS_LINUX)
        auto defaultPath = QDir::homePath() + "/.steam/steam/steamapps/common/Hypnospace Outlaw/data";
#elif defined(Q_OS_WIN32)
        auto defaultPath = "C:/Program Files (x86)/Steam/steamapps/common/Hypnospace Outlaw/data";
#endif
        dir.setCurrent(defaultPath);
        if (!dir.exists())
        {
            QMessageBox::warning(nullptr, "Hypnospace Outlaw not found!", "Hypnospace Outlaw has not been found, please select the directory where the game is installed.");
            auto gamePath = QFileDialog::getExistingDirectory(nullptr, "Hypnospace Outlaw directory", QDir::homePath());
            if (gamePath.isEmpty())
            {
                QMessageBox::information(nullptr, "Bye", "The application will now quit.");
                return 0;
            }

            dir.setCurrent(gamePath);

            if (dir.exists("data"))
            {
                dir.cd("data");
            }

        }

        AppSettings::SetRootPath(dir.absolutePath());
    }

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
