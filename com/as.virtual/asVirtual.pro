#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T10:44:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsVitrual
TEMPLATE = app

##@Linux Flags
DEFINES += __LINUX__
LIBS += -L/lib64 -l:libdl.so.2

##@Windows Flags
#DEFINES += __WINDOWS__

DEFINES += USE_OPENCAN_DIRECTLY DYNAMIC_XLDRIVER_DLL




SOURCES +=              \
    source/main.cpp \
    source/vEcu.cpp \
    source/Virtio.cpp \
    source/asdebug.cpp \
    source/vCan.cpp \
    source/arcan.cpp \
    source/ardevice.cpp \
    source/entry.cpp \
    source/ocdevice.cpp \
    source/ocdevicemanager.cpp \
    source/ocmessage.cpp \
    source/xlLoadlib.cpp

HEADERS  +=             \
    include/vEcu.h \
    include/Virtio.h \
    include/asdebug.h \
    include/vCan.h \
    include/arcan.h \
    include/ardevice.h \
    include/entry.h \
    include/occore.h \
    include/ocdevice.h \
    include/ocdeviceinterface.h \
    include/ocdevicemanager.h \
    include/ocmessage.h \
    include/ocplugininfo.h \
    include/vxlapi.h

FORMS    +=

INCLUDEPATH += include

RESOURCES += 

LIBS +=

OTHER_FILES += 

