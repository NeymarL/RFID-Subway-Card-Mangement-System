#include "db_manager.h"

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    this->error = "NO ERROR";
    if (this->createDB(DATABASE_NAME)) {
        if (!this->tableExist(TABLE_USERS)) {
            User_model user_model;
            user_model.create_table();
        }

        if (!this->tableExist(TABLE_STATION)) {
            Station_model station_model;
            station_model.create_table();
            this->init_route(station_model);
        }


        if (!this->tableExist(TABLE_CONSUME)) {
            Consume_model consume_model;
            consume_model.create_table();
        }

        if (!this->tableExist(TABLE_COST_INFO)) {
            Costinfo_model costinfo_model;
            costinfo_model.create_table();
            this->init_costinfo(costinfo_model);
        }

        if (!this->tableExist(TABLE_RECHARGE)) {
            Recharge_model recharge_model;
            recharge_model.create_table();
        }
    }
}
/**
 * @brief RecordTableModel::createDB
 * @param dbName 数据库名称
 * 用于创建数据库
 */
bool DBManager::createDB(const QString &dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (db.open()) {
        qDebug() << dbName << " create success";
        return true;
    }
    else {
        qDebug() << dbName << " create failed!";
        return false;
    }
}
/**
 * @brief DBManager::tableExist
 * @param tableName 表名
 * @return  如果存在返回true，否则返回false
 * 用于判断表是否存在
 */
bool DBManager::tableExist(const QString &tableName)
{
    int count = 0 ;
    QString sqlText = QObject::tr("select count(*) from sqlite_master where type='table' and name='%1'").arg(tableName);
    QSqlQuery query;
    query.exec(sqlText);
    if (query.next()) {
        count = query.value(0).toInt();
    }
    if (count > 0) {
        return true;
    }
    else {
        return false;
    }
}
/**
 * @brief DBManager::getTableNames
 * @return  数据库表集合
 * 获取数据库所有表的表名
 */
QStringList DBManager::getTableNames()
{
    QStringList tables;
    QString sqlText = QObject::tr("select name from sqlite_master where type='table' order by name;");
    QSqlQuery query;
    query.exec(sqlText);
    while (query.next())
    {
        tables << query.value(0).toString();
    }
    return tables;
}
/**
 * @brief DBManager::dbClose
 * 关闭数据库
 */
void DBManager::dbClose()
{
    if (db.isOpen()) {
        db.close();
    }
}

void DBManager::init_route(Station_model& station_model)
{
    QString route = "2";
    station_model.bind_table();
    station_model.add_station(route, 1, "光谷广场");
    station_model.add_station(route, 2, "杨家湾");
    station_model.add_station(route, 3, "虎泉");
    station_model.add_station(route, 4, "广埠屯");
    station_model.add_station(route, 5, "街道口");
    station_model.add_station(route, 6, "宝通寺");
    station_model.add_station(route, 7, "中南路");
    station_model.add_station(route, 8, "洪山广场");
    station_model.add_station(route, 9, "螃蟹岬");
    station_model.add_station(route, 10, "积玉桥");
    station_model.add_station(route, 11, "江汉路");
    station_model.add_station(route, 12, "天河机场");
}


void DBManager::init_costinfo(Costinfo_model& costinfo_model)
{
    if (costinfo_model.rowCount() > 10) {
        return;
    }
    QString route = "2";
    costinfo_model.bind_table();
    costinfo_model.add_costinfo(route, 0, 0);
    costinfo_model.add_costinfo(route, 1, 2);
    costinfo_model.add_costinfo(route, 2, 2);
    costinfo_model.add_costinfo(route, 3, 2);
    costinfo_model.add_costinfo(route, 4, 3);
    costinfo_model.add_costinfo(route, 5, 3);
    costinfo_model.add_costinfo(route, 6, 3);
    costinfo_model.add_costinfo(route, 7, 4);
    costinfo_model.add_costinfo(route, 8, 4);
    costinfo_model.add_costinfo(route, 9, 4);
    costinfo_model.add_costinfo(route, 10, 5);
    costinfo_model.add_costinfo(route, 11, 6);
    costinfo_model.add_costinfo(route, 12, 7);
    costinfo_model.add_costinfo(route, 13, 8);
}

