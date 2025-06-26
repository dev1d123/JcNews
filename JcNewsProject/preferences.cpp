#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include <QSettings>

Preferences::Preferences(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Preferences)
{
    ui->setupUi(this);
    setWindowTitle("Funcionamiento");

    // Cargar valores previos
    QSettings settings("JcNews", "config");

    QString rutaGuardada = settings.value("dir_news").toString();
    if (!rutaGuardada.isEmpty()) {
        ui->direccionLine->setText(rutaGuardada);
    }

    QString region = settings.value("region").toString();
    int regionIndex = ui->regionBox->findText(region);
    if (regionIndex != -1) {
        ui->regionBox->setCurrentIndex(regionIndex);
    }

    QString fuente = settings.value("fuente").toString();
    int fuenteIndex = ui->fuenteBox->findText(fuente);
    if (fuenteIndex != -1) {
        ui->fuenteBox->setCurrentIndex(fuenteIndex);
    }

    QString tema = settings.value("tema").toString();
    if (tema == "claro") {
        ui->claroButton->setChecked(true);
    } else if (tema == "oscuro") {
        ui->oscuroButton->setChecked(true);
    }

    // Botones
    connect(ui->cambiarButton, &QToolButton::clicked, this, &Preferences::seleccionarCarpeta);
    connect(ui->atrasButton, &QPushButton::clicked, this, [=]() {
        guardarPreferencias();
        accept(); // cerrar ventana
    });
}

Preferences::~Preferences()
{
    delete ui;
}

void Preferences::seleccionarCarpeta()
{
    QString carpeta = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta");

    if (!carpeta.isEmpty()) {
        ui->direccionLine->setText(carpeta);

        QSettings settings("JcNews", "config");
        settings.setValue("dir_news", carpeta);
    }
}

void Preferences::guardarPreferencias()
{
    QSettings settings("JcNews", "config");

    // Guardar carpeta si cambió manualmente
    QString carpetaActual = ui->direccionLine->text();
    if (!carpetaActual.isEmpty()) {
        settings.setValue("dir_news", carpetaActual);
    }

    // Guardar región y fuente
    settings.setValue("region", ui->regionBox->currentText());
    settings.setValue("fuente", ui->fuenteBox->currentText());

    // Guardar tema
    if (ui->claroButton->isChecked()) {
        settings.setValue("tema", "claro");
    } else if (ui->oscuroButton->isChecked()) {
        settings.setValue("tema", "oscuro");
    }
}

