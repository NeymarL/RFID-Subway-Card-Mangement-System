#include "Station_model.h"
#include "database.h"

Station_model::Station_model(QObject *parent) : QSqlTableModel(parent)
{
    this->table_name = TABLE_STATION;
    this->header << QObject::trUtf8("地铁线路编号") <<
                 QObject::trUtf8("站名1") <<
                 QObject::trUtf8("站名2") <<
                 QObject::trUtf8("站名3") <<
                 QObject::trUtf8("站名4") <<
                 QObject::trUtf8("站名5") <<
                 QObject::trUtf8("站名6") <<
                 QObject::trUtf8("站名7") <<
                 QObject::trUtf8("站名8") <<
                 QObject::trUtf8("站名9") <<
                 QObject::trUtf8("站名10") <<
                 QObject::trUtf8("站名11") <<
                 QObject::trUtf8("站名12");
    this->error = "NO ERROR";
}

/**
 * 创建表
 */
bool Station_model::create_table()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + this->table_name + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");
    str += header.at(1) + tr(" varchar, ");
    str += header.at(2) + tr(" varchar, ");
    str += header.at(3) + tr(" varchar, ");
    str += header.at(4) + tr(" varchar, ");
    str += header.at(5) + tr(" varchar, ");
    str += header.at(6) + tr(" varchar, ");
    str += header.at(7) + tr(" varchar, ");
    str += header.at(8) + tr(" varchar, ");
    str += header.at(9) + tr(" varchar, ");
    str += header.at(10) + tr(" varchar, ");
    str += header.at(11) + tr(" varchar, ");
    str += header.at(12) + tr(" varchar) ");
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

int Station_model::get_station_num_from_name(QString& name)
{
    for (int row = 0; row < rowCount(); row++) {
        for (int col = 1; col <= 12; col++) {
            if (data(index(row, col)).toString() == name) {
                return col;
            }
        }
    }
    return 0;
}

QString Station_model::get_station_name_from_num(int num)
{
    int row = 0;
    return data(index(row, num)).toString();
}

/**
 * 绑定表
 */
void Station_model::bind_table()
{
    setTable(this->table_name);
    select();
}

/**
 * 判断线路是否存在
 * @param  tagID 卡号
 * @return       第几行
 */
int Station_model::route_exists(const QString& route)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 0)).toString() == route) {
            return row;
        }
    }
    return -1;
}

/**
 * 新增地铁站
 * @param  route        第几号线
 * @param  sta_num      第几站，从1开始
 * @param  station_name 站名
 * @return              成功或失败
 */
bool Station_model::add_station(QString& route, int sta_num, const char* name)
{
    int row = this->route_exists(route);
    QString station_name(name);
    bool result;
    if (row == -1) {
        QSqlRecord record;
        record.append(QSqlField(header.at(0), QVariant::String));
        record.append(QSqlField(header.at(1), QVariant::String));
        record.append(QSqlField(header.at(2), QVariant::String));
        record.append(QSqlField(header.at(3), QVariant::String));
        record.append(QSqlField(header.at(4), QVariant::String));
        record.append(QSqlField(header.at(5), QVariant::String));
        record.append(QSqlField(header.at(6), QVariant::String));
        record.append(QSqlField(header.at(7), QVariant::String));
        record.append(QSqlField(header.at(8), QVariant::String));
        record.append(QSqlField(header.at(9), QVariant::String));
        record.append(QSqlField(header.at(10), QVariant::String));
        record.append(QSqlField(header.at(11), QVariant::String));
        record.append(QSqlField(header.at(12), QVariant::String));
        record.setValue(0, QVariant(route));
        record.setValue(sta_num, QVariant(station_name));
        result = insertRecord(-1, record);
        if (!result) {
            QString sql;
            QSqlQuery query;
            sql = tr("INSERT INTO station (地铁线路编号, 站名1) VALUES (%1, %2)").arg(
                      route, station_name);
            query.prepare(sql);
            result = query.exec();
            if (!result) {
                error = query.lastError().text();
                // error = sql;
            }
        }
    }
    else {
        setData(index(row, sta_num), QVariant(station_name));
        result = submitAll();
    }
    return result;
}

