#ifndef CARD_DIALOG_H
#define CARD_DIALOG_H

#include <QDialog>
#include <QString>
#include <QMessageBox>
#include "models/Users_model.h"
#include "serial_thread/serial_thread.h"
#include "models/Station_model.h"
#include "simu_reacord.h"
#include "inc/m1356dll.h"

namespace Ui {
class Card_dialog;
}

class Card_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Card_dialog(QWidget *parent = 0, SerialPortThread *serial = 0);
    ~Card_dialog();

    void authentication();

private slots:
    void on_concern_btn_clicked();

    void on_tagIdReceived(QString tagId);

    void on_serialMsgreceived(QByteArray bytes);//串口消息接收槽

signals:
    void sendCardId(QString tagID);

private:
    Ui::Card_dialog *ui;

    User_model* user_model;

    Station_model* station_model;

    M1356Dll *m1356dll;

    SerialPortThread *serialThread;

    int currentOps;

    QString tagID;
};

#endif // CARD_DIALOG_H
