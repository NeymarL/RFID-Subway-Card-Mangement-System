#ifndef STATION_MODEL_H
#define STATION_MODEL_H

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QString>
#include <QDebug>

class Station_model : public QSqlTableModel
{
    Q_OBJECT
private:
    QString table_name; //表名

    QStringList header;//表头

public:

    explicit Station_model(QObject *parent = 0);

    bool create_table();

    void bind_table();

    bool add_station(QString& route, int sta_num, const char* name);

    int route_exists(const QString& route);

    int get_station_num_from_name(QString& name);

    QString get_station_name_from_num(int num);

    QString error;

signals:

public slots:
};

#endif // STATION_MODEL_H
