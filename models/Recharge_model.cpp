#include "Recharge_model.h"
#include "database.h"

Recharge_model::Recharge_model(QObject *parent) : QSqlTableModel(parent)
{
    this->table_name = TABLE_RECHARGE;
    this->header << QObject::trUtf8("卡号") <<
                 QObject::trUtf8("充值金额") <<
                 QObject::trUtf8("充值时间") <<
                 QObject::trUtf8("充值地点");
    this->error = "NO ERROR";
}

/**
 * 创建表
 */
bool Recharge_model::create_table()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + this->table_name + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");
    str += header.at(1) + tr(" varchar not null, ");
    str += header.at(2) + tr(" varchar not null, ");
    str += header.at(3) + tr(" varchar not null) ");
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
void Recharge_model::bind_record_by_id(const QString& tagID)
{
    setTable(this->table_name);
    setFilter(tr("卡号 = '%1'").arg(tagID));
    select();
}

/**
 * 绑定表
 */
void Recharge_model::bind_table()
{
    setTable(this->table_name);
    select();
}

/**
 * 根据卡号删除记录
 * @param  tagID 卡号
 * @return       删除结果
 */
bool Recharge_model::delete_record(const QString& tagID)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            removeRow(row);
        }
    }
    return submitAll();
}

/**
 * 新增充值记录
 * @param  tagID    卡号
 * @param  money    充值金额
 * @param  position 充值地点
 * @return          结果
 */
bool Recharge_model::add_record(QString& tagID, int money, QString& position)
{
    QSqlRecord record;
    QString time = CurrentDateTime();
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.append(QSqlField(header.at(3), QVariant::String));
    record.setValue(0, QVariant(tagID));
    record.setValue(1, QVariant(QString::number(money)));
    record.setValue(2, QVariant(time));
    record.setValue(3, QVariant(position));
    bool result = insertRecord(-1, record);
    if (!result) {
        error = lastError().text();
    }
    return result;
}

