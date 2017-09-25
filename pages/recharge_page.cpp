#include "recharge_page.h"
#include "ui_recharge_page.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QMessageBox>

recharge_page::recharge_page(QWidget *parent, QString* tagID, QString* phone,
                             SerialPortThread *serial) :
    QDialog(parent),
    ui(new Ui::recharge_page)
{
    ui->setupUi(this);
    this->m1356dll = new M1356Dll();
    this->tagID = *tagID;
    this->phone_num = *phone;
    this->serialThread = serial;
    recharge_model = new Recharge_model();
    recharge_model->bind_record_by_id(this->tagID);
    user_model = new User_model();
    user_model->bind_table();
    ui->tableView->setModel(recharge_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    QRegExp phone_rx("[0-9]+");
    QRegExpValidator *validator = new QRegExpValidator(phone_rx, this);
    ui->money_edit->setValidator(validator);
    ui->id_line->setText(this->tagID);
    connect(serialThread, SIGNAL(receivedMsg(QByteArray)), this, SLOT(on_serialMsgreceived(QByteArray)));
}

recharge_page::~recharge_page()
{
    delete recharge_model;
    delete user_model;
    delete m1356dll;
    delete ui;
}

void recharge_page::on_recharge_btn_clicked()
{
    if (!user_model->user_exists(this->tagID)) {
        QMessageBox::warning(this, tr("温馨提示"), tr("用户不存在"), QMessageBox::Yes);
    }
    QSqlRecord user = user_model->find_record_by_id(this->tagID);
    int old = user.value("卡内余额").toInt();
    int add = ui->money_edit->text().toInt();
    int now = old + add;
    this->value = now;
    QString money = QString::number(now);
    QString position = ui->comboBox->currentText();
    if (!user_model->change_money(this->tagID, money)) {
        QMessageBox::warning(this, tr("温馨提示"), tr("充值失败"), QMessageBox::Yes);
        return;
    }
    if (!recharge_model->add_record(tagID, add, position)) {
        QMessageBox::warning(this, tr("温馨提示"), tr("充值失败"), QMessageBox::Yes);
        QString old_money = QString::number(old);
        user_model->change_money(this->tagID, old_money);
        return;
    }
    this->currentOps = 0;
    this->authentication();
    QMessageBox::warning(this, tr("温馨提示"), tr("充值成功"), QMessageBox::Yes);
}

/**
 * @brief recharge_page::on_serialMsgreceived
 * @param bytes 接收到的数据字节数组
 * 串口接收消息后的处理槽函数
 */
void recharge_page::on_serialMsgreceived(QByteArray bytes) {
    M1356_RspFrame_t frame = m1356dll->M1356_RspFrameConstructor(bytes);
    if (frame.cmd.remove(" ") == "0702" && frame.status == "00") //授权成功
    {
        switch (currentOps) {
        case 0: //init
        {
            ui->mem_line->setText("Writing...");
            uint16 frameLen;
            quint8 buffer[16];
            uint8 *p;
            buffer[0] = BLOCK0;
            memset(buffer + 1, 0, 15);
            memcpy(buffer + 1, &this->value, 4);
            QSTRING_TO_HEX(this->phone_num, buffer + 5, 6);
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1WRITE, buffer, 16);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2 ), frameLen);
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
}


/**
 * @brief recharge_page::authentication
 * 授权
 */
void recharge_page::authentication()
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


void recharge_page::on_read_btn_clicked()
{
    this->authentication();
    currentOps = 1;
}

