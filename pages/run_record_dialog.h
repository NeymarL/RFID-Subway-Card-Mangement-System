#ifndef RUN_RECORD_DIALOG_H
#define RUN_RECORD_DIALOG_H

#include <QDialog>
#include <models/Consume_record_model.h>

namespace Ui {
class run_record_dialog;
}

class run_record_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit run_record_dialog(QWidget *parent = 0, QString* tagID = 0);
    ~run_record_dialog();

private:
    Ui::run_record_dialog *ui;

    Consume_model* consume_model;
};

#endif // RUN_RECORD_DIALOG_H
