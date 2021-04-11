#ifndef CHARACTEREDITOR_H
#define CHARACTEREDITOR_H

#include <QDialog>

namespace Ui {
class CharacterEditor;
}

class CharacterEditor : public QDialog
{
    Q_OBJECT

public:
    explicit CharacterEditor(QWidget *parent = nullptr);
    ~CharacterEditor();

private:
    Ui::CharacterEditor *ui;
};

#endif // CHARACTEREDITOR_H
