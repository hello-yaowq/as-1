#-------------------------------------------------
#
# Project created by QtCreator 2014-08-31T10:44:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AsVitrual
TEMPLATE = app

DEFINES = USE_KERNEL3 USE_OPENCAN_DIRECTLY DYNAMIC_XLDRIVER_DLL


SOURCES +=              \
    source/main.cpp \
    source/vEcu.cpp \
    source/vmWindow.cpp

HEADERS  +=             \
    src/Std_Types.h \ 
    include/vEcu.h \
    include/vmWindow.h

FORMS    +=

INCLUDEPATH += include src

RESOURCES += 

LIBS +=

OTHER_FILES += 

