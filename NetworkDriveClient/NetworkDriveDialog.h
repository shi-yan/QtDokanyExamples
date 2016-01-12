#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "../NetworkDriveServer/NetworkDriveClient.h"

namespace Ui {
class NetworkDriveDialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    NetworkDriveClient *m_networkClient;

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::NetworkDriveDialog *ui;

private slots:
    void onConnect();
};

#endif // DIALOG_H
