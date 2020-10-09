#include "pagesettings.h"
#include "ui_pagesettings.h"
#include "pageelement.h"
#include "gif.h"
#include "text.h"
#include <QColorDialog>

PageSettings::PageSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageSettings)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    ui->animationComboBox->addItem("None", static_cast<int>(Animation::None));
    ui->animationComboBox->addItem("TypeWriter", static_cast<int>(Animation::TypeWriter));
    ui->animationComboBox->addItem("Floating", static_cast<int>(Animation::Floating));
    ui->animationComboBox->addItem("Marquee", static_cast<int>(Animation::Marquee));

    connect(ui->elementsList, &QListWidget::currentItemChanged, this, &PageSettings::itemChanged);
    connect(ui->textEdit, &QTextEdit::textChanged, [&]() {
        auto item = ui->elementsList->currentItem();
        assert(item);
        auto graphics = item->data(Qt::UserRole+1).value<Text*>();
        assert(graphics);
        graphics->setString(ui->textEdit->toPlainText());
    });
    connect(ui->colorBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        assert(item);
        auto graphics = item->data(Qt::UserRole+1).value<Text*>();
        assert(graphics);

        auto c = QColorDialog::getColor(graphics->fontColor, this, "Font color");
        if (c.isValid())
        {
            graphics->setFontColor(c);
            setFontColorButton(c);
        }
    });
    connect(ui->animationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        auto item = ui->elementsList->currentItem();
        assert(item);
        auto graphics = item->data(Qt::UserRole+1).value<Text*>();
        assert(graphics);

        graphics->setAnimation(ui->animationComboBox->currentData().toInt(), ui->speedSpinBox->value());
    });
    connect(ui->speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int) {
        auto item = ui->elementsList->currentItem();
        assert(item);
        auto graphics = item->data(Qt::UserRole+1).value<Text*>();
        assert(graphics);

        graphics->setAnimation(ui->animationComboBox->currentData().toInt(), ui->speedSpinBox->value());
    });
}

PageSettings::~PageSettings()
{
    delete ui;
}

void PageSettings::select(int idSel)
{
    for (int i = 1; i < ui->elementsList->count(); i++)
    {
        auto item = ui->elementsList->item(i);
        auto id = item->data(Qt::UserRole).toInt();

        if (id == idSel)
        {
            ui->elementsList->setCurrentItem(item, QItemSelectionModel::SelectCurrent);
            break;
        }
    }
}

void PageSettings::clearSelection()
{
    ui->elementsList->clearSelection();
    ui->stackedWidget->setCurrentIndex(0);
    ui->elementName->clear();
}

void PageSettings::itemChanged(QListWidgetItem * item, QListWidgetItem * previous)
{
    ui->elementName->setText(item->text());

    updateProperties(item);

    int oldSel = previous ? previous->data(Qt::UserRole).toInt() : -1;

    emit selectionChanged(item->data(Qt::UserRole).toInt(), oldSel);
}

void PageSettings::updateProperties(QListWidgetItem * item)
{
    auto elem = item->data(Qt::UserRole + 1).value<PageElement*>();
    if (elem)
    {
        switch (elem->elementType())
        {
        case PageElement::ElementType::Gif:
            updateGifProperties(qobject_cast<Gif*>(elem));
            break;
        case PageElement::ElementType::Text:
            updateTextProperties(qobject_cast<Text*>(elem));
            break;
        }
    }
}

void PageSettings::updateGifProperties(Gif * gif)
{
    ui->stackedWidget->setCurrentIndex(2);
}

void PageSettings::updateTextProperties(Text * text)
{
    ui->stackedWidget->setCurrentIndex(1);

    ui->textEdit->setText(text->string);

    setFontColorButton(text->fontColor);
}

void PageSettings::setFontColorButton(QColor color)
{
    ui->colorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
}
