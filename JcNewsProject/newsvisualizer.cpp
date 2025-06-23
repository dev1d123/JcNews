#include "newsvisualizer.h"
#include "ui_newsvisualizer.h"

NewsVisualizer::NewsVisualizer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewsVisualizer)
{
    ui->setupUi(this);
    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

}

NewsVisualizer::~NewsVisualizer()
{
    delete ui;
}
