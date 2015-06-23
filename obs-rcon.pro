TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/rcon-main.c \
    src/rcon-server.c \
    src/rcon-handlers.c \
    src/mongoose.c


unix:!macx: LIBS += -L/home/admalledd/bin/obs/bin/64bit/ -lobs

INCLUDEPATH += /home/admalledd/bin/obs/include
DEPENDPATH += /home/admalledd/bin/obs/include

OTHER_FILES += \
    Makefile

HEADERS += \
    src/rcon-main.h \
    src/rcon-server.h \
    src/rcon-handlers.h \
    src/mongoose.h
