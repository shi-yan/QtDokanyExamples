#include "FileSystemThread.h"
#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("Qt Dokany Example");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("This is an example of using Qt to implement a dokany driver"
                                     "It tries to implement a mirror drive similar to the one included in the dokany project."
                                     "You need to use -d to specify a local folder, and -l to specify the drive letter."
                                     "Example: mirror.exe -d c:\\test -l m");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption directoryOption(QStringList() << "d" << "directory",
                                             "The directory you want to mount as a dokany drive.",
                                             "directory");
    parser.addOption(directoryOption);

    QCommandLineOption letterOption(QStringList() << "l" << "letter",
                                             "The letter of the dokany drive.",
                                             "letter");
    parser.addOption(letterOption);

    parser.process(app);

    if(!parser.isSet(directoryOption) || !parser.isSet(letterOption))
    {
        if (!parser.isSet("directory"))
        {
            qDebug() << "Error: Need to provide a directory to mount as the dokany drive.";
        }
        if (!parser.isSet("letter"))
        {
            qDebug() << "Error: Need to provide a letter to mount the dokany drive.";
        }
    }
    else
    {
        const QString dirLocation = parser.value(directoryOption);

        QDir dir(dirLocation);

        if (dir.exists())
        {
            const QString driveLetter = parser.value(letterOption);
            qDebug() << "Directory" << dirLocation <<"will be mounted as the dokan drive" << driveLetter;
            FileSystemThread *fsThread = new FileSystemThread(dir.absolutePath(), driveLetter);
            fsThread->start();
            return app.exec();
        }
        else
        {
            qDebug() << "Error: Folder" << dirLocation << "does not exist.";
        }
    }
    qDebug() << "Use the -h option for help.";
    return -1;
}
