/********************************************************************************
** Form generated from reading UI file 'Dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_NetworkDriveDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *serverLabel;
    QLineEdit *serverLineEdit;
    QPushButton *connectPushButton;
    QLabel *driveLetterLabel;
    QComboBox *driveLetterComboBox;
    QPlainTextEdit *logPlainTextEdit;

    void setupUi(QDialog *NetworkDriveDialog)
    {
        if (NetworkDriveDialog->objectName().isEmpty())
            NetworkDriveDialog->setObjectName(QStringLiteral("NetworkDriveDialog"));
        NetworkDriveDialog->resize(400, 300);
        gridLayout = new QGridLayout(NetworkDriveDialog);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        serverLabel = new QLabel(NetworkDriveDialog);
        serverLabel->setObjectName(QStringLiteral("serverLabel"));

        gridLayout->addWidget(serverLabel, 1, 0, 1, 1);

        serverLineEdit = new QLineEdit(NetworkDriveDialog);
        serverLineEdit->setObjectName(QStringLiteral("serverLineEdit"));

        gridLayout->addWidget(serverLineEdit, 1, 1, 1, 1);

        connectPushButton = new QPushButton(NetworkDriveDialog);
        connectPushButton->setObjectName(QStringLiteral("connectPushButton"));

        gridLayout->addWidget(connectPushButton, 2, 0, 1, 4);

        driveLetterLabel = new QLabel(NetworkDriveDialog);
        driveLetterLabel->setObjectName(QStringLiteral("driveLetterLabel"));

        gridLayout->addWidget(driveLetterLabel, 1, 2, 1, 1);

        driveLetterComboBox = new QComboBox(NetworkDriveDialog);
        driveLetterComboBox->setObjectName(QStringLiteral("driveLetterComboBox"));

        gridLayout->addWidget(driveLetterComboBox, 1, 3, 1, 1);

        logPlainTextEdit = new QPlainTextEdit(NetworkDriveDialog);
        logPlainTextEdit->setObjectName(QStringLiteral("logPlainTextEdit"));

        gridLayout->addWidget(logPlainTextEdit, 0, 0, 1, 4);


        retranslateUi(NetworkDriveDialog);

        driveLetterComboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(NetworkDriveDialog);
    } // setupUi

    void retranslateUi(QDialog *NetworkDriveDialog)
    {
        NetworkDriveDialog->setWindowTitle(QApplication::translate("NetworkDriveDialog", "Qt Dokany Network Drive Example", 0));
        serverLabel->setText(QApplication::translate("NetworkDriveDialog", "Server:", 0));
        serverLineEdit->setText(QApplication::translate("NetworkDriveDialog", "127.0.0.1", 0));
        connectPushButton->setText(QApplication::translate("NetworkDriveDialog", "Connect", 0));
        driveLetterLabel->setText(QApplication::translate("NetworkDriveDialog", "Drive Letter:", 0));
        driveLetterComboBox->clear();
        driveLetterComboBox->insertItems(0, QStringList()
         << QApplication::translate("NetworkDriveDialog", "C", 0)
         << QApplication::translate("NetworkDriveDialog", "D", 0)
         << QApplication::translate("NetworkDriveDialog", "E", 0)
         << QApplication::translate("NetworkDriveDialog", "F", 0)
         << QApplication::translate("NetworkDriveDialog", "G", 0)
         << QApplication::translate("NetworkDriveDialog", "H", 0)
         << QApplication::translate("NetworkDriveDialog", "I", 0)
         << QApplication::translate("NetworkDriveDialog", "J", 0)
         << QApplication::translate("NetworkDriveDialog", "K", 0)
         << QApplication::translate("NetworkDriveDialog", "L", 0)
         << QApplication::translate("NetworkDriveDialog", "M", 0)
         << QApplication::translate("NetworkDriveDialog", "N", 0)
         << QApplication::translate("NetworkDriveDialog", "O", 0)
         << QApplication::translate("NetworkDriveDialog", "P", 0)
         << QApplication::translate("NetworkDriveDialog", "Q", 0)
         << QApplication::translate("NetworkDriveDialog", "R", 0)
         << QApplication::translate("NetworkDriveDialog", "S", 0)
         << QApplication::translate("NetworkDriveDialog", "T", 0)
         << QApplication::translate("NetworkDriveDialog", "U", 0)
         << QApplication::translate("NetworkDriveDialog", "V", 0)
         << QApplication::translate("NetworkDriveDialog", "W", 0)
         << QApplication::translate("NetworkDriveDialog", "X", 0)
         << QApplication::translate("NetworkDriveDialog", "Y", 0)
         << QApplication::translate("NetworkDriveDialog", "Z", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class NetworkDriveDialog: public Ui_NetworkDriveDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
