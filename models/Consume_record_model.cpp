#include "Consume_record_model.h"
#include "database.h"
#include "tools/tools.h"

Consume_model::Consume_model(QObject *parent) : QSqlTableModel(parent)
{
    this->table_name = TABLE_CONSUME;
    this->header << QObject::trUtf8("卡号") <<
                 QObject::trUtf8("手机号") <<
                 QObject::trUtf8("姓名") <<
                 QObject::trUtf8("进站") <<
                 QObject::trUtf8("出战") <<
                 QObject::trUtf8("乘车时间") <<
                 QObject::trUtf8("刷卡时间") <<
                 QObject::trUtf8("扣款金额");
    this->error = "NO ERROR";
}

/**
 * 创建表
 */
bool Consume_model::create_table()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + this->table_name + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");
    str += header.at(1) + tr(" varchar not null, ");
    str += header.at(2) + tr(" varchar not null, ");
    str += header.at(3) + tr(" varchar, ");
    str += header.at(4) + tr(" varchar, ");
    str += header.at(5) + tr(" varchar, ");
    str += header.at(6) + tr(" varchar, ");
    str += header.at(7) + tr(" varchar) ");
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
 * 只绑定这张卡的消费记录
 * @param tagID 卡号
 */
void Consume_model::bind_record_by_id(const QString& tagID)
{
    bind_table();
    // setFilter(tr("卡号 like '%1%'").arg(tagID));
    setFilter(tr("卡号 = '%1'").arg(tagID));
    select();
}

/**
 * 绑定表
 */
void Consume_model::bind_table()
{
    setTable(this->table_name);
    select();
}

void Consume_model::bind_simulate()
{
    bind_table();
    QString n = QString::number(rowCount() - 1);
    setFilter(tr("rowid > ") + n);
    select();
}

bool Consume_model::has_entered(QString& tagID)
{
    for (int row = rowCount(); row >= 0; row--) {
        if (data(index(row, 0)).toString() == tagID) {
            if (data(index(row, 4)).toString() == "") {
                return true;
            }
        }
    }
    return false;
}

QString Consume_model::last_station(QString& tagID)
{
    for (int row = rowCount(); row >= 0; row--) {
        if (data(index(row, 0)).toString() == tagID) {
            if (data(index(row, 4)).toString() == "") {
                return data(index(row, 3)).toString();
            }
        }
    }
    return QString();
}

/**
 * 进站
 * @param  tagID   卡号
 * @param  phone   手机号
 * @param  name    姓名
 * @param  station 站名
 * @return         成功或失败
 */
bool Consume_model::enter_station(QString& tagID, QString& phone,
                                  QString& name, QString& station)
{
    QSqlRecord record;
    QString blank = "";
    QString time = CurrentDateTime();
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.append(QSqlField(header.at(3), QVariant::String));
    record.append(QSqlField(header.at(4), QVariant::String));
    record.append(QSqlField(header.at(5), QVariant::String));
    record.append(QSqlField(header.at(6), QVariant::String));
    record.append(QSqlField(header.at(7), QVariant::String));
    record.setValue(0, QVariant(tagID));
    record.setValue(1, QVariant(phone));
    record.setValue(2, QVariant(name));
    record.setValue(3, QVariant(station));
    record.setValue(4, QVariant(blank));
    record.setValue(5, QVariant(blank));
    record.setValue(6, QVariant(time));
    record.setValue(7, QVariant(blank));
    return insertRecord(-1, record);
}

/**
 * 出站
 * @param  tagID   卡号
 * @param  station 站名
 * @param  cost    花费
 * @return         成功或失败
 */
bool Consume_model::exit_station(QString& tagID, QString& station, int cost)
{
    int row = rowCount() - 1;
    bool found = false;
    for (; row >= 0; row--) {
        if (data(index(row, 0)).toString() == tagID) {
            found = true;
            break;
        }
    }
    if (found) {
        QString in_time = data(index(row, 6)).toString();
        QString out_time = CurrentDateTime();
        QDateTime arr = QDateTime::fromString(in_time, "yyyy-MM-dd hh:mm:ss");
        QDateTime lea = QDateTime::fromString(out_time, "yyyy-MM-dd hh:mm:ss");
        uint time_diff = (lea.toTime_t() - arr.toTime_t());
        QString last_time =  QDateTime::fromTime_t(time_diff).toString("mm:ss");
        setData(index(row, 4), QVariant(station));
        setData(index(row, 5), QVariant(last_time));
        setData(index(row, 6), QVariant(out_time));
        setData(index(row, 7), QVariant(QString::number(cost)));
        return submitAll();
    }
    else {
        error = "No Record Found";
        return false;
    }
}

/**
 * 删除卡的消费记录
 * @param  tagID [description]
 * @return       [description]
 */
bool Consume_model::delete_record(QString& tagID)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == tagID) {
            removeRow(row);
        }
    }
    return submitAll();
}


