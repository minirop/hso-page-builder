#include "pagesettings.h"
#include "ui_pagesettings.h"
#include "appsettings.h"
#include "pageelement.h"
#include "gif.h"
#include "text.h"
#include "fontdatabase.h"
#include "globals.h"
#include <QColorDialog>
#include <QGraphicsScene>
#include <algorithm>

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

    ui->alignmentComboBox->addItem("Left", 0);
    ui->alignmentComboBox->addItem("Centre", 1);
    ui->alignmentComboBox->addItem("Right", 2);

    ui->lawBrokenComboBox->addItem("None", -1);
    ui->lawBrokenComboBox->addItem("C: Content Infringement", 1);
    ui->lawBrokenComboBox->addItem("H: Harassment", 2);
    ui->lawBrokenComboBox->addItem("I: Illegal or Profane Activity", 3);
    ui->lawBrokenComboBox->addItem("M: Malicious Software", 4);
    ui->lawBrokenComboBox->addItem("E: Extralegal Commerce", 5);
    ui->lawBrokenComboBox->addItem("S: Submit Evidence", 6);

    connect(ui->elementsList, &QListWidget::currentItemChanged, this, &PageSettings::itemChanged);
    connect(ui->elementName, &QLineEdit::textChanged, [&](QString newName) {
        auto item = ui->elementsList->currentItem();
        if (item)
        {
            item->setText(newName);
        }

        emit selectedNameChanged(newName);
    });
    connect(ui->textEdit, &QTextEdit::textChanged, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);
        graphics->setString(ui->textEdit->toPlainText());
    });
    connect(ui->colorBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        auto c = QColorDialog::getColor(graphics->fontColor, this, "Font color");
        if (c.isValid())
        {
            graphics->setFontColor(c);
            setFontColorButton(c);
        }
    });
    connect(ui->animationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setAnimation(ui->animationComboBox->currentData().toInt());
    });
    connect(ui->speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setAnimationSpeed(ui->speedSpinBox->value());
    });
    connect(ui->alignmentComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setAlign(ui->alignmentComboBox->currentData().toInt());
    });

    connect(ui->colorFadeBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        auto c = QColorDialog::getColor(graphics->fadeColor, this, "Fade color");
        if (c.isValid())
        {
            graphics->setFade(c, ui->fadeSpeedSpinBox->value());
            setFadeColorButton(c);
        }
    });
    connect(ui->fadeSpeedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setFade(graphics->fadeColor, ui->fadeSpeedSpinBox->value());
    });

    auto setFontCallback = [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        auto fontSize = ui->fontSizeGroup->checkedButton()->text();
        graphics->setFontSize(fontSize.toInt());

        auto fontBold = ui->boldButton->isChecked();
        graphics->setFontBold(fontBold);

        auto newFont = ui->fontsCombo->currentText();
        graphics->setFont(newFont.toLower());
    };

    connect(ui->fontsCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), setFontCallback);
    connect(ui->fontSizeGroup, &QButtonGroup::idClicked, setFontCallback);
    connect(ui->boldButton, &QPushButton::clicked, setFontCallback);

    connect(ui->deleteBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto txt = item->data(ROLE_ELEMENT).value<Text*>();
        auto gif = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(txt || gif);

        QGraphicsItem * graphics = nullptr;
        if (txt)
        {
            graphics = txt;
            txt->deleteLater();
        }
        else if (gif)
        {
            graphics = gif;
            gif->deleteLater();
        }

        if (graphics)
        {
            graphics->scene()->removeItem(graphics);
        }

        delete ui->elementsList->takeItem(ui->elementsList->row(item));
    });

    connect(ui->addTextButton, &QPushButton::clicked, [&]() {
        QJsonArray definition;
        definition.append(0);
        definition.append(0);
        definition.append(QString(TYPE_TEXT));

        auto eventData = QStringList() << EVENT_DEFAULT << "0" << "0" << "100" << "-1" << "Hypnospace" << "1741311" << "HypnoFont" << "0n" << "1" << "-1" << "-1" << "0" << "0" << "" << "0";

        emit createElement(TYPE_TEXT, definition, eventData);
        emit updateZOrder();
    });

    auto moveItem = [](Ui::PageSettings * ui, QListWidgetItem * item, int from, int to) {
        ui->elementsList->takeItem(from);
        ui->elementsList->insertItem(to, item);

        ui->elementsList->clearSelection();
        ui->elementsList->setCurrentItem(item, QItemSelectionModel::SelectCurrent);
    };

    connect(ui->moveUp, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row < 1) return; // already at the top

            moveItem(ui, item, row, row - 1);
            emit updateZOrder();
        }
    });
    connect(ui->moveDown, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row == ui->elementsList->count() - 1) return; // already at the bottom

            moveItem(ui, item, row, row + 1);
            emit updateZOrder();
        }
    });
    connect(ui->moveToTop, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row < 1) return; // already at the top

            moveItem(ui, item, row, 0);
            emit updateZOrder();
        }
    });
    connect(ui->moveToBottom, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            auto count = ui->elementsList->count();
            if (row == count - 1) return; // already at the bottom

            moveItem(ui, item, row, count - 1);
            emit updateZOrder();
        }
    });

    connect(ui->pageTitleLineEdit, &QLineEdit::textChanged, [&](QString title) {
        emit pageTitleChanged(title);
    });
    connect(ui->pageOwnerLineEdit, &QLineEdit::textChanged, [&](QString owner) {
        emit pageOwnerChanged(owner);
    });
    connect(ui->pageDescriptionAndTags, &QPlainTextEdit::textChanged, [&]() {
        emit pageDescriptionChanged(ui->pageDescriptionAndTags->toPlainText());
    });
    connect(ui->lawBrokenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
        assert(pageElement);

        pageElement->setBrokenLaw(ui->lawBrokenComboBox->currentData().toInt());
    });
    connect(ui->widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int w) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setWidth(w);

        auto diff = abs((w - (PAGE_WIDTH/3)) / 2);
        auto xPos = (((int)graphics->x() * 100) / PAGE_WIDTH) - 50;
        auto xNewPos = std::clamp(xPos, -diff, diff);
        ui->xSpinBox->setValue(xNewPos);
    });
    connect(ui->xSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int x) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        auto diff = (100 - graphics->width) / 2;
        x = std::clamp(x, -diff, diff);
        graphics->setHSPosition(x, graphics->y());

        QSignalBlocker a(ui->xSpinBox);
        ui->xSpinBox->setValue(x);
    });
}

PageSettings::~PageSettings()
{
    delete ui;
}

void PageSettings::select(int idSel)
{
    for (int i = 0; i < ui->elementsList->count(); i++)
    {
        auto item = ui->elementsList->item(i);
        auto id = item->data(ROLE_ID).toInt();

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
    int newSel = -1;
    QString newName;

    if (item)
    {
        newName = item->text();
        ui->elementName->setText(item->text());

        updateProperties(item);

        newSel = item->data(ROLE_ID).toInt();
    }
    else
    {
        ui->elementName->clear();
        ui->stackedWidget->setCurrentIndex(0);
    }

    int oldSel = previous ? previous->data(ROLE_ID).toInt() : -1;

    emit selectionChanged(newSel, oldSel);
    emit selectedNameChanged(newName);
}

void PageSettings::updateProperties(QListWidgetItem * item)
{
    auto elem = item->data(ROLE_ELEMENT).value<PageElement*>();
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
    Q_UNUSED(gif)

    ui->stackedWidget->setCurrentIndex(2);
}

void PageSettings::updateTextProperties(Text * text)
{
    ui->stackedWidget->setCurrentIndex(1);

    QSignalBlocker a1(this);
    QSignalBlocker a2(ui->fontsCombo);
    QSignalBlocker a3(ui->fontSizeGroup);
    QSignalBlocker a4(ui->animationComboBox);
    QSignalBlocker a5(ui->fadeSpeedSpinBox);
    QSignalBlocker a6(ui->textEdit);
    QSignalBlocker a7(ui->boldButton);
    QSignalBlocker a8(ui->colorBtn);
    QSignalBlocker a9(ui->colorFadeBtn);
    QSignalBlocker b0(ui->alignmentComboBox);
    QSignalBlocker b1(ui->xSpinBox);
    QSignalBlocker b2(ui->widthSpinBox);

    auto fontsNames = FontDatabase::GetFonts();

    ui->fontsCombo->clear();
    for (auto name : fontsNames)
    {
        ui->fontsCombo->addItem(name);
    }

    auto index = ui->fontsCombo->findText(text->fontName, Qt::MatchStartsWith);
    ui->fontsCombo->setCurrentIndex(index);

    ui->textEdit->setText(text->string);

    setFontColorButton(text->fontColor);

    int animIndex = ui->animationComboBox->findData(static_cast<int>(text->animation));
    assert(animIndex != -1);
    ui->animationComboBox->setCurrentIndex(animIndex);
    ui->speedSpinBox->setValue(text->animationSpeed);

    int alignIndex = ui->alignmentComboBox->findData(text->align);
    assert(alignIndex != -1);
    ui->alignmentComboBox->setCurrentIndex(alignIndex);

    setFadeColorButton(text->fadeColor);
    ui->fadeSpeedSpinBox->setValue(text->fadeSpeed);

    switch (text->fontSize)
    {
    case 0:
        ui->size0Button->click();
        break;
    case 1:
        ui->size1Button->click();
        break;
    case 2:
        ui->size2Button->click();
        break;
    }

    int lawIndex = ui->lawBrokenComboBox->findData(text->brokenLaw);
    assert(lawIndex != -1);
    ui->lawBrokenComboBox->setCurrentIndex(lawIndex);

    ui->xSpinBox->setValue(text->xoffset);
    ui->widthSpinBox->setValue(text->width);

    ui->boldButton->setChecked(text->fontBold);
}

void PageSettings::setFontColorButton(QColor color)
{
    ui->colorBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PageSettings::setFadeColorButton(QColor color)
{
    ui->colorFadeBtn->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
}

void PageSettings::refresh()
{
    // update elements
    for (int i = 0; i < ui->elementsList->count(); i++)
    {
        auto item = ui->elementsList->item(i);
        if (!item) continue;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
        assert(pageElement);

        pageElement->refresh();
    }

    // update form
    auto items = ui->elementsList->selectedItems();
    if (items.size() == 1)
    {
        auto item = items.first();
        updateProperties(item);
    }
}
