#-------------------------------------------------
#
# Project created by QtCreator 2016-12-21T11:20:22
#
#-------------------------------------------------

QT       += core gui serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Subway_System
TEMPLATE = app
INCLUDEPATH += .


# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    lib/m1356dll.cpp \
    tools/tools.cpp \
    serial_thread/serial_thread.cpp \
    pages/mainpage.cpp \
    pages/simulatepage.cpp \
    models/Users_model.cpp \
    pages/user_table.cpp \
    models/db_manager.cpp \
    pages/regisit_dialog.cpp \
    models/Station_model.cpp \
    models/Consume_record_model.cpp \
    pages/run_record_dialog.cpp \
    models/Costinfo_model.cpp \
    pages/cost_table.cpp \
    models/Recharge_model.cpp \
    pages/recharge_page.cpp \
    pages/card_dialog.cpp

HEADERS  += mainwindow.h \
    inc/m1356dll.h \
    inc/m1356dll_global.h \
    tools/tools.h \
    serial_thread/serial_thread.h \
    pages/mainpage.h \
    pages/simulatepage.h \
    models/Users_model.h \
    models/database.h \
    pages/user_table.h \
    models/db_manager.h \
    pages/regisit_dialog.h \
    models/Station_model.h \
    models/Consume_record_model.h \
    pages/run_record_dialog.h \
    models/Costinfo_model.h \
    pages/cost_table.h \
    models/Recharge_model.h \
    pages/recharge_page.h \
    pages/card_dialog.h \
    pages/simu_reacord.h

FORMS    += mainwindow.ui \
    mainwindow_copy.ui \
    pages/mainpage.ui \
    pages/simulatepage.ui \
    pages/user_table.ui \
    pages/regisit_dialog.ui \
    pages/run_record_dialog.ui \
    pages/cost_table.ui \
    pages/recharge_page.ui \
    pages/card_dialog.ui

RESOURCES += \
    sub.qrc

