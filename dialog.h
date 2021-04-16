#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private Q_SLOTS:
    void updateDialog();

private:
    void sendData();
    Ui::Dialog *ui;

public:
    QList<int> dialogData();
};

#endif // DIALOG_H
