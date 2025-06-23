#include "fakeresults.h"
#include "ui_fakeresults.h"

FakeResults::FakeResults(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FakeResults)
{
    ui->setupUi(this);
    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

}

FakeResults::~FakeResults()
{
    delete ui;
}
