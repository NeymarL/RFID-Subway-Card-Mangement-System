#include "cost_table.h"
#include "ui_cost_table.h"

cost_table::cost_table(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cost_table)
{
    ui->setupUi(this);
    costinfo = new Costinfo_model();
    costinfo->bind_table();
    ui->tableView->setModel(costinfo);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

cost_table::~cost_table()
{
    delete costinfo;
    delete ui;
}
