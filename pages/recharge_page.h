#ifndef RECHARGE_PAGE_H
#define RECHARGE_PAGE_H

#include <QDialog>
#include "models/Recharge_model.h"
#include "models/Users_model.h"
#include "serial_thread/serial_thread.h"
#include "inc/m1356dll.h"

namespace Ui {
class recharge_page;
}

class recharge_page : public QDialog
{
    Q_OBJECT

public:
    explicit recharge_page(QWidget *parent = 0, QString* tagID = 0, QString* phone = 0,
                           SerialPortThread *serial = 0);
    ~recharge_page();

    void authentication();

private slots:
    void on_recharge_btn_clicked();

    void on_serialMsgreceived(QByteArray bytes);//串口消息接收槽

    void on_read_btn_clicked();

private:
    Ui::recharge_page *ui;

    Recharge_model* recharge_model;

    User_model* user_model;

    QString tagID;

    M1356Dll *m1356dll;

    SerialPortThread *serialThread;

    QString phone_num;

    int currentOps; //记录当前的操作

    int value;
};

#endif // RECHARGE_PAGE_H
