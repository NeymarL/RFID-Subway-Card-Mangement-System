#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QCloseEvent>
#include <QSerialPort>
#include <tools/tools.h>
#include "inc/m1356dll.h"
#include "pages/pages.h"
#include "serial_thread/serial_thread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void addWidgets(); //加载页面

private slots:
    void on_btn_connect_clicked();//连接

    void on_btn_enter_clicked();//断开连接

    void on_btn_refresh_clicked();//刷新

    void checkCustomBaudRatePolicy(int idx);//波特率下拉框改变时调用

    void fillPortsParameters(QComboBox *box);

    void on_serialMsgreceived(QByteArray bytes);

    void onOperationError(QString msg); //串口发生错误时接收错误信息槽

    void onSendMessage(char *data, int frameLen);//串口消息发送槽

    void handleError(QSerialPort::SerialPortError error);//串口错误槽函数

public slots:
    void on_error(QString error);

signals:
    void sendCardId(QString tagID);

    void findFailed();

private:
    Ui::MainWindow *ui;

    SerialPortThread *serialPortThread; //自定义串口处理线程

    QSerialPort *serialPort;//串口实例

    QIntValidator *intValidator;

    M1356Dll *m1356dll; //13.56M的库对象

    QString tagID;//卡号
};

#endif // MAINWINDOW_H
