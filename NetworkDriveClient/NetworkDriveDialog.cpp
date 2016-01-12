#include "NetworkDriveDialog.h"
#include "ui_Dialog.h"
#include <QFileDialog>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDriveDialog)
{
    ui->setupUi(this);
    connect(ui->connectPushButton, SIGNAL(released()),this,SLOT(onConnect()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onConnect()
{
    QString directory = QFileDialog::getExistingDirectory();
    QString letter = ui->driveLetterComboBox->currentText();
    QString address = ui->serverLineEdit->text();


    m_networkClient = new NetworkDriveClient(directory, letter);
    m_networkClient->connectToServer(QHostAddress::LocalHost, 12345);
}
