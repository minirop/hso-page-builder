#include "charactereditor.h"
#include "ui_charactereditor.h"

CharacterEditor::CharacterEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharacterEditor)
{
    ui->setupUi(this);
}

CharacterEditor::~CharacterEditor()
{
    delete ui;
}
