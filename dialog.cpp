#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->agencyComboBox->hide();
    ui->agencyLabel->hide();

    connect(ui->themeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDialog()));
    connect(ui->fonctionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDialog()));
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}
Dialog::~Dialog()
{
    delete ui;
}

QList<int> Dialog::dialogData(){
    QList<int> list;
    list.append(ui->themeComboBox->currentIndex());
    list.append(ui->fonctionComboBox->currentIndex());
    if(ui->fonctionComboBox->currentIndex() == 1)
        list.append(ui->agencyComboBox->currentIndex()+1);
    return list;
}

void Dialog::updateDialog()
{
    int theme = ui->themeComboBox->currentIndex();
    int function = ui->fonctionComboBox->currentIndex();

    if(function == 1){
        ui->agencyLabel->show();
        ui->agencyComboBox->show();
    }
    else{
        ui->agencyComboBox->hide();
        ui->agencyLabel->hide();
    }

    QPalette pal = window()->palette();
    if(theme == 1){
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    }
    else{
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
    }
    this->setPalette(pal);

}
