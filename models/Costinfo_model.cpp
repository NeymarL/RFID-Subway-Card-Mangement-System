#include "Costinfo_model.h"
#include "database.h"

Costinfo_model::Costinfo_model(QObject *parent) : QSqlTableModel(parent)
{
    this->table_name = TABLE_COST_INFO;
    this->header << QObject::trUtf8("地铁线路编号") <<
                 QObject::trUtf8("地铁区间站数") <<
                 QObject::trUtf8("价格");
    this->error = "NO ERROR";
}

/**
 * 创建表
 */
bool Costinfo_model::create_table()
{
    QSqlQuery query;
    QString str;
    str  = tr("create table ") + this->table_name + tr(" ( ");
    str += header.at(0) + tr(" varchar not null, ");
    str += header.at(1) + tr(" varchar not null, ");
    str += header.at(2) + tr(" varchar not null) ");
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
 * 通过距离查找资费
 * @param  dist [description]
 * @return      [description]
 */
int Costinfo_model::find_cost_by_dist(const int dist)
{
    for (int row = 0; row < rowCount(); row++) {
        if (data(index(row, 1)).toInt() == dist) {
            return data(index(row, 2)).toInt();
        }
    }
    return 0;
}

/**
 * 绑定表
 */
void Costinfo_model::bind_table()
{
    setTable(this->table_name);
    select();
}



/**
 * 添加用户
 * @param  tagID 卡号
 * @param  phone 手机号
 * @param  name  姓名
 * @param  money 余额
 * @return       [description]
 */
bool Costinfo_model::add_costinfo(QString& route, int distance, int cost)
{
    QSqlRecord record;
    record.append(QSqlField(header.at(0), QVariant::String));
    record.append(QSqlField(header.at(1), QVariant::String));
    record.append(QSqlField(header.at(2), QVariant::String));
    record.setValue(0, QVariant(route));
    record.setValue(1, QVariant(QString::number(distance)));
    record.setValue(2, QVariant(QString::number(cost)));
    bool result = insertRecord(-1, record);
    if (!result) {
        error = lastError().text();
    }
    return result;
}

