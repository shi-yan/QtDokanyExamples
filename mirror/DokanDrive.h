#ifndef DOKANDRIVE_H
#define DOKANDRIVE_H

#include <QObject>

class DokanDrive : public QObject
{
public:
    DokanDrive();
    ~DokanDrive();

public:
    static void init();

private:
    static DokanDrive *m_dokanDrive;
};

#endif // DOKANDRIVE_H
