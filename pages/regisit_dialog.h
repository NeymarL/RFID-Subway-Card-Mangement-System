#ifndef REGISIT_DIALOG_H
#define REGISIT_DIALOG_H

#include <QDialog>
#include <QString>
#include <QMessageBox>
#include "models/Users_model.h"
#include "serial_thread/serial_thread.h"
#include "inc/m1356dll.h"

namespace Ui {
class regisit_dialog;
}

class regisit_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit regisit_dialog(QWidget *parent = 0, SerialPortThread *serial = 0);
    ~regisit_dialog();

    void authentication();

private slots:
    void on_regist_btn_clicked();

    void on_concern_btn_clicked();

    void on_tagIdReceived(QString tagId);

    void on_serialMsgreceived(QByteArray bytes);//串口消息接收槽

    void onOperationError();
    void on_read_btn_clicked();

signals:
    void regsendCardId(QString tagID);

    void findFailed();

private:
    Ui::regisit_dialog *ui;

    User_model* user_model;

    M1356Dll *m1356dll;

    QString tagID;

    SerialPortThread *serialThread;

    int currentOps; //记录当前的操作

    int init_value;

    QString phone_num;
};

#endif // REGISIT_DIALOG_H
