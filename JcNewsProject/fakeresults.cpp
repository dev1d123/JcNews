#include "fakeresults.h"
#include "ui_fakeresults.h"

FakeResults::FakeResults(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FakeResults)
{
    ui->setupUi(this);
    setWindowTitle("Resultados Fake");

    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

}

FakeResults::~FakeResults()
{
    delete ui;
}
