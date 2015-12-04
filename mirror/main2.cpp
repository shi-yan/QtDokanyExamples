#include "LocalDrive.h"
#include "DokanDrive.h"

int main(int argc, char *argv[])
{
    LocalDrive *localDrive = new LocalDrive();

    DokanMirrorDriveInterface::mount(localDrive);

}
