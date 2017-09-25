#include "simulatepage.h"
#include "ui_simulatepage.h"
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QMessageBox>

SimulatePage::SimulatePage(QWidget *parent, SerialPortThread *serial) :
    QDialog(parent),
    ui(new Ui::SimulatePage)
{
    ui->setupUi(this);
    this->serialThread = serial;
    this->m1356dll = new M1356Dll();
    this->timer = new QTimer(this);
    consume_model = new Consume_model();
    consume_model->bind_simulate();
    user_model = new User_model();
    user_model->bind_table();
    station_model = new Station_model();
    station_model->bind_table();
    costinfo_model = new Costinfo_model();
    costinfo_model->bind_table();
    ui->now_table->setModel(consume_model);
    ui->now_table->resizeColumnsToContents();
    ui->now_table->resizeRowsToContents();
    ui->now_table->horizontalHeader()->setStretchLastSection(true);
    this->current_station = 0;
    this->interval = 1000;
    connect(serialThread, SIGNAL(receivedMsg(QByteArray)), this, SLOT(on_serialMsgreceived(QByteArray)));
    connect(this, SIGNAL(sendCardId(QString)), this, SLOT(on_tagIdReceived(QString)));
    connect(timer, SIGNAL(timeout()), this, SLOT(onFinding()));
}

SimulatePage::~SimulatePage()
{
    delete costinfo_model;
    delete user_model;
    delete station_model;
    delete m1356dll;
    delete consume_model;
    delete ui;
}

void SimulatePage::handle_next_station(int station)
{
    if (this->current_station != 0) {
        switch (station) {
        case 12:
            ui->radioButton_11->setEnabled(false);
        case 11:
            ui->radioButton_10->setEnabled(false);
        case 10:
            ui->radioButton_9->setEnabled(false);
        case 9:
            ui->radioButton_8->setEnabled(false);
        case 8:
            ui->radioButton_7->setEnabled(false);
        case 7:
            ui->radioButton_6->setEnabled(false);
        case 6:
            ui->radioButton_5->setEnabled(false);
        case 5:
            ui->radioButton_4->setEnabled(false);
        case 4:
            ui->radioButton_3->setEnabled(false);
        case 3:
            ui->radioButton_2->setEnabled(false);
        case 2:
            ui->radioButton_1->setEnabled(false);
        }
    }
    this->current_station = station;
    if (this->current_station == 1) {
        if (!timer->isActive()) {
            timer->start(this->interval);
        }
    }
}

void SimulatePage::on_radioButton_12_clicked()
{
    this->handle_next_station(12);
}

void SimulatePage::on_radioButton_2_clicked()
{
    this->handle_next_station(2);
}

void SimulatePage::on_radioButton_clicked()
{
    this->handle_next_station(2);
}

void SimulatePage::on_radioButton_3_clicked()
{
    this->handle_next_station(3);
}

void SimulatePage::on_radioButton_4_clicked()
{
    this->handle_next_station(4);
}

void SimulatePage::on_radioButton_8_clicked()
{
    this->handle_next_station(8);
}

void SimulatePage::on_radioButton_5_clicked()
{
    this->handle_next_station(5);
}

void SimulatePage::on_radioButton_6_clicked()
{
    this->handle_next_station(6);
}

void SimulatePage::on_radioButton_7_clicked()
{
    this->handle_next_station(7);
}

void SimulatePage::on_radioButton_9_clicked()
{
    this->handle_next_station(9);
}

void SimulatePage::on_radioButton_10_clicked()
{
    this->handle_next_station(10);
}

void SimulatePage::on_radioButton_11_clicked()
{
    this->handle_next_station(11);
}

void SimulatePage::on_radioButton_1_clicked()
{
    this->handle_next_station(1);
}

/**
* @brief regisit_dialog::on_tagIdReceived
* @param tagID 标签ID(卡号)
* 当读取到卡号时调用该方法
*/
void SimulatePage::on_tagIdReceived(QString tagID)
{
    if (!user_model->user_exists(tagID)) {
        ui->info_label->setText("该用户 " + tagID + " 未注册！");
        return;
    }
    QString station = station_model->get_station_name_from_num(this->current_station);
    QSqlRecord user = user_model->find_record_by_id(tagID);
    if (user.value("是否激活") == "否") {
        ui->info_label->setText("该用户 " + tagID + " 未激活！");
        return;
    }
    if (consume_model->has_entered(tagID)) {
        QString last = consume_model->last_station(tagID);
        int last_num = station_model->get_station_num_from_name(last);
        int dist = this->current_station - last_num;
        if (dist < 0) {
            dist += 12;
        }
        int cost = costinfo_model->find_cost_by_dist(dist);
        int money = user.value("卡内余额").toInt();
        if (money < cost) {
            ui->info_label->setText("用户 " + tagID + " 余额不足，出站失败，请先充值");
            return;
        }
        QString now = QString::number(money - cost);
        if (consume_model->exit_station(tagID, station, cost)) {
            if (user_model->change_money(tagID, now)) {
                this->value = money - cost;
                this->phone_num = user.value("手机号").toString();
                // this->temp为Simu_record类型的结构体实例
                this->temp.station = 0;
                // 高四位为进站
                this->temp.station = ((this->temp.station) >> 4 | last_num) << 4;
                // 低四位为出战
                this->temp.station |= this->current_station & 0x0F;
                // 写入压缩时间
                this->temp.time = Str2time(CurrentDateTime());
                // QMessageBox::warning(this, tr("温馨提示"), QString::number(this->temp.time, 16), QMessageBox::Yes);
                this->currentOps = 0;
                this->authentication();
                return;
            }
        }
        ui->info_label->setText("用户 " + tagID + " 出站失败");
    }
    else {
        QString name = user.value("姓名").toString();
        QString phone = user.value("手机号").toString();
        if (consume_model->enter_station(tagID, phone, name, station)) {
            ui->info_label->setText("用户 " + tagID + " 进站" + QString::number(current_station));
        }
        else {
            ui->info_label->setText("用户 " + tagID + " 进站失败");
        }
    }
}

/**
* @brief MainWindow::on_serialMsgreceived
* @param bytes 接收到的数据字节数组
* 串口接收消息后的处理槽函数
*/
void SimulatePage::on_serialMsgreceived(QByteArray bytes) {
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
        switch (currentOps) {
        case 0: // 更新卡内钱数
        {
            uint16 frameLen;
            quint8 buffer[17];
            uint8 *p;
            buffer[0] = BLOCK0;
            memset(buffer + 1, 0, 16);
            memcpy(buffer + 1, &this->value, 4);
            QSTRING_TO_HEX(this->phone_num, buffer + 5, 6);
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1WRITE, buffer, 17);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2), frameLen);
        }
        break;
        case 1: // 更新消费记录
        {
            uint16 frameLen;
            quint8 buffer[17];
            uint8 *p;
            buffer[0] = BLOCK1;
            memset(buffer + 1, 0, 16);
            buffer[1] = this->rec_num;
            quint8 station;
            quint16 time;
            for (int i = 0; i < (int)this->rec_num; ++i) {
                station = (this->record + i)->station;
                time = (this->record + i)->time;
                memcpy(buffer + 2 + i * 3, &(station), 1);
                memcpy(buffer + 2 + i * 3 + 1, &(time), 2);
            }
            p = m1356dll->RC632_SendCmdReq(RC632_CMD_M1WRITE, buffer, 17);
            frameLen = BUILD_UINT16(p[0], p[1]);
            serialThread->writeData((char *)(p + 2 ), frameLen);
            ui->info_label->setText("用户 " + tagID + " 出站" + QString::number(this->rec_num));
        }
        break;
        case 2: // 读第一块内存
        {
            uint16 frameLen;
            quint8 buffer[1];
            uint8 *p;
            buffer[0] = BLOCK1;
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
        quint8 hex[16];
        // vdata为读到的原始数据，为字符串类型
        QString vdata = frame.vdata.remove(" ");
        // 把vdata转换为十六进制数组
        StringToHex(vdata, hex);
        // 第0字节为卡内现有记录的个数
        int num = (int)hex[0];
        // 指向原记录数组从哪里开始拷贝
        int i = 0;
        this->rec_num = num;
        // 如果原记录大于5条
        if (num >= 5) {
            // 记录从第一条开始拷贝
            i++;
            num = 5;
            this->rec_num = 4;
        }
        // 临时变量，用来保存解析出的station
        quint8 station;
        // 临时变量，用来保存解析出的time
        quint16 time;
        // 把原有记录拷贝过来，如果原来有5条记录的话，只需拷贝后面的4条
        for (int j = 0; i < num && j < 5; j++, i++) {
            memcpy(&(station), hex + 1 + i * 3, 1);
            memcpy(&(time), hex + 1 + i * 3 + 1, 2);
            // this->record为拷贝目标
            (this->record + j)->station = station;
            (this->record + j)->time = time;
        }
        // temp为新产生的记录，把新记录加到老记录末尾
        this->record[this->rec_num++] = this->temp;
        // 改变状态为写记录
        this->currentOps = 1;
        // 发送验证请求(执行写记录)
        this->authentication();
    }
    else if (frame.cmd.remove(" ") == "0902" && this->currentOps == 0)
    {
        this->currentOps = 2;
        this->authentication();
    }
}


/**
 * @brief SerialPortThread::stopTimer
 * 停止定时器
 */
void SimulatePage::stopTimer()
{
    if (timer->isActive())
    {
        timer->stop();
    }
}

/**
 * 发送寻卡指令
 */
void SimulatePage::onFinding()
{
    uint16 frameLen;
    quint8 buffer[1];
    uint8 *p;
    memset(buffer, 0, 1);
    buffer[0] = RC632_14443_ALL;
    p = m1356dll->RC632_SendCmdReq(RC632_CMD_REQUEST_A, buffer, 1);
    frameLen = BUILD_UINT16(p[0], p[1]);
    serialThread->writeData((char *)(p + 2 ), frameLen);
    if (!timer->isActive()) {
        timer->start(this->interval);
    }
}

/**
 * @brief regisit_dialog::authentication
 * 授权
 */
void SimulatePage::authentication()
{
    uint16 frameLen;
    quint8 buffer[8];
    uint8 *p;
    buffer[0] = 0x60;    // A密钥
    if (this->currentOps == 0) {
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


void SimulatePage::on_read_btn_clicked()
{
    this->authentication();
    currentOps = 1;
}

