#-------------------------------------------------
#
# Project created by QtCreator 2020-04-30T12:28:10
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = teevid-client-native
TEMPLATE = app

CONFIG += c++14

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
    AudioFrameData.cpp \
    ConnectParamsDialog.cpp \
    VideoFrameData.cpp \
    main.cpp \
    mainwindow.cpp \
    InitialScreen.cpp \
    FriendItem.cpp \
    Contact.cpp \
    CallItemVideoView.cpp \
    ServerSimulationDialog.cpp \
    VideoQualityDialog.cpp \
    InvitationManager.cpp \
    AudioParams.cpp \
    DeviceVideoManager.cpp \
    DeviceAudioManager.cpp \
    ExternalVideoContainer.cpp

HEADERS += \
    AudioFrameData.h \
    CallItemVideoView.h \
    ConnectParamsDialog.h \
    VideoFrameData.h \
    mainwindow.h \
    InitialScreen.h \
    FriendItem.h \
    Contact.h \
    IVideoProvider.h \
    ServerSimulationDialog.h \
    VideoQualityDialog.h \
    InvitationManager.h \
    AudioParams.h \
    DeviceVideoManager.h \
    DeviceAudioManager.h \
    ExternalVideoContainer.h

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

INCLUDEPATH += $$PWD/include \

UNAME = $$system(uname -p)

INCLUDEPATH += ../desktop-client-sdk/include \
               /usr/include/gstreamer-1.0 \
               /usr/include/glib-2.0 \

contains( UNAME, x86_64 ):{
    INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
}

contains( UNAME, aarch64 ):{
    INCLUDEPATH += /usr/lib/aarch64-linux-gnu/glib-2.0/include
}

LIBS += -lgstreamer-1.0 \
        -lgstapp-1.0 \
        -lgobject-2.0 \
        -lglib-2.0

contains( UNAME, x86_64 ):{
    CONFIG(debug, debug|release){
        LIBS += -L"$$PWD/libs/desktop/debug" -lteevid_sdk
    } else {
        LIBS += -L"$$PWD/libs/desktop/release" -lteevid_sdk
    }
}

contains( UNAME, aarch64 ):{
    CONFIG(debug, debug|release){
        LIBS += -L"$$PWD/libs/jetson/debug" -lteevid_sdk
    } else {
        LIBS += -L"$$PWD/libs/jetson/release" -lteevid_sdk
    }
}
