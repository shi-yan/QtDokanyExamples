#include "NetworkThread.h"
#include "NetworkDriveClient.h"
#include <QCoreApplication>
#include <QDebug>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("Qt Dokany Network Drive Server Example");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("This is an example of using Qt to implement a dokany driver. \n\n"
                                     "Example for local test:          NetworkDriveServer.exe -t -d c:\\test -l m \n"
                                     "Example for running as a server: NetworkDriveServer.exe");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption testOption(QStringList() << "t" << "test",
                                             "Enable testing mode.");

    parser.addOption(testOption);

    QCommandLineOption directoryOption(QStringList() << "d" << "directory",
                                             "The directory you want to mount as a dokany drive.",
                                             "directory");
    parser.addOption(directoryOption);

    QCommandLineOption letterOption(QStringList() << "l" << "letter",
                                             "The letter of the dokany drive.",
                                             "letter");
    parser.addOption(letterOption);

    parser.process(app);

    if (parser.isSet(testOption))
    {
        qDebug() << "Running local test";
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

                NetworkThread *netThread = new NetworkThread();
                netThread->start();

                while(!netThread->getNetworkDriveServer());

                NetworkDriveClient *networkDriveClient = new NetworkDriveClient(dirLocation, driveLetter);
                networkDriveClient->connectToServer();
                return app.exec();
            }
            else
            {
                qDebug() << "Error: Folder" << dirLocation << "does not exist.";
            }
        }
    }

    return app.exec();
}
