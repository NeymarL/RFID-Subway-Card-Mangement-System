#include "run_record_dialog.h"
#include "ui_run_record_dialog.h"
#include <QMessageBox>

run_record_dialog::run_record_dialog(QWidget *parent, QString* tagID) :
    QDialog(parent),
    ui(new Ui::run_record_dialog)
{
    ui->setupUi(this);
    consume_model = new Consume_model();
    if (tagID == 0) {
        consume_model->bind_table();
    }
    else {
        consume_model->bind_record_by_id(*tagID);
    }
    ui->tableView->setModel(consume_model);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

}


run_record_dialog::~run_record_dialog()
{
    delete consume_model;
    delete ui;
}
