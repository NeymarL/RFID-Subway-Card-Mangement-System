#include "user_table.h"
#include "ui_user_table.h"

User_table::User_table(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::User_table)
{
    ui->setupUi(this);
    model = new User_model(this);
    model->bind_table();
    ui->user_tab_view->setModel(model);
    ui->user_tab_view->resizeColumnsToContents();
    ui->user_tab_view->resizeRowsToContents();
    ui->user_tab_view->horizontalHeader()->setStretchLastSection(true);
}

User_table::~User_table()
{
    delete model;
    delete ui;
}
