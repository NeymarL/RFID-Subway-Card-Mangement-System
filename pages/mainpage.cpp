#include "mainpage.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_mainpage.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QMessageBox>
#include "models/Consume_record_model.h"
#include "models/Recharge_model.h"

MainPage::MainPage(QWidget *parent, SerialPortThread *serial) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    dbManager = new DBManager();
    this->serialThread = serial;
    this->m1356dll = new M1356Dll();
    this->user_model = new User_model();
    user_model->bind_table();
    this->tagID = "";
}

MainPage::~MainPage()
{
    delete user_model;
    dbManager->dbClose();
    delete dbManager;
    delete m1356dll;
    delete ui;
}

/**
 * 模拟出行
 */
void MainPage::on_simu_btn_clicked()
{
    SimulatePage simu_page(this, this->serialThread);
    simu_page.exec();
}

/**
 * 查看用户
 */
void MainPage::on_user_btn_clicked()
{
    User_table user_table;
    user_table.exec();
}

/**
 * 注册按钮
 */
void MainPage::on_regist_btn_clicked()
{
    regisit_dialog reg_dialog(this, this->serialThread);
    reg_dialog.exec();
}

/**
 * 出行记录
 */
void MainPage::on_pushButton_7_clicked()
{
    run_record_dialog rr_dialog;
    rr_dialog.exec();
}

/**
 * 消费明细
 */
void MainPage::on_pushButton_6_clicked()
{
    cost_table costt;
    costt.exec();
}

/**
 * 充值
 */
void MainPage::on_pushButton_4_clicked()
{
    recharge_page re_page(this, &(this->tagID), &(this->phone_num), this->serialThread);
    re_page.exec();
}

/**
 * 识别按钮点击事件
 */
void MainPage::on_concern_btn_clicked()
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
 * @brief RegistorWidget::on_tagIdReceived
 * @param tagID 标签ID(卡号)
 * 当读取到卡号时调用该方法
 */
void MainPage::on_tagIdReceived(QString tagID)
{
    ui->id_line->setText(tagID);
    ui->concern_btn->setEnabled(true);
    this->tagID = tagID;
    user_model->bind_table();
    if (user_model->user_exists(tagID)) {
        ui->reg_line->setText("是");
        QSqlRecord user = user_model->find_record_by_id(tagID);
        ui->phone_line->setText(user.value("手机号").toString());
        this->phone_num = user.value("手机号").toString();
        ui->name_line->setText(user.value("姓名").toString());
        ui->active_line->setText(user.value("是否激活").toString());
        ui->money_line->setText(user.value("卡内余额").toString());
        ui->buy_record_btn->setEnabled(true);
        ui->unregisit_btn->setEnabled(true);
        if (user.value("是否激活").toString() == "是") {
            ui->lost_btn->setEnabled(true);
            ui->find_btn->setEnabled(false);
            ui->pushButton_4->setEnabled(true);
        }
        else {
            ui->find_btn->setEnabled(true);
            ui->lost_btn->setEnabled(false);
            ui->pushButton_4->setEnabled(false);
        }
    }
    else {
        ui->reg_line->setText("否");
        ui->phone_line->setText("");
        ui->name_line->setText("");
        ui->active_line->setText("");
        ui->money_line->setText("");
        ui->pushButton_4->setEnabled(false);
        ui->buy_record_btn->setEnabled(false);
        ui->lost_btn->setEnabled(false);
        ui->unregisit_btn->setEnabled(false);
        ui->find_btn->setEnabled(false);
    }
}

void MainPage::onOperationError(QString msg)
{
    ui->concern_btn->setEnabled(true);
    msg = "";
}

void MainPage::onOperationError()
{
    ui->concern_btn->setEnabled(true);
}

void MainPage::on_buy_record_btn_clicked()
{
    run_record_dialog rr_dialog(this, &this->tagID);
    rr_dialog.exec();
}

/**
 * 挂失按钮
 */
void MainPage::on_lost_btn_clicked()
{
    if (user_model->user_exists(this->tagID)) {
        QString state = "否";
        if (user_model->lost_card(this->tagID, state)) {
            ui->lost_btn->setEnabled(false);
            ui->find_btn->setEnabled(true);
            ui->active_line->setText("否");
            ui->pushButton_4->setEnabled(false);
            QMessageBox::warning(this, tr("温馨提示"), tr("挂失成功"), QMessageBox::Yes);
        }
        else {
            QMessageBox::warning(this, tr("温馨提示"), tr("挂失失败"), QMessageBox::Yes);
        }
    }
    else {
        QMessageBox::warning(this, tr("温馨提示"), tr("该卡未注册"), QMessageBox::Yes);
    }
}

/**
 * 解挂按钮
 */
void MainPage::on_find_btn_clicked()
{
    if (user_model->user_exists(this->tagID)) {
        QString state = "是";
        if (user_model->lost_card(this->tagID, state)) {
            ui->lost_btn->setEnabled(true);
            ui->find_btn->setEnabled(false);
            ui->active_line->setText("是");
            QMessageBox::warning(this, tr("温馨提示"), tr("解挂成功"), QMessageBox::Yes);
            ui->pushButton_4->setEnabled(true);
        }
        else {
            QMessageBox::warning(this, tr("温馨提示"), tr("解挂失败"), QMessageBox::Yes);
        }
    }
    else {
        QMessageBox::warning(this, tr("温馨提示"), tr("该卡未注册"), QMessageBox::Yes);
    }
}

void MainPage::on_unregisit_btn_clicked()
{
    if (user_model->user_exists(this->tagID)) {
        QSqlRecord user = user_model->find_record_by_id(this->tagID);
        QString money = user.value("卡内余额").toString();
        if (user_model->delete_user(this->tagID)) {
            // 删除其他记录
            Consume_model con;
            con.delete_record(this->tagID);
            Recharge_model rec;
            rec.delete_record(this->tagID);
            QMessageBox::warning(this, tr("温馨提示"), tr("注销成功，退款") + money + tr("元"), QMessageBox::Yes);
            ui->reg_line->setText("否");
            ui->phone_line->setText("");
            ui->name_line->setText("");
            ui->active_line->setText("");
            ui->money_line->setText("");
            ui->pushButton_4->setEnabled(false);
            ui->buy_record_btn->setEnabled(false);
            ui->lost_btn->setEnabled(false);
            ui->unregisit_btn->setEnabled(false);
            ui->find_btn->setEnabled(false);
        }
        else {
            QMessageBox::warning(this, tr("温馨提示"), tr("注销失败"), QMessageBox::Yes);
        }
    }
    else {
        QMessageBox::warning(this, tr("温馨提示"), tr("该卡未注册"), QMessageBox::Yes);
    }
}


void MainPage::on_card_btn_clicked()
{
    Card_dialog card_dialog(this, this->serialThread);
    card_dialog.exec();
}
