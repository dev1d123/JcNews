#include "fakeresults.h"
#include "ui_fakeresults.h"

FakeResults::FakeResults(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FakeResults)
{
    ui->setupUi(this);
}

FakeResults::~FakeResults()
{
    delete ui;
}
