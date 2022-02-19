#-------------------------------------------------
#
# Project created by QtCreator 2019-11-25T16:53:34
#
#-------------------------------------------------

QT       += core gui serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Production_test_interface_V2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    editcmdwindow.cpp \
    jsonhandle.cpp \
    mythread.cpp \
    qcustomplot.cpp

HEADERS += \
        mainwindow.h \
    editcmdwindow.h \
    jsonhandle.h \
    main.h \
    mythread.h \
    qcustomplot.h \
    fftw-3.3.5-dll32/fftw3.h

FORMS +=

DISTFILES += \
    config.json \
    speaker_mic_test_server.py

win32: LIBS += -LC:/Users/wb-sy649554/.conda/envs/alibaba_32bit/libs/ -lpython27

INCLUDEPATH += C:/Users/wb-sy649554/.conda/envs/alibaba_32bit/include
DEPENDPATH += C:/Users/wb-sy649554/.conda/envs/alibaba_32bit/include

win32: LIBS += -L$$PWD/fftw-3.3.5-dll32/ -llibfftw3-3

INCLUDEPATH += $$PWD/fftw-3.3.5-dll32
DEPENDPATH += $$PWD/fftw-3.3.5-dll32
