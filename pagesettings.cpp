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
#include <QDir>
#include <QDoubleSpinBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

PageSettings::PageSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageSettings)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    ui->animationComboBox->addItem("None", static_cast<int>(Animation::None));
    ui->animationComboBox->addItem("TypeWriter", static_cast<int>(Animation::TypeWriter));
    ui->animationComboBox->addItem("Floating", static_cast<int>(Animation::Floating));
    ui->animationComboBox->addItem("Marquee", static_cast<int>(Animation::Marquee));

    ui->textLawBrokenComboBox->addItem("None", -1);
    ui->textLawBrokenComboBox->addItem("C: Content Infringement", 1);
    ui->textLawBrokenComboBox->addItem("H: Harassment", 2);
    ui->textLawBrokenComboBox->addItem("I: Illegal or Profane Activity", 3);
    ui->textLawBrokenComboBox->addItem("M: Malicious Software", 4);
    ui->textLawBrokenComboBox->addItem("E: Extralegal Commerce", 5);
    ui->textLawBrokenComboBox->addItem("S: Submit Evidence", 6);

    ui->gifLawBrokenComboBox->addItem("None", -1);
    ui->gifLawBrokenComboBox->addItem("C: Content Infringement", 1);
    ui->gifLawBrokenComboBox->addItem("H: Harassment", 2);
    ui->gifLawBrokenComboBox->addItem("I: Illegal or Profane Activity", 3);
    ui->gifLawBrokenComboBox->addItem("M: Malicious Software", 4);
    ui->gifLawBrokenComboBox->addItem("E: Extralegal Commerce", 5);
    ui->gifLawBrokenComboBox->addItem("S: Submit Evidence", 6);

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

        graphics->setAlign(ui->alignmentComboBox->currentIndex());
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

    connect(ui->duplicateBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
        assert(pageElement);

        emit duplicateElement(item->text(), pageElement);
    });
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

        auto eventData = QStringList() << EVENT_DEFAULT << "0" << "0" << "100" << "" << "Hypnospace" << "1741311" << "HypnoFont" << "0n" << "1" << "-1" << "-1" << "0" << "0" << "0" << "0" << "0";

        emit createElement(TYPE_TEXT, definition, eventData);
        emit updateZOrder();

        auto count = ui->elementsList->count();
        if (count > 0)
        {
            ui->elementsList->setCurrentRow(count - 1);
        }
    });
    connect(ui->addGifButton, &QPushButton::clicked, [&]() {
        QJsonArray definition;
        definition.append(0);
        definition.append(0);
        definition.append(QString(TYPE_GIF));

        auto eventData = QStringList() << EVENT_DEFAULT << "150" << "40" << "-1" << "" << "000" << "1" << "0" << "0" << "0" << "-1" << "0" << "-1" << "-1" << "-1" << "0" << "0" << "0" << "0" << "0" << "0";

        emit createElement(TYPE_GIF, definition, eventData);
        emit updateZOrder();

        auto count = ui->elementsList->count();
        if (count > 0)
        {
            ui->elementsList->setCurrentRow(count - 1);
        }
    });

    auto moveItem = [](Ui::PageSettings * ui, QListWidgetItem * item, int from, int to) {
        ui->elementsList->takeItem(from);
        ui->elementsList->insertItem(to, item);

        ui->elementsList->clearSelection();
        ui->elementsList->setCurrentItem(item, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
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
    connect(ui->pageOwnerComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        emit pageOwnerChanged(ui->pageOwnerComboBox->currentText());
    });
    connect(ui->pageDescriptionAndTags, &QPlainTextEdit::textChanged, [&]() {
        emit pageDescriptionChanged(ui->pageDescriptionAndTags->toPlainText());
    });
    connect(ui->onLoadScriptTextEdit, &QPlainTextEdit::textChanged, [&]() {
        emit onLoadScriptChanged(ui->onLoadScriptTextEdit->toPlainText());
    });
    connect(ui->pageStyleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        emit pageStyleChanged(value);
    });
    connect(ui->textLawBrokenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
        assert(pageElement);

        pageElement->setBrokenLaw(ui->textLawBrokenComboBox->currentData().toInt());
    });
    connect(ui->textCaseTagLineEdit, &QLineEdit::textChanged, [&](const QString & newText) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setCaseTag(newText);
    });
    connect(ui->widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int w) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setWidth(w);

        auto diff = abs((w - (PAGE_WIDTH/3)) / 2);
        auto xPos = (((int)graphics->x() * 100) / PAGE_WIDTH) - 50;
        auto xNewPos = std::clamp(xPos, -diff, diff-1);
        ui->xSpinBox->setValue(xNewPos);
    });
    connect(ui->xSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int x) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        auto diff = (100 - graphics->width) / 2;
        x = std::clamp(x, -diff, diff-1);
        graphics->setHSPosition(x, graphics->y());

        QSignalBlocker a(ui->xSpinBox);
        ui->xSpinBox->setValue(x);
    });
    connect(ui->noContentCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();
        assert(graphics);

        graphics->setNoContent(c);
    });
    connect(ui->bgColorBtn, &QPushButton::clicked, [&]() {
        auto c = QColorDialog::getColor(bgColor, this, "Background color");
        if (c.isValid())
        {
            emit backgroundColorChanged(c);
            setBackgroundColorButton(c);
            ui->imageSlider->setEmptyColor(c);
        }
    });
    connect(ui->pageHeightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int count) {
        emit lineCountChanged(count);
    });
    connect(ui->previousBG, &QPushButton::clicked, [&]() {
        ui->imageSlider->prev();
    });
    connect(ui->nextBG, &QPushButton::clicked, [&]() {
        ui->imageSlider->next();
    });
    connect(ui->imageSlider, &ImageSlider::backgroundChanged, this, &PageSettings::backgroundChanged);
    connect(ui->gifsListWidget, &QListWidget::currentItemChanged, [&](QListWidgetItem * current, QListWidgetItem * previous) {
        Q_UNUSED(previous)

        if (current)
        {

            auto item = ui->elementsList->currentItem();
            if (!item) return;
            auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
            assert(graphics);

            graphics->nameOf = current->text();
            graphics->refresh();

            ui->gifSlider->setGif(graphics);
        }
    });
    connect(ui->prevGif, &QPushButton::clicked, [&]() {
        auto row = ui->gifsListWidget->currentRow();
        if (row > 0)
        {
            ui->gifsListWidget->setCurrentRow(row - 1);
        }
    });
    connect(ui->nextGif, &QPushButton::clicked, [&]() {
        auto row = ui->gifsListWidget->currentRow();
        if (row < ui->gifsListWidget->count() - 1)
        {
            ui->gifsListWidget->setCurrentRow(row + 1);
        }
    });

    auto changeHSL = [&]() {
        int H = ui->hueSpinBox->value();
        int S = ui->saturationSpinBox->value();
        int L = ui->lightnessSpinBox->value();

        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setHSL(H, S, L);
    };
    connect(ui->hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->lightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->scaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setScale(value);
    });
    connect(ui->rotationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int angle) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setRotation(angle);
    });
    connect(ui->hFlipButton, &QPushButton::toggled, [&](bool toggled) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->mirror(toggled);
    });
    connect(ui->vFlipButton, &QPushButton::toggled, [&](bool toggled) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->flip(toggled);
    });
    connect(ui->gifFrameSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int offset) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setFrameOffset(offset);
    });
    connect(ui->gifLawBrokenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
        assert(pageElement);

        pageElement->setBrokenLaw(ui->gifLawBrokenComboBox->currentData().toInt());
    });
    connect(ui->gifCaseTagLineEdit, &QLineEdit::textChanged, [&](const QString & newText) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setCaseTag(newText);
    });
    connect(ui->swingOrSpinComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setSwingOrSpin(value);
    });
    connect(ui->swingOrSpinSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setSwingOrSpinSpeed(value);
    });
    connect(ui->flip3DXCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->set3DFlipX(c);
    });
    connect(ui->flip3DXSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->set3DFlipXSpeed(value);
    });
    connect(ui->flip3DYCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->set3DFlipY(c);
    });
    connect(ui->flip3DYSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->set3DFlipYSpeed(value);
    });
    connect(ui->gifFadeCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setFade(c);
    });
    connect(ui->gifFadeSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setFadeSpeed(value);
    });
    connect(ui->syncCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setSync(c);
    });
    connect(ui->gifAnimationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
        assert(graphics);

        graphics->setGifAnimation(value);
    });
    connect(ui->musicComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        Q_UNUSED(value)

        emit musicChanged(ui->musicComboBox->currentData().toString());
    });
    connect(ui->cursorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        emit cursorChanged(value);
    });

    auto scriptChanged = [ths = this](QPlainTextEdit * textEdit) {
        auto lambda = [ths, textEdit]() {
            auto item = ths->ui->elementsList->currentItem();
            if (!item) return;
            auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();
            assert(pageElement);

            pageElement->setScript(textEdit->toPlainText());
        };

        return lambda;
    };
    connect(ui->textScriptTextEdit, &QPlainTextEdit::textChanged, scriptChanged(ui->textScriptTextEdit));
    connect(ui->gifScriptTextEdit, &QPlainTextEdit::textChanged, scriptChanged(ui->gifScriptTextEdit));

    refreshGifsList();
    refreshMusicList();
    refreshUsers();
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
            ui->elementsList->setCurrentItem(item, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
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

void PageSettings::setBackground(QString image)
{
    ui->imageSlider->setImage(image);
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
        default:
            assert(false);
        }
    }
}

void PageSettings::updateGifProperties(Gif * gif)
{
    ui->stackedWidget->setCurrentIndex(2);

    QSignalBlocker a1(this);
    QSignalBlocker a2(ui->hueSpinBox);
    QSignalBlocker a3(ui->saturationSpinBox);
    QSignalBlocker a4(ui->lightnessSpinBox);
    QSignalBlocker a5(ui->scaleSpinBox);
    QSignalBlocker a6(ui->rotationSpinBox);
    QSignalBlocker a7(ui->hFlipButton);
    QSignalBlocker a8(ui->vFlipButton);
    QSignalBlocker a9(ui->swingOrSpinComboBox);
    QSignalBlocker b0(ui->swingOrSpinSpeed);
    QSignalBlocker b1(ui->flip3DXCheckBox);
    QSignalBlocker b2(ui->flip3DXSpeed);
    QSignalBlocker b3(ui->flip3DYCheckBox);
    QSignalBlocker b4(ui->flip3DYSpeed);
    QSignalBlocker b5(ui->gifFadeCheckBox);
    QSignalBlocker b6(ui->gifFadeSpeed);
    QSignalBlocker b7(ui->syncCheckBox);
    QSignalBlocker b8(ui->gifAnimationComboBox);
    QSignalBlocker b9(ui->gifFrameSpinBox);
    QSignalBlocker c0(ui->gifCaseTagLineEdit);
    QSignalBlocker c1(ui->gifScriptTextEdit);

    ui->hueSpinBox->setValue(gif->H);
    ui->saturationSpinBox->setValue(gif->S);
    ui->lightnessSpinBox->setValue(gif->L);

    ui->scaleSpinBox->setValue(gif->scale());
    ui->rotationSpinBox->setValue(gif->rotation());
    ui->hFlipButton->setChecked(gif->mirrored);
    ui->vFlipButton->setChecked(gif->flipped);

    ui->gifsListWidget->clearSelection();
    QListWidgetItem * selectedItem = nullptr;
    auto found = ui->gifsListWidget->findItems(gif->nameOf, Qt::MatchExactly);
    if (found.size())
    {
        selectedItem = found.first();
    }
    else
    {
        selectedItem = ui->gifsListWidget->item(0);
    }
    ui->gifsListWidget->clearSelection();
    ui->gifsListWidget->setCurrentItem(selectedItem, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
    ui->gifsListWidget->scrollToItem(selectedItem);

    ui->gifSlider->setGif(gif);

    ui->swingOrSpinComboBox->setCurrentIndex(gif->swingOrSpin);
    ui->swingOrSpinSpeed->setValue(gif->swingOrSpinSpeed);
    ui->flip3DXCheckBox->setChecked(gif->flip3DX);
    ui->flip3DXSpeed->setValue(gif->flip3DXSpeed);
    ui->flip3DYCheckBox->setChecked(gif->flip3DY);
    ui->flip3DYSpeed->setValue(gif->flip3DYSpeed);
    ui->gifFadeCheckBox->setChecked(gif->fade);
    ui->gifFadeSpeed->setValue(gif->fadeSpeed);
    ui->syncCheckBox->setChecked(gif->sync);
    ui->gifAnimationComboBox->setCurrentIndex(gif->gifAnimation);

    ui->gifFrameSpinBox->setValue(gif->offsetFrame);

    int lawIndex = ui->gifLawBrokenComboBox->findData(gif->brokenLaw);
    assert(lawIndex != -1);
    ui->gifLawBrokenComboBox->setCurrentIndex(lawIndex);
    ui->gifCaseTagLineEdit->setText(gif->caseTag);
    ui->gifScriptTextEdit->setPlainText(gif->script);
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
    QSignalBlocker b3(ui->bgColorBtn);
    QSignalBlocker b4(ui->pageHeightSpinBox);
    QSignalBlocker b5(ui->noContentCheckBox);
    QSignalBlocker b6(ui->textCaseTagLineEdit);
    QSignalBlocker b7(ui->textScriptTextEdit);

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

    ui->alignmentComboBox->setCurrentIndex(text->align);

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

    int lawIndex = ui->textLawBrokenComboBox->findData(text->brokenLaw);
    assert(lawIndex != -1);
    ui->textLawBrokenComboBox->setCurrentIndex(lawIndex);
    ui->textCaseTagLineEdit->setText(text->caseTag);

    ui->xSpinBox->setValue(text->xoffset);
    ui->widthSpinBox->setValue(text->width);

    ui->boldButton->setChecked(text->fontBold);
    ui->noContentCheckBox->setChecked(text->noContent);
    ui->textScriptTextEdit->setPlainText(text->script);
}

void PageSettings::setFontColorButton(QColor color)
{
    setBackgroundColor(ui->colorBtn, color);
}

void PageSettings::setFadeColorButton(QColor color)
{
    setBackgroundColor(ui->colorFadeBtn, color);
}

void PageSettings::setBackgroundColorButton(QColor color)
{
    bgColor = color;
    ui->imageSlider->setEmptyColor(color);
    setBackgroundColor(ui->bgColorBtn, color);
}

void PageSettings::setLineCounts(int count)
{
    ui->pageHeightSpinBox->setValue(count);
}

void PageSettings::setPageCursor(int pageCursor)
{
    ui->cursorComboBox->setCurrentIndex(pageCursor);
}

void PageSettings::setMusic(QString music)
{
    auto index = ui->musicComboBox->findData(music);
    if (index == -1) index = 0;
    ui->musicComboBox->setCurrentIndex(index);
}

void PageSettings::setOnLoadScript(QString script)
{
    ui->onLoadScriptTextEdit->setPlainText(script);
}

void PageSettings::setPageStyle(int style)
{
    ui->pageStyleSpinBox->setValue(style);
}

void PageSettings::setBackgroundColor(QWidget * widget, QColor color)
{
    widget->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
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

    ui->imageSlider->refresh();

    refreshGifsList();
    refreshMusicList();
    refreshUsers();
}

void PageSettings::refreshGifsList()
{
    auto selectedItems = ui->gifsListWidget->selectedItems();

    QString saved;
    if (selectedItems.size())
        saved = selectedItems.first()->text();

    ui->gifsListWidget->clear();

    auto subFolders = QStringList() << "/images/static/" << "/images/shapes/";
    QSet<QString> uniqueGifs;
    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        auto folders = QDir(path + "/images/gifs/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto folder : folders)
        {
            uniqueGifs.insert(folder);
        }

        for (auto subFolder : subFolders)
        {
            auto files = QDir(path + subFolder).entryList(QStringList() << "*.png", QDir::Files);
            for (auto f : files)
            {
                uniqueGifs.insert(f.chopped(4));
            }
        }

        auto fonts = QDir(path + "/images/wordart/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto folder : fonts)
        {
            uniqueGifs.insert(folder);
        }
    }

    auto foundFiles = uniqueGifs.values();
    if (foundFiles.size())
    {
        foundFiles.sort();
        for (auto foundFile : foundFiles)
        {
            ui->gifsListWidget->addItem(foundFile);
        }

        ui->gifsListWidget->clearSelection();
        QListWidgetItem * selectedItem = nullptr;
        if (saved.size())
        {
            auto found = ui->gifsListWidget->findItems(saved, Qt::MatchExactly);
            if (found.size())
            {
                selectedItem = found.first();
            }
            else
            {
                selectedItem = ui->gifsListWidget->item(0);
            }
        }
        else
        {
            selectedItem = ui->gifsListWidget->item(0);
        }
        ui->gifsListWidget->clearSelection();
        ui->gifsListWidget->setCurrentItem(selectedItem, QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent);
        ui->gifsListWidget->scrollToItem(selectedItem);
    }
}

void PageSettings::refreshMusicList()
{
    auto currentdata = ui->musicComboBox->currentData();
    ui->musicComboBox->clear();

    QStringList doneMusics;

    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        QDir dir(path + "/audio/music");

        for (auto txt : dir.entryList(QStringList() << "*.txt", QDir::Files, QDir::Name))
        {
            if (doneMusics.contains(txt)) continue;
            doneMusics.append(txt);

            QFile file(dir.absoluteFilePath(txt));
            if (file.open(QFile::ReadOnly))
            {
                auto data = file.readAll().split('|');

                auto formattedName = QString("\"%1\" by %2").arg(QString(data[0])).arg(QString(data[1]));
                ui->musicComboBox->addItem(formattedName, "audio\\music\\" + txt.replace(".txt", ".ogg"));

                file.close();
            }
        }

        dir.setPath(path + "/audio/hsm/pageloops");

        for (auto hsm : dir.entryList(QStringList() << "*.hsm", QDir::Files, QDir::Name))
        {
            if (doneMusics.contains(hsm)) continue;
            doneMusics.append(hsm);

            QFile file(dir.absoluteFilePath(hsm));
            if (file.open(QFile::ReadOnly))
            {
                auto data = file.readAll();
                auto json = QJsonDocument::fromJson(data);
                auto obj = json.object();
                auto jsonData = obj["data"].toArray();
                auto jsonBox = jsonData[0].toArray();
                auto jsonLine = jsonBox[0].toArray();

                auto formattedName = QString("\"%1\" by %2").arg(jsonLine[0].toString()).arg(jsonLine[1].toString());
                ui->musicComboBox->addItem(formattedName, "audio\\hsm\\pageloops\\" + hsm);

                file.close();
            }
        }
    }

    ui->musicComboBox->model()->sort(0);
    ui->musicComboBox->insertItem(0, "No Music", "");
    auto index = ui->musicComboBox->findData(currentdata);
    if (index == -1) index = 0;
    ui->musicComboBox->setCurrentIndex(index);
}

void PageSettings::refreshUsers()
{
    auto currentOwner = ui->pageOwnerComboBox->currentText();
    ui->pageOwnerComboBox->clear();

    QSet<QString> uniqueNames;

    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        QFile file(path + "/misc/chardata.hsd");
        if (file.open(QFile::ReadOnly))
        {
            auto data = file.readAll();
            auto json = QJsonDocument::fromJson(data);
            auto obj = json.object();
            auto jsonData = obj["data"].toArray();
            for (auto jsonUserData : jsonData)
            {
                auto jsonBox = jsonUserData.toArray();
                auto jsonLine = jsonBox[0].toArray();
                auto username = jsonLine[0].toString();

                uniqueNames.insert(username);
            }

            file.close();
        }
    }

    auto names = uniqueNames.values();
    names.sort();

    for (auto name : names)
    {
        ui->pageOwnerComboBox->addItem(name);
    }

    auto index = ui->pageOwnerComboBox->findText(currentOwner);
    if (index == -1) index = 0;
    ui->pageOwnerComboBox->setCurrentIndex(index);
}
