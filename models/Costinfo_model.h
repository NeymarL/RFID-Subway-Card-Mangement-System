#ifndef COST_MODEL_H
#define COST_MODEL_H

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QString>
#include <QDebug>

class Costinfo_model : public QSqlTableModel
{
    Q_OBJECT
private:
    QString table_name; //表名

    QStringList header;//表头

public:

    explicit Costinfo_model(QObject *parent = 0);

    bool create_table();

    int find_cost_by_dist(const int dist);

    void bind_table();

    bool add_costinfo(QString& route, int distance, int cost);

    QString error;

signals:

public slots:
};

#endif // COST_MODEL_H
