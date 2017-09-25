#ifndef COMSUME_MODEL_H
#define COMSUME_MODEL_H

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QTime>
#include <QDate>

class Consume_model : public QSqlTableModel
{
    Q_OBJECT
private:
    QString table_name; //表名

    QStringList header;//表头

public:

    explicit Consume_model(QObject *parent = 0);

    bool create_table();

    void bind_record_by_id(const QString& tagID);

    void bind_table();

    bool enter_station(QString& tagID, QString& phone,
                       QString& name, QString& station);

    bool exit_station(QString& tagID, QString& station, int cost);

    void bind_simulate();

    bool has_entered(QString& tagID);

    bool delete_record(QString& tagID);

    QString last_station(QString& tagID);

    QString error;

signals:

public slots:
};

#endif // COMSUME_MODEL_H
