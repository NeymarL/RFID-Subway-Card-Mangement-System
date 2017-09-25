#ifndef COST_TABLE_H
#define COST_TABLE_H

#include <QDialog>
#include "models/Costinfo_model.h"

namespace Ui {
class cost_table;
}

class cost_table : public QDialog
{
    Q_OBJECT

public:
    explicit cost_table(QWidget *parent = 0);
    ~cost_table();

private:
    Ui::cost_table *ui;

    Costinfo_model* costinfo;
};

#endif // COST_TABLE_H
