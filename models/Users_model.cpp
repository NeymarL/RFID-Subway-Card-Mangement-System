#include "Users_model.h"
#include "database.h"

User_model::User_model(QObject *parent) : QSqlTableModel(parent)
{
    this->table_name = TABLE_USERS;
    this->header << QObject::trUtf8("卡号") <<
                 QObject::trUtf8("手机号") <<
                 QObject::trUtf8("姓名") <<
                 QObject::trUtf8("卡内余额") <<
                 QObject::trUtf8("是否激活");
    this->error = "NO ERROR";
}

User_model::~User_model()
{
    submitAll();
}

/**
 * 创建表
 */
bool User_model::create_table()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + this->table_name + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");
    str += header.at(1) + tr(" varchar not null, ");
    str += header.at(2) + tr(" varchar not null, ");
    str += header.at(3) + tr(" varchar not null, ");
    str += header.at(4) + tr(" varchar not null) ");
    qDebug() << "Sql: " << str.toUtf8().data();
    bool ret = query.exec(str);
    if (ret == true) {
        qDebug() << this->table_name << QObject::tr(" table create success");
    }
    else {
        qDebug() << this->table_name << QObject::tr(" table create failed");
    }
    return ret;
}

/**
 * 根据卡号找到该用户
 * @param  tagID 卡号
 * @return       相应记录
 */
QSqlRecord User_model::find_record_by_id(const QString& tagID)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            return record(row);
        }
    }
    return QSqlRecord();
}

/**
 * 绑定表
 */
void User_model::bind_table()
{
    setTable(this->table_name);
    select();
}

/**
 * 判断该卡是否注册
 * @param  tagID 卡号
 * @return       true or false
 */
bool User_model::user_exists(const QString& tagID)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            return true;
        }
    }
    return false;
}

/**
 * 根据卡号删除用户
 * @param  tagID 卡号
 * @return       删除结果
 */
bool User_model::delete_user(const QString& tagID)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            removeRow(row);
        }
    }
    return submitAll();
}

/**
 * 添加用户
 * @param  tagID 卡号
 * @param  phone 手机号
 * @param  name  姓名
 * @param  money 余额
 * @return       [description]
 */
bool User_model::add_user(QString& tagID, QString& phone,
                          QString& name, QString& money)
{
    if (this->user_exists(tagID)) {
        return false;
    }
    QSqlRecord record;
    QString state = "是";
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.append(QSqlField(header.at(3), QVariant::String));
    record.append(QSqlField(header.at(4), QVariant::String));
    record.setValue(0, QVariant(tagID));
    record.setValue(1, QVariant(phone));
    record.setValue(2, QVariant(name));
    record.setValue(3, QVariant(money));
    record.setValue(4, QVariant(state));
    bool result = insertRecord(-1, record);
    if (!result) {
        error = lastError().text();
        QString sql;
        QSqlQuery query;
        sql = tr("INSERT INTO users (卡号, 手机号, 姓名, 卡内余额, 是否激活)\
                 VALUES (%1, %2, %3, %4, %5)").arg(tagID, phone, name, money, state);
        query.prepare(sql);
        result = query.exec();
        bind_table();
        if (!result) {
            error = query.lastError().text();
        }
    }
    submitAll();
    return result;
}

bool User_model::lost_card(QString& tagID, QString& state)
{
    int row;
    bool found = false;
    for (row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    setData(index(row, 4), QVariant(state));
    return submitAll();
}

bool User_model::change_money(QString& tagID, QString& money)
{
    int row;
    bool found = false;
    for (row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    setData(index(row, 3), QVariant(money));
    return submitAll();
}
