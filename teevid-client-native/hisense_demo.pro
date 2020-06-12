#-------------------------------------------------
#
# Project created by QtCreator 2020-04-30T12:28:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hisense_demo
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
    ConnectParamsDialog.cpp \
        main.cpp \
        mainwindow.cpp \
    InitialScreen.cpp \
    FriendItem.cpp \
    Contact.cpp \
    CallItemVideoView.cpp \
    ServerSimulationDialog.cpp \
    VideoQualityDialog.cpp

HEADERS += \
    CallItemVideoView.h \
    ConnectParamsDialog.h \
        mainwindow.h \
    InitialScreen.h \
    FriendItem.h \
    Contact.h \
    IVideoProvider.h \
    ServerSimulationDialog.h \
    VideoQualityDialog.h

FORMS += \
    CallItemVideoView.ui \
    ConnectParamsDialog.ui \
        mainwindow.ui \
    InitialScreen.ui \
    FriendItem.ui \
    ServerSimulationDialog.ui \
    VideoQualityDialog.ui

RESOURCES += \
    resourses.qrc

INCLUDEPATH += ../desktop-client-sdk/include \

CONFIG(debug, debug|release){
    LIBS += -L"$$PWD/../desktop-client-sdk/cmake-build-debug" -lteevid_sdk
} else {
    LIBS += -L"$$PWD/../desktop-client-sdk/cmake-build-release" -lteevid_sdk
}
