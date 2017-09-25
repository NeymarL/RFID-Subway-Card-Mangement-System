#ifndef SIMULATEPAGE_H
#define SIMULATEPAGE_H

#include <QThread>
#include <QDialog>
#include "models/Consume_record_model.h"
#include "models/Users_model.h"
#include "models/Costinfo_model.h"
#include "models/Station_model.h"
#include "inc/m1356dll.h"
#include "serial_thread/serial_thread.h"
#include <QTimer>
#include "tools/tools.h"
#include "simu_reacord.h"

namespace Ui {
class SimulatePage;
}

class SimulatePage : public QDialog
{
    Q_OBJECT

public:
    explicit SimulatePage(QWidget *parent = 0, SerialPortThread *serial = 0);
    ~SimulatePage();

    void handle_next_station(int station);

    void on_write_read_data(QByteArray bytes);

private slots:
    void on_radioButton_12_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_8_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_radioButton_7_clicked();

    void on_radioButton_9_clicked();

    void on_radioButton_10_clicked();

    void on_radioButton_11_clicked();

    void on_radioButton_1_clicked();

    void on_serialMsgreceived(QByteArray bytes);//串口消息接收槽

    void on_tagIdReceived(QString tagId);

    void onFinding();

    void stopTimer();//停止定时器

    void authentication();

    void on_read_btn_clicked();

signals:
    void sendCardId(QString tagID);

private:
    Ui::SimulatePage *ui;

    int current_station;

    Consume_model* consume_model;

    User_model* user_model;

    Station_model* station_model;

    Costinfo_model* costinfo_model;

    SerialPortThread *serialThread;

    M1356Dll *m1356dll;

    QTimer *timer; //定时器

    int interval;

    QString tagID;

    int currentOps;

    int value;

    QString phone_num;

    Simu_record record[5];

    Simu_record temp;

    quint8 rec_num;
};

#endif // SIMULATEPAGE_H
