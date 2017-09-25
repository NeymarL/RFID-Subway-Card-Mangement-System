#ifndef USER_MODEL_H
#define USER_MODEL_H

#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlField>
#include <QString>
#include <QDebug>

class User_model : public QSqlTableModel
{
    Q_OBJECT
private:
    QString table_name; //表名

    QStringList header;//表头

public:

    explicit User_model(QObject *parent = 0);

    ~User_model();

    bool create_table();

    QSqlRecord find_record_by_id(const QString& tagID);

    void bind_table();

    bool user_exists(const QString& tagID);

    bool delete_user(const QString& tagID);

    bool add_user(QString& tagID, QString& phone, QString& name, QString& money);

    bool lost_card(QString& tagID, QString& state);

    bool change_money(QString& tagID, QString& money);

    QString error;

signals:

public slots:
};

#endif // USER_MODEL_H
