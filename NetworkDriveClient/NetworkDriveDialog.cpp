#include "NetworkDriveDialog.h"
#include "ui_Dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDriveDialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}
