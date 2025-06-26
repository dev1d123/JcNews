#include "preferences.h"
#include "ui_preferences.h"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "mainwindow.h"
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

    QWidget *w = QApplication::activeWindow();
    while (w->parentWidget())
        w = w->parentWidget();

    MainWindow *main = qobject_cast<MainWindow *>(w);
    if (main) {
        QString tema = settings.value("tema", "claro").toString();
        QString rutaTemaQss = (tema == "oscuro")
            ? ":/styles/oscuro.qss"
            : ":/styles/claro.qss";

        QString fuente = settings.value("fuente", "Sans").toString();
        QString rutaFuenteQss;

        if (fuente == "Fuente 1") {
            rutaFuenteQss = ":/styles/font1.qss";
        } else if (fuente == "Fuente 2") {
            rutaFuenteQss = ":/styles/font2.qss";
        } else {
            rutaFuenteQss = ":/styles/font3.qss"; // fuente por defecto
        }

        QString estiloFinal;

        // Cargar tema
        QFile fileTema(rutaTemaQss);
        if (fileTema.open(QFile::ReadOnly | QFile::Text)) {
            estiloFinal += QString::fromUtf8(fileTema.readAll());
            fileTema.close();
        } else {
            qDebug() << "No se pudo cargar el archivo de tema QSS:" << rutaTemaQss;
        }

        // Cargar fuente
        QFile fileFuente(rutaFuenteQss);
        if (fileFuente.open(QFile::ReadOnly | QFile::Text)) {
            estiloFinal += "\n" + QString::fromUtf8(fileFuente.readAll());
            fileFuente.close();
        } else {
            qDebug() << "No se pudo cargar el archivo de fuente QSS:" << rutaFuenteQss;
        }

        // Aplicar todo el estilo
        qApp->setStyleSheet(estiloFinal);

        // Opcional: Mostrar mensaje
        QMessageBox::information(this, "Ejecución", "La letra usada es: " + fuente);
    }

}

