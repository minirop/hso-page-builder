QT += core gui widgets

QMAKE_CXXFLAGS += -std=c++2a

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    appsettings.cpp \
    charactereditor.cpp \
    eventslist.cpp \
    eventslistfiltermodel.cpp \
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
    tabbedimages.cpp \
    text.cpp \
    utils.cpp

HEADERS += \
    appsettings.h \
    charactereditor.h \
    eventslist.h \
    eventslistfiltermodel.h \
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
    tabbedimages.h \
    text.h \
    utils.h

FORMS += \
    charactereditor.ui \
    mainwindow.ui \
    pagesettings.ui \
    tabbedimages.ui
