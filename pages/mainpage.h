#ifndef MAINPAGE_H
#define MAINPAGE_H
#include "models/db_manager.h"
#include "simulatepage.h"
#include "user_table.h"
#include "regisit_dialog.h"
#include "run_record_dialog.h"
#include "cost_table.h"
#include "recharge_page.h"
#include <QWidget>
#include "serial_thread/serial_thread.h"
#include "inc/m1356dll.h"
#include "models/Users_model.h"
#include "card_dialog.h"

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0, SerialPortThread *serial = 0);
    ~MainPage();

private slots:
    void on_simu_btn_clicked();

    void on_user_btn_clicked();

    void on_regist_btn_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_4_clicked();

    void on_concern_btn_clicked();

    void on_tagIdReceived(QString tagId);

    void onOperationError(QString msg);

    void onOperationError();

    void on_buy_record_btn_clicked();

    void on_lost_btn_clicked();

    void on_find_btn_clicked();

    void on_unregisit_btn_clicked();


    void on_card_btn_clicked();

signals:
    void sig_error(QString error);

private:
    Ui::MainPage *ui;

    DBManager* dbManager;

    SerialPortThread *serialThread;

    M1356Dll *m1356dll;

    QString tagID;

    User_model* user_model;

    QString phone_num;
};

#endif // MAINPAGE_H
