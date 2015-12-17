#include "LocalDrive.h"
#include "DokanDrive.h"
#include "NetworkDrive.h"

int main(int argc, char *argv[])
{
    //LocalDrive *localDrive = new LocalDrive();

    NetworkDrive *networkDrive = new NetworkDrive();

    DokanMirrorDriveInterface::mount(networkDrive);

}
