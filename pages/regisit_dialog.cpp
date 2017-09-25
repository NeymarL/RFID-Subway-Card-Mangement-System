#include "regisit_dialog.h"
#include "ui_regisit_dialog.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>

regisit_dialog::regisit_dialog(QWidget *parent, SerialPortThread *serial) :
    QDialog(parent),
    ui(new Ui::regisit_dialog)
{
    ui->setupUi(this);
    this->serialThread = serial;
    this->m1356dll = new M1356Dll();
    this->tagID = "";
    QRegExp phone_rx("^1[3|4|5|7|8][0-9]\\d{8}$");
    QRegExpValidator *validator = new QRegExpValidator(phone_rx, this);
    ui->phone_edit->setValidator(validator);
    ui->phone_edit->installEventFilter(this);
    user_model = new User_model(this);
    user_model->bind_table();
    this->init_value = 30;
    connect(serialThread, SIGNAL(receivedMsg(QByteArray)), this, SLOT(on_serialMsgreceived(QByteArray)));
    connect(this, SIGNAL(regsendCardId(QString)), this, SLOT(on_tagIdReceived(QString)));
    connect(this, SIGNAL(findFailed()), this, SLOT(onOperationError()));
}

regisit_dialog::~regisit_dialog()
{
    delete user_model;
    delete m1356dll;
    delete ui;
}


void regisit_dialog::on_regist_btn_clicked()
{
    QString tagID = ui->id_line->text();
    if (tagID == "") {
        QMessageBox msgBox;
        msgBox.setText("请点击识别按钮识别卡");
        msgBox.exec();
        return;
    }
    if (user_model->user_exists(tagID)) {
        QMessageBox msgBox;
        msgBox.setText("该卡已注册过，无需重复注册");
        msgBox.exec();
        return;
    }
    QString phone_num = ui->phone_edit->text();
    if (phone_num == "") {
        QMessageBox msgBox;
        msgBox.setText("请填写手机号");
        msgBox.exec();
        return;
    }
    if (phone_num.length() < 11) {
        QMessageBox msgBox;
        msgBox.setText("手机号长度错误");
        msgBox.exec();
        return;
    }
    QString name = ui->name_edit->text();
    if (name == "") {
        QMessageBox msgBox;
        msgBox.setText("请填写姓名");
        msgBox.exec();
        return;
    }
    QString money = "30";
    if (user_model->add_user(tagID, phone_num, name, money)) {
        this->phone_num = phone_num;
        this->authentication();
        this->currentOps = 0;
        QMessageBox msgBox;
        msgBox.setText("注册成功！");
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText(user_model->error);
        msgBox.exec();
        return;
    }

}

void regisit_dialog::on_concern_btn_clicked()
{
    ui->concern_btn->setEnabled(false);
    uint16 frameLen;
    quint8 buffer[1];
    uint8 *p;
    memset(buffer, 0, 1);
    buffer[0] = RC632_14443_ALL;
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_REQUEST_A, buffer, 1);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialThread->writeData((char *)(p + 2 ), frameLen);
}

/**
 * @brief regisit_dialog::on_tagIdReceived
 * @param tagID 标签ID(卡号)
 * 当读取到卡号时调用该方法
 */
void regisit_dialog::on_tagIdReceived(QString tagID)
{
    ui->id_line->setText(tagID);
    ui->concern_btn->setEnabled(true);
    this->tagID = tagID;
    ui->regist_btn->setEnabled(true);
}

/**
 * @brief MainWindow::on_serialMsgreceived
 * @param bytes 接收到的数据字节数组
 * 串口接收消息后的处理槽函数
 */
void regisit_dialog::on_serialMsgreceived(QByteArray bytes) {
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if (frame.status.left(2) == "00")
    {
        if (frame.cmd.remove(" ") == "0102") //寻卡
        {
            uint16 frameLen;
            quint8 buffer[1];
            uint8 *p;
            memset(buffer, 0, 1);
            buffer[0] = 0x04;
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_ANTICOLL, buffer, 1);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2 ), frameLen, true);
        }
        else if (frame.cmd.remove(" ") == "0202") //寻卡结果
        {
            uint16 frameLen;
            quint8 buffer[4];
            uint8 *p;
            memset(buffer, 0, 4);
            QSTRING_TO_HEX(frame.vdata.remove(" "), buffer, 4); // 卡号
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_SELECT, buffer, 4);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2 ), frameLen, true);
            tagID = frame.vdata.remove(" ");
        }
        else if (frame.cmd.remove(" ") == "0302")
        {
            emit regsendCardId(tagID);
        }
    }
    if (frame.cmd.remove(" ") == "0702" && frame.status == "00") //授权成功
    {
        switch (currentOps) {
        case 0: //init
        {
            ui->mem_line->setText("Writing...");
            // 帧长度
            uint16 frameLen;
            // 发送数据缓冲区，大小为16字节
            quint8 buffer[16];
            uint8 *p;
            // 数据第0字节为写入的区块号
            buffer[0] = BLOCK0;
            // 把后面15个字节清零
            memset(buffer + 1, 0, 15);
            // 把初始化金钱(30)写入从第一个字节开始的4个字节
            memcpy(buffer + 1, &this->init_value, 4);
            // 把手机号转换成十六进制，并写入从第5个字节开始的6个字节
            QSTRING_TO_HEX(this->phone_num, buffer + 5, 6);
            // 根据数据构造发送帧
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1WRITE, buffer, 16);
            // 获取帧长度
            frameLen = BUILD_UINT16(p[0], p[1]);
            // 发送数据
            serialThread->writeData((char *)(p + 2 ), frameLen);
            // 重制状态
            currentOps = -1;
        }
        break;
        case 1: //mem
        {
            ui->mem_line->setText("Reading...");
            uint16 frameLen;
            quint8 buffer[1];
            uint8 *p;
            buffer[0] = BLOCK0;
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1READ, buffer, 1);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2 ), frameLen);
            currentOps = -1;
        }
        break;
        default:
            break;
        }
    }
    else if (frame.cmd.remove(" ") == "0802")
    {
        ui->mem_line->setText(frame.vdata);
    }
    else
    {
        emit findFailed();
    }
}

void regisit_dialog::onOperationError()
{
    ui->concern_btn->setEnabled(true);
}

/**
 * @brief regisit_dialog::authentication
 * 授权
 */
void regisit_dialog::authentication()
{
    uint16 frameLen;
    quint8 buffer[8];
    uint8 *p;
    buffer[0] = 0x60;    // A密钥
    buffer[1] = BLOCK0;   // 绝对块号
    QString str = "FFFFFFFFFFFF"; //六字节
    QSTRING_TO_HEX(str, (uint8*)(buffer + 2), 6);
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_AUTHENTICATION, buffer, 8);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialThread->writeData((char *)(p + 2 ), frameLen);
}


void regisit_dialog::on_read_btn_clicked()
{
    this->authentication();
    currentOps = 1;
}



