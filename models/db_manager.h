#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include "database.h"
#include "Users_model.h"
#include "Station_model.h"
#include "Consume_record_model.h"
#include "Costinfo_model.h"
#include "Recharge_model.h"

class DBManager : public QObject
{
    Q_OBJECT
public:
    QString error;

    explicit DBManager(QObject *parent = 0);

    void dbClose(); //关闭数据库

    QStringList getTableNames();//获取所有表的表名

    void init_route(Station_model& model);

    void init_costinfo(Costinfo_model& costinfo_model);

private:
    QSqlDatabase db;//sqlite database

    //创建数据库
    bool createDB(const QString &dbName);

    //检测该表是否存在
    bool tableExist(const QString &tableName);

signals:

public slots:
};

#endif // DBMANAGER_H
