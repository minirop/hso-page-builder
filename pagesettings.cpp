#include "pagesettings.h"
#include "ui_pagesettings.h"
#include "ui_tabbedimages.h"
#include "appsettings.h"
#include "pageelement.h"
#include "gif.h"
#include "text.h"
#include "fontdatabase.h"
#include "globals.h"
#include "eventslist.h"
#include "eventslistfiltermodel.h"
#include "mainwindow.h"
#include <QColorDialog>
#include <QGraphicsScene>
#include <algorithm>
#include <QDir>
#include <QDoubleSpinBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define CHECK_COMBO_BOX_INDEX(value) if (value == -1) value = 0

PageSettings::PageSettings(MainWindow * parent) :
    QWidget(parent),
    ui(new Ui::PageSettings),
    mainWindow(parent)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    webpageEventsList = new EventsList(this);
    webpageInactiveEvents = new EventsListFilterModel(false, this);
    webpageInactiveEvents->setSourceModel(webpageEventsList);
    ui->webpageEventsNamesComboBox->setModel(webpageInactiveEvents);

    elementsEventsList = new EventsList(this);
    elementsInactiveEvents = new EventsListFilterModel(false, this);
    elementsInactiveEvents->setSourceModel(elementsEventsList);
    ui->elementsEventsNamesComboBox->setModel(elementsInactiveEvents);

    ui->animationComboBox->addItem("None", static_cast<int>(Animation::None));
    ui->animationComboBox->addItem("TypeWriter", static_cast<int>(Animation::TypeWriter));
    ui->animationComboBox->addItem("Floating", static_cast<int>(Animation::Floating));
    ui->animationComboBox->addItem("Marquee", static_cast<int>(Animation::Marquee));

    ui->gifAnimationComboBox->addItem("Always animate", 0);
    ui->gifAnimationComboBox->addItem("Animate on mouse over", 1);
    ui->gifAnimationComboBox->addItem("Still image", -1);
    ui->gifAnimationComboBox->addItem("Simulate a button", -2);

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

        graphics->setString(ui->textEdit->toPlainText());
    });
    connect(ui->colorBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        auto c = QColorDialog::getColor(graphics->fontColor(), this, "Font color");
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

        graphics->setAnimation(ui->animationComboBox->currentData().toInt());
    });
    connect(ui->speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        graphics->setAnimationSpeed(ui->speedSpinBox->value());
    });
    connect(ui->alignmentComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        graphics->setAlign(ui->alignmentComboBox->currentIndex());
    });

    connect(ui->colorFadeBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        auto c = QColorDialog::getColor(graphics->fadeColor(), this, "Fade color");
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

        graphics->setFade(graphics->fadeColor(), ui->fadeSpeedSpinBox->value());
    });

    auto setFontCallback = [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        auto fontSize = ui->fontSizeGroup->checkedButton()->text();
        graphics->setFontSize(fontSize.toInt());

        auto fontBold = ui->boldButton->isChecked();
        graphics->setFontBold(fontBold);

        auto newFont = ui->fontsCombo->currentText();
        graphics->setFont(newFont.toLower());
    };

    connect(ui->fontsCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), setFontCallback);
    connect(ui->fontSizeGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked), setFontCallback);
    connect(ui->boldButton, &QPushButton::clicked, setFontCallback);

    connect(ui->duplicateBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        emit duplicateElement(item->text(), pageElement);
    });
    connect(ui->deleteBtn, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto txt = item->data(ROLE_ELEMENT).value<Text*>();
        auto gif = item->data(ROLE_ELEMENT).value<Gif*>();

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

    auto moveItem = [](QListWidget * listWidget, QListWidgetItem * item, int from, int to) {
        listWidget->takeItem(from);
        listWidget->insertItem(to, item);

        listWidget->setCurrentItem(item);
    };

    connect(ui->moveUp, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->elementsList, item, row, row - 1);
            mainWindow->webpage->moveActiveEvent(row, row - 1);
            emit updateZOrder();
        }
    });
    connect(ui->moveDown, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row == 0) return; // can't move "default"
            if (row == ui->elementsList->count() - 1) return; // already at the bottom

            moveItem(ui->elementsList, item, row, row + 1);
            mainWindow->webpage->moveActiveEvent(row, row + 1);
            emit updateZOrder();
        }
    });
    connect(ui->moveToTop, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->elementsList, item, row, 0);
            mainWindow->webpage->moveActiveEvent(row, 1);
            emit updateZOrder();
        }
    });
    connect(ui->moveToBottom, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();

        if (item)
        {
            auto row = ui->elementsList->row(item);
            if (row == 0) return; // can't move "default"
            auto count = ui->elementsList->count();
            if (row == count - 1) return; // already at the bottom

            moveItem(ui->elementsList, item, row, count - 1);
            mainWindow->webpage->moveActiveEvent(row, count - 1);
            emit updateZOrder();
        }
    });

    connect(ui->elementsMoveUpEvent, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        item = ui->elementsEventsList->currentItem();
        if (item)
        {
            auto row = ui->elementsEventsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->elementsEventsList, item, row, row - 1);
            pageElement->moveActiveEvent(row, row - 1);
        }
    });
    connect(ui->elementsMoveDownEvent, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        item = ui->elementsEventsList->currentItem();
        if (item)
        {
            auto row = ui->elementsEventsList->row(item);
            if (row == 0) return; // can't move "default"
            if (row == ui->elementsEventsList->count() - 1) return; // already at the bottom

            moveItem(ui->elementsEventsList, item, row, row + 1);
            pageElement->moveActiveEvent(row, row + 1);
        }
    });
    connect(ui->elementsMoveToTopEvent, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        item = ui->elementsEventsList->currentItem();
        if (item)
        {
            auto row = ui->elementsEventsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->elementsEventsList, item, row, 1);
            pageElement->moveActiveEvent(row, 1);
        }
    });
    connect(ui->elementsMoveToBottomEvent, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        item = ui->elementsEventsList->currentItem();
        if (item)
        {
            auto row = ui->elementsEventsList->row(item);
            if (row == 0) return; // can't move "default"
            auto count = ui->elementsEventsList->count();
            if (row == count - 1) return; // already at the bottom

            moveItem(ui->elementsEventsList, item, row, count - 1);
            pageElement->moveActiveEvent(row, count - 1);
        }
    });

    connect(ui->webpageMoveUpEvent, &QPushButton::clicked, [&]() {
        auto item = ui->webpageEventsList->currentItem();

        if (item)
        {
            auto row = ui->webpageEventsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->webpageEventsList, item, row, row - 1);
        }
    });
    connect(ui->webpageMoveDownEvent, &QPushButton::clicked, [&]() {
        auto item = ui->webpageEventsList->currentItem();

        if (item)
        {
            auto row = ui->webpageEventsList->row(item);
            if (row == 0) return; // can't move "default"
            if (row == ui->webpageEventsList->count() - 1) return; // already at the bottom

            moveItem(ui->webpageEventsList, item, row, row + 1);
        }
    });
    connect(ui->webpageMoveToTopEvent, &QPushButton::clicked, [&]() {
        auto item = ui->webpageEventsList->currentItem();

        if (item)
        {
            auto row = ui->webpageEventsList->row(item);
            if (row < 2) return; // already at the top

            moveItem(ui->webpageEventsList, item, row, 1);
        }
    });
    connect(ui->webpageMoveToBottomEvent, &QPushButton::clicked, [&]() {
        auto item = ui->webpageEventsList->currentItem();

        if (item)
        {
            auto row = ui->webpageEventsList->row(item);
            if (row == 0) return; // can't move "default"
            auto count = ui->webpageEventsList->count();
            if (row == count - 1) return; // already at the bottom

            moveItem(ui->webpageEventsList, item, row, count - 1);
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

        ui->pageStylePreview->setPixmap(AppSettings::GetFilePath(QString("/images/gifs/zoneselectorb/zoneselectorb%1.png").arg(value, 2, 10, QChar('0'))));
    });
    connect(ui->textLawBrokenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        pageElement->setBrokenLaw(ui->textLawBrokenComboBox->currentData().toInt());
    });
    connect(ui->textCaseTagLineEdit, &QLineEdit::textChanged, [&](const QString & newText) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

        graphics->setCaseTag(newText);
    });
    connect(ui->widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int w) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

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

        auto diff = (100 - graphics->width()) / 2;
        x = std::clamp(x, -diff, diff-1);
        graphics->setHSPosition(x, graphics->y());

        QSignalBlocker a(ui->xSpinBox);
        ui->xSpinBox->setValue(x);
    });
    connect(ui->noContentCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Text*>();

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
    connect(ui->tabbedImages, &TabbedImages::currentItemChanged, [&](QListWidgetItem * current, QListWidgetItem * previous) {
        Q_UNUSED(previous)

        if (current)
        {
            auto item = ui->elementsList->currentItem();
            if (!item) return;
            auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();
            if (!graphics) return;

            graphics->setNameOf(current->text());
            graphics->refresh();

            ui->gifSlider->setGif(graphics);
        }
    });
    connect(ui->prevGif, &QPushButton::clicked, [&]() {
        QListWidget * list = nullptr;
        switch (ui->tabbedImages->type())
        {
        case TabbedImages::Type::Gif:
            list = ui->tabbedImages->ui->gifsListWidget;
            break;
        case TabbedImages::Type::Static:
            list = ui->tabbedImages->ui->staticListWidget;
            break;
        case TabbedImages::Type::Shape:
            list = ui->tabbedImages->ui->shapesListWidget;
            break;
        case TabbedImages::Type::Wordart:
            list = ui->tabbedImages->ui->wordartListWidget;
            break;
        default:
            assert(false);
        }

        int row = list->currentRow();
        if (row > 0)
        {
            list->setCurrentRow(row - 1);
        }
    });
    connect(ui->nextGif, &QPushButton::clicked, [&]() {
        QListWidget * list = nullptr;
        switch (ui->tabbedImages->type())
        {
        case TabbedImages::Type::Gif:
            list = ui->tabbedImages->ui->gifsListWidget;
            break;
        case TabbedImages::Type::Static:
            list = ui->tabbedImages->ui->staticListWidget;
            break;
        case TabbedImages::Type::Shape:
            list = ui->tabbedImages->ui->shapesListWidget;
            break;
        case TabbedImages::Type::Wordart:
            list = ui->tabbedImages->ui->wordartListWidget;
            break;
        default:
            assert(false);
        }

        int row = list->currentRow();
        if (row < list->count() - 1)
        {
            list->setCurrentRow(row + 1);
        }
    });

    auto changeHSL = [&]() {
        int H = ui->hueSpinBox->value();
        int S = ui->saturationSpinBox->value();
        int L = ui->lightnessSpinBox->value();

        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setHSL(H, S, L);
    };
    connect(ui->hueSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->saturationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->lightnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), changeHSL);
    connect(ui->scaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&](double value) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setHSScale(value);
    });
    connect(ui->rotationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int angle) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setHSRotation(angle);
    });
    connect(ui->hFlipButton, &QPushButton::toggled, [&](bool toggled) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->mirror(toggled);
    });
    connect(ui->vFlipButton, &QPushButton::toggled, [&](bool toggled) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->flip(toggled);
    });
    connect(ui->gifFrameSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [&](int offset) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setFrameOffset(offset);
    });
    connect(ui->gifLawBrokenComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        pageElement->setBrokenLaw(ui->gifLawBrokenComboBox->currentData().toInt());
    });
    connect(ui->gifCaseTagLineEdit, &QLineEdit::textChanged, [&](const QString & newText) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setCaseTag(newText);
    });
    connect(ui->swingOrSpinComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setSwingOrSpin(value);
    });
    connect(ui->swingOrSpinSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setSwingOrSpinSpeed(value);
    });
    connect(ui->flip3DXCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->set3DFlipX(c);
    });
    connect(ui->flip3DXSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->set3DFlipXSpeed(value);
    });
    connect(ui->flip3DYCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->set3DFlipY(c);
    });
    connect(ui->flip3DYSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->set3DFlipYSpeed(value);
    });
    connect(ui->gifFadeCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setFade(c);
    });
    connect(ui->gifFadeSpeed, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value){
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setFadeSpeed(value);
    });
    connect(ui->syncCheckBox, &QCheckBox::toggled, [&](bool c) {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setSync(c);
    });
    connect(ui->gifAnimationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int value){
        Q_UNUSED(value)

        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto graphics = item->data(ROLE_ELEMENT).value<Gif*>();

        graphics->setGifAnimation(ui->gifAnimationComboBox->currentData().toInt());
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

            pageElement->setScript(textEdit->toPlainText());
        };

        return lambda;
    };
    connect(ui->textScriptTextEdit, &QPlainTextEdit::textChanged, scriptChanged(ui->textScriptTextEdit));
    connect(ui->gifScriptTextEdit, &QPlainTextEdit::textChanged, scriptChanged(ui->gifScriptTextEdit));

    connect(ui->webpageAddEventButton, &QPushButton::clicked, [&]() {
        auto name = ui->webpageEventsNamesComboBox->currentText();
        webpageEventsList->setEventActive(name, true);

        ui->webpageEventsList->addItem(name);
        ui->webpageEventsList->setCurrentRow(ui->webpageEventsList->count() - 1);
    });
    connect(ui->webpageEventsList, &QListWidget::currentItemChanged, [&](QListWidgetItem * current, QListWidgetItem * previous) {
        Q_UNUSED(previous)

        if (current)
            emit webpageEventSelected(current->text());
    });
    connect(ui->webpageDeleteEvent, &QPushButton::clicked, [&]() {
        if (ui->webpageEventsList->count() > 1)
        {
            if (ui->webpageEventsList->currentRow() == 0) return;

            auto item = ui->webpageEventsList->currentItem();
            if (item)
            {
                auto dirty = AppSettings::IsPageDirty();

                auto name = item->text();
                webpageEventsList->setEventActive(name, false);
                emit webpageEventDeactivated(name);

                auto deletedEvents = ui->webpageEventsList->findItems(name, Qt::MatchExactly);
                for (auto deletedEvent : deletedEvents)
                {
                    delete deletedEvent;
                }

                AppSettings::SetPageDirty(dirty);
            }
        }
    });
    connect(ui->elementsAddEventButton, &QPushButton::clicked, [&]() {
        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        auto name = ui->elementsEventsNamesComboBox->currentText();
        elementsEventsList->setEventActive(name, true);
        ui->elementsEventsList->addItem(name);

        pageElement->setEvent(name);

        ui->elementsEventsList->setCurrentRow(0);
    });
    connect(ui->elementsEventsList, &QListWidget::currentItemChanged, [&](QListWidgetItem *current) {
        if (!current) return;

        auto item = ui->elementsList->currentItem();
        if (!item) return;
        auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

        auto dirty = AppSettings::IsPageDirty();

        auto name = current->text();
        pageElement->setEvent(name);
        pageElement->refresh();
        updateProperties(pageElement);

        AppSettings::SetPageDirty(dirty);
    });
    connect(ui->elementsDeleteEvent, &QPushButton::clicked, [&]() {
        if (ui->elementsEventsList->count() > 1)
        {
            if (ui->elementsEventsList->currentRow() == 0) return;

            auto item = ui->elementsEventsList->currentItem();
            if (item)
            {
                auto name = item->text();
                elementsEventsList->setEventActive(name, false);
                emit elementsEventDeactivated(name);

                auto item = ui->elementsList->currentItem();
                if (!item) return;
                auto pageElement = item->data(ROLE_ELEMENT).value<PageElement*>();

                pageElement->clearEvent(name);

                auto deletedEvents = ui->elementsEventsList->findItems(name, Qt::MatchExactly);
                for (auto deletedEvent : deletedEvents)
                {
                    delete deletedEvent;
                }

                ui->elementsEventsList->setCurrentRow(0);
            }
        }
    });
    connect(ui->editUsersBtn, &QPushButton::clicked, [&]() {
        refreshUsers();
    });

    // to force the page style image to be displayed.
    setPageStyle(2);
    setPageStyle(1);

    refreshGifsList();
    refreshMusicList();
    refreshUsers();
    refreshEvents();
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

        auto elem = item->data(ROLE_ELEMENT).value<PageElement*>();

        elementsEventsList->reset();

        ui->elementsEventsList->clear();
        for (auto name : elem->activeEvents())
        {
            ui->elementsEventsList->addItem(name);
            elementsEventsList->setEventActive(name, true);
        }
        ui->elementsEventsNamesComboBox->setCurrentIndex(0);

        ui->elementsEventsList->setCurrentRow(0);

        newSel = item->data(ROLE_ID).toInt();
    }
    else
    {
        ui->elementName->clear();
        ui->stackedWidget->setCurrentIndex(0);
    }

    int oldSel = -1;

    if (previous)
    {
        auto pageElement = previous->data(ROLE_ELEMENT).value<PageElement*>();

        pageElement->setEvent(EVENT_DEFAULT);
        pageElement->refresh();

        oldSel = previous->data(ROLE_ID).toInt();
    }

    emit selectionChanged(newSel, oldSel);
    emit selectedNameChanged(newName);
}

void PageSettings::updateProperties(QListWidgetItem * item)
{
    auto elem = item->data(ROLE_ELEMENT).value<PageElement*>();
    if (elem)
    {
        updateProperties(elem);
    }
}

void PageSettings::updateProperties(PageElement * elem)
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

    ui->hueSpinBox->setValue(gif->H());
    ui->saturationSpinBox->setValue(gif->S());
    ui->lightnessSpinBox->setValue(gif->L());

    ui->scaleSpinBox->setValue(gif->HSScale());
    ui->rotationSpinBox->setValue(gif->HSRotation());
    ui->hFlipButton->setChecked(gif->mirrored());
    ui->vFlipButton->setChecked(gif->flipped());

    ui->tabbedImages->select(gif->nameOf());
    ui->gifSlider->setGif(gif);

    ui->swingOrSpinComboBox->setCurrentIndex(gif->swingOrSpin());
    ui->swingOrSpinSpeed->setValue(gif->swingOrSpinSpeed());
    ui->flip3DXCheckBox->setChecked(gif->flip3DX());
    ui->flip3DXSpeed->setValue(gif->flip3DXSpeed());
    ui->flip3DYCheckBox->setChecked(gif->flip3DY());
    ui->flip3DYSpeed->setValue(gif->flip3DYSpeed());
    ui->gifFadeCheckBox->setChecked(gif->fade());
    ui->gifFadeSpeed->setValue(gif->fadeSpeed());
    ui->syncCheckBox->setChecked(gif->sync());

    auto mouseOverIndex = ui->gifAnimationComboBox->findData(gif->gifAnimation());
    CHECK_COMBO_BOX_INDEX(mouseOverIndex);
    ui->gifAnimationComboBox->setCurrentIndex(mouseOverIndex);

    ui->gifFrameSpinBox->setValue(gif->offsetFrame());

    auto lawIndex = ui->gifLawBrokenComboBox->findData(gif->brokenLaw());
    CHECK_COMBO_BOX_INDEX(lawIndex);
    ui->gifLawBrokenComboBox->setCurrentIndex(lawIndex);
    ui->gifCaseTagLineEdit->setText(gif->caseTag());
    ui->gifScriptTextEdit->setPlainText(gif->script());
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

    auto index = ui->fontsCombo->findText(text->fontName(), Qt::MatchStartsWith);
    ui->fontsCombo->setCurrentIndex(index);

    ui->textEdit->setText(text->string());

    setFontColorButton(text->fontColor());

    int animIndex = ui->animationComboBox->findData(static_cast<int>(text->animation()));
    CHECK_COMBO_BOX_INDEX(animIndex);
    ui->animationComboBox->setCurrentIndex(animIndex);
    ui->speedSpinBox->setValue(text->animationSpeed());

    ui->alignmentComboBox->setCurrentIndex(text->align());

    setFadeColorButton(text->fadeColor());
    ui->fadeSpeedSpinBox->setValue(text->fadeSpeed());

    switch (text->fontSize())
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

    int lawIndex = ui->textLawBrokenComboBox->findData(text->brokenLaw());
    CHECK_COMBO_BOX_INDEX(lawIndex);
    ui->textLawBrokenComboBox->setCurrentIndex(lawIndex);
    ui->textCaseTagLineEdit->setText(text->caseTag());

    ui->xSpinBox->setValue(text->xoffset());
    ui->widthSpinBox->setValue(text->width());

    ui->boldButton->setChecked(text->fontBold());
    ui->noContentCheckBox->setChecked(text->noContent());
    ui->textScriptTextEdit->setPlainText(text->script());
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
    refreshEvents();
}

void PageSettings::refreshGifsList()
{
    ui->tabbedImages->clear();

    auto subFolders = QStringList() << "/images/static/" << "/images/shapes/";
    auto paths = AppSettings::GetSearchPaths();
    for (auto path : paths)
    {
        auto folders = QDir(path + "/images/gifs/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto folder : folders)
        {
            ui->tabbedImages->addImage(TabbedImages::Type::Gif, folder);
        }

        auto type = TabbedImages::Type::Static;
        for (auto subFolder : subFolders)
        {
            auto files = QDir(path + subFolder).entryList(QStringList() << "*.png", QDir::Files);
            for (auto f : files)
            {
                ui->tabbedImages->addImage(type, f.chopped(4));
            }

            type = TabbedImages::Type::Shape;
        }

        auto fonts = QDir(path + "/images/wordart/").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (auto folder : fonts)
        {
            ui->tabbedImages->addImage(TabbedImages::Type::Wordart, folder);
        }
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
    auto oldDirty = AppSettings::IsPageDirty();

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
    if (index == -1)
    {
        index = 0;
        // if the user has been deleted, the page is now dirty.
        if (currentOwner.size())
        {
            oldDirty = true;
        }
    }
    ui->pageOwnerComboBox->setCurrentIndex(index);

    AppSettings::SetPageDirty(oldDirty);
}

void PageSettings::refreshEvents()
{
    webpageEventsList->clear();
    webpageEventsList->addEvent(EVENT_DEFAULT);

    elementsEventsList->clear();
    elementsEventsList->addEvent(EVENT_DEFAULT);

    auto eventsFile = AppSettings::GetFilePath("/misc/events.txt");
    QFile f(eventsFile);
    if (f.open(QFile::ReadOnly))
    {
        auto eventsNames = QString(f.readAll()).split('\n',
#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
                                                      QString::SkipEmptyParts
#else
                                                      Qt::SkipEmptyParts
#endif
                                                      );
        for (auto ev : eventsNames)
        {
            auto name = ev.trimmed();
            webpageEventsList->addEvent(name);
            elementsEventsList->addEvent(name);
            realEventsNames.append(name);
        }
        f.close();
    }

    webpageEventsList->setEventActive(EVENT_DEFAULT, true);

    ui->webpageEventsList->clear();
    ui->webpageEventsList->addItem(EVENT_DEFAULT);

    ui->webpageEventsList->setCurrentRow(0);
}

void PageSettings::reset()
{
    ui->elementsList->clear();
    refreshEvents();
}
