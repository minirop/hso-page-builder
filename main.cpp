#include "mainwindow.h"
#include "appsettings.h"
#include <QStyleFactory>
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Windows"));

    AppSettings settings;

    auto root = AppSettings::GetRootPath();
    QDir dir(root);
    if (root.isEmpty() || !dir.exists())
    {
#if defined(Q_OS_LINUX)
        auto defaultPath = QDir::homePath() + "/.local/share/Steam/steamapps/common/Hypnospace Outlaw/data";
#elif defined(Q_OS_WIN32)
        auto defaultPath = "";
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

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
