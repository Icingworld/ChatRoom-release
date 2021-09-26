QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += network

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    copyright.cpp

HEADERS += \
    login.h \
    mainwindow.h \
    copyright.h

FORMS += \
    login.ui \
    mainwindow.ui \
    copyright.ui

TRANSLATIONS += \
    ChatRoom_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Resources/clear.png \
    Resources/pic.png \
    Resources/save.png \
    Resources/login.png

RESOURCES += \
    Resources/Resources.qrc
