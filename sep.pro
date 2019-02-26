TEMPLATE = app
TARGET = sep
QT += core gui
CONFIG += c++14 release
DESTDIR = target
OBJECTS_DIR = obj
MOC_DIR = moc

HEADERS += \
    mainwindow.h \
    fonteditor.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    fonteditor.cpp
