QT       += core gui multimedia widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# OpenAL для Windows
win32 {
    LIBS += -lopenal
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    audioprocessor.cpp

HEADERS += \
    mainwindow.h \
    audioprocessor.h
