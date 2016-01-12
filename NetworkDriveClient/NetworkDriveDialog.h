#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class NetworkDriveDialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::NetworkDriveDialog *ui;
};

#endif // DIALOG_H
