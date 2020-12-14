QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KuGou
TEMPLATE = app

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    titlebar.cpp \
    controlbar.cpp \
    toplistitem.cpp \
    lyricwidget.cpp

HEADERS += \
        mainwindow.h \
    titlebar.h \
    controlbar.h \
    toplistitem.h \
    lyricwidget.h

RESOURCES += \
    res.qrc
