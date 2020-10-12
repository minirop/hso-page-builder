#include "modsmanager.h"
#include "appsettings.h"
#include <QDialogButtonBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QLabel>

ModsManager::ModsManager(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Mods Manager");

    auto enabledMods = AppSettings::GetEnabledMods();

    QDir dir(AppSettings::GetModsPath());

    auto listWidget = new QListWidget;
    listWidget->setFocusPolicy(Qt::NoFocus);

    for (auto name : dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name))
    {
        dir.cd(name);

        if (dir.exists("config.ini"))
        {
            auto item = new QListWidgetItem(name);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

            if (enabledMods.contains(name))
            {
                item->setCheckState(Qt::Checked);
            }
            else
            {
                item->setCheckState(Qt::Unchecked);
            }
            listWidget->addItem(item);
        }

        dir.cdUp();
    }

    connect(listWidget, &QListWidget::itemDoubleClicked, [&](QListWidgetItem * item) {
        if (item->checkState() == Qt::Unchecked)
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
    });

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Double-click on mods you want to activate (on simple click on the checkbox), to get access to their assets."));
    layout->addWidget(listWidget);
    layout->addWidget(buttons);
    setLayout(layout);

    connect(buttons, &QDialogButtonBox::accepted, [listWidget, this]() {
        QStringList mods;

        for (int i = 0; i < listWidget->count(); i++)
        {
            auto item = listWidget->item(i);
            if (item->checkState() == Qt::Checked)
            {
                mods += item->text();
            }
        }

        AppSettings::SetEnabledMods(mods);

        this->accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
