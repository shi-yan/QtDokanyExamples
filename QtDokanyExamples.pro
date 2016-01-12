TEMPLATE = subdirs

SUBDIRS = Mirror \
          NetworkDriveServer \
          NetworkDriveClient

NetworkDriveClient.depends = NetworkDriveServer