#ifndef USER_TABLE_H
#define USER_TABLE_H

#include <QDialog>
#include "models/Users_model.h"

namespace Ui {
class User_table;
}

class User_table : public QDialog
{
    Q_OBJECT

public:
    explicit User_table(QWidget *parent = 0);
    ~User_table();

private:
    Ui::User_table *ui;

    User_model* model;
};

#endif // USER_TABLE_H
