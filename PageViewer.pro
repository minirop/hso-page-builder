QT += core gui widgets

CONFIG += c++2a

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    appsettings.cpp \
    eventslist.cpp \
    fontdatabase.cpp \
    gif.cpp \
    gifslider.cpp \
    imageslider.cpp \
    main.cpp \
    mainwindow.cpp \
    modsmanager.cpp \
    page.cpp \
    pageelement.cpp \
    pagesettings.cpp \
    text.cpp \
    utils.cpp

HEADERS += \
    appsettings.h \
    eventslist.h \
    fontdatabase.h \
    gif.h \
    gifslider.h \
    globals.h \
    imageslider.h \
    mainwindow.h \
    modsmanager.h \
    page.h \
    pageelement.h \
    pagesettings.h \
    text.h \
    utils.h

FORMS += \
    mainwindow.ui \
    pagesettings.ui
