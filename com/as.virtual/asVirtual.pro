#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T10:44:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsVitrual
TEMPLATE = app

DEFINES += __LINUX__

LIBS += -L/lib64 -l:libdl.so.2


SOURCES +=              \
    source/main.cpp \
    source/vEcu.cpp \
    source/vmWindow.cpp

HEADERS  +=             \
    include/vEcu.h \
    include/vmWindow.h \
    include/Std_Types.h

FORMS    +=

INCLUDEPATH += include

RESOURCES += 

LIBS +=

OTHER_FILES += 

