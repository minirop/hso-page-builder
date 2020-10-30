#include "tabbedimages.h"
#include "ui_tabbedimages.h"

TabbedImages::TabbedImages(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabbedImages)
{
    ui->setupUi(this);

    lists[0] = ui->gifsListWidget;
    lists[1] = ui->staticListWidget;
    lists[2] = ui->shapesListWidget;
    lists[3] = ui->wordartListWidget;
    buttons[0] = ui->gifsButton;
    buttons[1] = ui->staticButton;
    buttons[2] = ui->shapesButton;
    buttons[3] = ui->wordartButton;

    connect(ui->gifsListWidget, &QListWidget::currentItemChanged, this, &TabbedImages::currentItemChanged);
    connect(ui->staticListWidget, &QListWidget::currentItemChanged, this, &TabbedImages::currentItemChanged);
    connect(ui->shapesListWidget, &QListWidget::currentItemChanged, this, &TabbedImages::currentItemChanged);
    connect(ui->wordartListWidget, &QListWidget::currentItemChanged, this, &TabbedImages::currentItemChanged);

    connect(ui->buttonGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), [this](QAbstractButton * button) {
        int id = 0;
        if (button == ui->gifsButton) id = 0;
        else if (button == ui->staticButton) id = 1;
        else if (button == ui->shapesButton) id = 2;
        else if (button == ui->wordartButton) id = 3;
        ui->stackedWidget->setCurrentIndex(id);

        QListWidgetItem * item = nullptr;
        switch (id)
        {
        case 0:
            item = ui->gifsListWidget->currentItem();
            break;
        case 1:
            item = ui->staticListWidget->currentItem();
            break;
        case 2:
            item = ui->shapesListWidget->currentItem();
            break;
        case 3:
            item = ui->wordartListWidget->currentItem();
            break;
        }
        emit currentItemChanged(item, nullptr);
    });

    ui->gifsButton->click();
}

TabbedImages::~TabbedImages()
{
    delete ui;
}

void TabbedImages::addImage(TabbedImages::Type type, QString name)
{
    auto index = static_cast<std::underlying_type_t<Type>>(type);
    lists[index]->addItem(name);
    if (lists[index]->count() == 1)
    {
        lists[index]->setCurrentRow(0);
    }
}

void TabbedImages::clear()
{
    for (auto list : lists)
    {
        list->clear();
    }
}

void TabbedImages::select(QString name)
{
    for (int i = 0; auto list : lists)
    {
        auto items = list->findItems(name, Qt::MatchExactly);
        if (items.size())
        {
            auto selectedItem = items.first();

            list->setCurrentItem(selectedItem);
            list->scrollToItem(selectedItem);

            buttons[i]->click();
            break;
        }
        i++;
    }
}

TabbedImages::Type TabbedImages::type() const
{
    return static_cast<Type>(ui->stackedWidget->currentIndex());
}
