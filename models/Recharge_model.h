#ifndef RECHARGE_MODEL_H
#define RECHARGE_MODEL_H

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QString>
#include <QDebug>
#include "tools/tools.h"

class Recharge_model : public QSqlTableModel
{
    Q_OBJECT
private:
    QString table_name; //表名

    QStringList header;//表头

public:

    explicit Recharge_model(QObject *parent = 0);

    bool create_table();

    void bind_record_by_id(const QString& tagID);

    void bind_table();

    bool delete_record(const QString& tagID);

    bool add_record(QString& tagID, int money, QString& position);

    QString error;

signals:

public slots:
};

#endif // RECHARGE_MODEL_H
