QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/database.cpp \
    src/db_additem.cpp \
    src/main.cpp \
    src/msg_send_recv.cpp \
    src/widget.cpp

HEADERS += \
    head/database.h \
    head/db_additem.h \
    head/pythreadstatelock.h \
    head/widget.h

FORMS += \
    ui/db_additem.ui \
    ui/widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    testpython.py \
    版本更新 \
    项目技术点 \
    项目知识点 \
    项目说明
INCLUDEPATH += C:\Users\86130\AppData\Local\Programs\Python\Python311\include
LIBS += -LC:\Users\86130\AppData\Local\Programs\Python\Python311\libs -lpython311
