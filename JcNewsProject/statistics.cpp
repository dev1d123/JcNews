#include "statistics.h"
#include "ui_statistics.h"

Statistics::Statistics(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Statistics)
{
    ui->setupUi(this);
    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

}

Statistics::~Statistics()
{
    delete ui;
}
