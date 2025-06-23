#include "newsvisualizer.h"
#include "fakeresults.h"
#include "ui_newsvisualizer.h"

NewsVisualizer::NewsVisualizer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewsVisualizer)
{
    ui->setupUi(this);
    setWindowTitle("Noticia");

    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);
    /*
     * CONEXION CON FAKERESULTS
     */
    connect(ui->fakeButton, &QPushButton::clicked, this, [this]() {
        FakeResults dialog(this);
        dialog.exec();
    });
}

NewsVisualizer::~NewsVisualizer()
{
    delete ui;
}
