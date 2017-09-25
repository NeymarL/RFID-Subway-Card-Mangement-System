#include "card_dialog.h"
#include "ui_card_dialog.h"

Card_dialog::Card_dialog(QWidget *parent, SerialPortThread *serial) :
    QDialog(parent),
    ui(new Ui::Card_dialog)
{
    ui->setupUi(this);
    this->serialThread = serial;
    this->m1356dll = new M1356Dll();
    user_model = new User_model(this);
    user_model->bind_table();
    station_model = new Station_model();
    station_model->bind_table();
    connect(serialThread, SIGNAL(receivedMsg(QByteArray)), this, SLOT(on_serialMsgreceived(QByteArray)));
    connect(this, SIGNAL(sendCardId(QString)), this, SLOT(on_tagIdReceived(QString)));
}

Card_dialog::~Card_dialog()
{
    delete user_model;
    delete station_model;
    delete m1356dll;
    delete ui;
}

void Card_dialog::on_concern_btn_clicked()
{
    ui->concern_btn->setEnabled(true);
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
 * @brief Card_dialog::on_tagIdReceived
 * @param tagID 标签ID(卡号)
 * 当读取到卡号时调用该方法
 */
void Card_dialog::on_tagIdReceived(QString tagID)
{
    ui->id_line->setText(tagID);
    ui->concern_btn->setEnabled(true);
    if (!user_model->user_exists(tagID)) {
        QMessageBox::warning(this, "Error", tr("该卡未注册"));
        return;
    }
    this->currentOps = 0;
    this->authentication();
}

/**
 * @brief Card_dialog::authentication
 * 授权
 */
void Card_dialog::authentication()
{
    uint16 frameLen;
    quint8 buffer[8];
    uint8 *p;
    buffer[0] = 0x60;    // A 密钥
    if (currentOps == 0) {
        buffer[1] = BLOCK0;   // 绝对块号
    }
    else {
        buffer[1] = BLOCK1;
    }
    QString str = "FFFFFFFFFFFF"; //六字节
    QSTRING_TO_HEX(str, (uint8*)(buffer + 2), 6);
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_AUTHENTICATION, buffer, 8);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialThread->writeData((char *)(p + 2 ), frameLen);
}

void Card_dialog::on_serialMsgreceived(QByteArray bytes) {
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
            emit sendCardId(tagID);
        }
    }
    if (frame.cmd.remove(" ") == "0702" && frame.status == "00") //授权成功
    {
        uint16 frameLen;
        quint8 buffer[1];
        uint8 *p;
        if (this->currentOps == 0) {
            buffer[0] = BLOCK0;
        }
        else {
            buffer[0] = BLOCK1;
        }
        p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1READ, buffer, 1);
        frameLen = BUILD_UINT16(p[0], p[1]);
        serialThread->writeData((char *)(p + 2 ), frameLen);
    }
    else if (frame.cmd.remove(" ") == "0802")
    {
        QString data = frame.vdata;
        data.remove(" ");
        if (this->currentOps == 0) {
            ui->mem1->setText(frame.vdata);
            // 解析余额和手机号
            quint8 bytes[16];
            QSTRING_TO_HEX(data, bytes, 16);
            int money;
            memcpy(&money, bytes, 4);
            QString phone = data.mid(8, 10) + data.mid(19, 1);
            QString smoney = QString::number(money);
            ui->money_line->setText(smoney);
            ui->phone_line->setText(phone);
            this->currentOps = 1;
            this->authentication();
        }
        else {
            // 解析消费记录
            ui->mem2->setText(frame.vdata);
            quint8 bytes[16];
            QSTRING_TO_HEX(data, bytes, 16);
            int num = (int)bytes[0];
            if (num == 0) {
                return;
            }
            Simu_record record[num];
            quint8 station;
            quint16 qtime;
            for (int i = 0; i < num; i++) {
                memcpy(&(station), bytes + 1 + i * 3, 1);
                memcpy(&(qtime), bytes + 1 + i * 3 + 1, 2);
                (record + i)->station = station;
                (record + i)->time = qtime;
            }
            int in, out;
            QString s_in, s_out;
            QString time;
            // QString base = "12-29 ";
            switch (num) {
            case 5:
            {
                in = (int)record[4].station >> 4;
                out = (int)record[4].station & 0x0F;
                s_in = station_model->get_station_name_from_num(in);
                s_out = station_model->get_station_name_from_num(out);
                time = Time2str(record[4].time);
                ui->in_line_5->setText(s_in);
                ui->out_line_5->setText(s_out);
                ui->time_line_5->setText(time);
            }
            case 4:
            {
                in = (int)record[3].station >> 4;
                out = (int)record[3].station & 0x0F;
                s_in = station_model->get_station_name_from_num(in);
                s_out = station_model->get_station_name_from_num(out);
                time = Time2str(record[3].time);
                ui->in_line_4->setText(s_in);
                ui->out_line_4->setText(s_out);
                ui->time_line_4->setText(time);
            }
            case 3:
            {
                in = (int)record[2].station >> 4;
                out = (int)record[2].station & 0x0F;
                s_in = station_model->get_station_name_from_num(in);
                s_out = station_model->get_station_name_from_num(out);
                time = Time2str(record[2].time);
                ui->in_line_3->setText(s_in);
                ui->out_line_3->setText(s_out);
                ui->time_line_3->setText(time);
            }
            case 2:
            {
                in = (int)record[1].station >> 4;
                out = (int)record[1].station & 0x0F;
                s_in = station_model->get_station_name_from_num(in);
                s_out = station_model->get_station_name_from_num(out);
                time = Time2str(record[1].time);
                ui->in_line_2->setText(s_in);
                ui->out_line_2->setText(s_out);
                ui->time_line_2->setText(time);
            }
            case 1:
            {
                in = (int)record[0].station >> 4;
                out = (int)record[0].station & 0x0F;
                s_in = station_model->get_station_name_from_num(in);
                s_out = station_model->get_station_name_from_num(out);
                time = Time2str(record[0].time);
                ui->in_line_1->setText(s_in);
                ui->out_line_1->setText(s_out);
                ui->time_line_1->setText(time);
            }
            }
        }
    }
}


