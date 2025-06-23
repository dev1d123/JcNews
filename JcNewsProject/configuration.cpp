#include "configuration.h"
#include "preferences.h"
#include "statistics.h"

#include "ui_configuration.h"

configuration::configuration(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::configuration)
{
    ui->setupUi(this);
    setWindowTitle("Configuración");
    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

    /*
     * CONEXION CON PREFERENCIAS
     */
    connect(ui->funcionamientoButton, &QPushButton::clicked, this, [this]() {
        Preferences dialog(this);
        dialog.exec();
    });


    /*
     * CONEXION CON ESTADISTICAS
     */
    connect(ui->estadisticasButton, &QPushButton::clicked, this, [this]() {
        Statistics dialog(this);
        dialog.exec();
    });
}

configuration::~configuration()
{
    delete ui;
}
