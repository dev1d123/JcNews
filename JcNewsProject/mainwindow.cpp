#include "mainwindow.h"
#include "newscomponent.h"
#include "configuration.h"
#include "./ui_mainwindow.h"
#include "preferences.h"
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings settings("JcNews", "config");

    QString tema = settings.value("tema", "claro").toString();
    QString fuente = settings.value("fuente", "Fuente 1").toString();

    QString rutaTema = (tema == "oscuro")
        ? ":/styles/oscuro.qss"
        : ":/styles/claro.qss";

    QString rutaFuente;

    if (fuente == "Fuente 1") {
        rutaFuente = ":/styles/font1.qss";
    } else if (fuente == "Fuente 2") {
        rutaFuente = ":/styles/font2.qss";
    } else {
        rutaFuente = ":/styles/font3.qss";
    }

    // Unir estilos
    QString estiloFinal;

    // Leer estilo del tema
    QFile fileTema(rutaTema);
    if (fileTema.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += QString::fromUtf8(fileTema.readAll());
        fileTema.close();
    } else {
        qDebug() << "No se pudo cargar el archivo QSS del tema:" << rutaTema;
    }

    // Leer estilo de la fuente
    QFile fileFuente(rutaFuente);
    if (fileFuente.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += "\n" + QString::fromUtf8(fileFuente.readAll());
        fileFuente.close();
    } else {
        qDebug() << "No se pudo cargar el archivo QSS de la fuente:" << rutaFuente;
    }

    // Aplicar estilo combinado
    qApp->setStyleSheet(estiloFinal);

    /*
     * CONEXION CON CONFIGURACION
     */
    connect(ui->configuracionButton, &QPushButton::clicked, this, [this]() {
        configuration* dialog = new configuration(this); // El padre es el componente
        dialog->exec(); // O usa show() si no quieres bloquear
    });


    QWidget* contenedor = ui->noticiasScroll->widget();  // esto asume que se llama contenidoNoticias

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenedor->layout());
    if (!layout) {
        layout = new QVBoxLayout(contenedor);
        contenedor->setLayout(layout);
    }

    for (int i = 0; i < 10; ++i) {
        NewsComponent* comp = new NewsComponent(this);
        comp->setFixedHeight(150);  // Altura de cada noticia
        layout->addWidget(comp);
    }

    layout->addStretch();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actualizarButton_clicked()
{
    QString scriptPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../apiScraping/main.py");
    QMessageBox::information(this, "Ejecución", "Ejecutando script:\n" + scriptPath);

    QProcess* process = new QProcess(this);

    // Ejecutar con Python
    process->start("python", QStringList() << scriptPath);

    connect(process, &QProcess::readyReadStandardOutput, [process, this]() {
        QMessageBox::information(this, "Salida estándar", process->readAllStandardOutput());
    });

    connect(process, &QProcess::readyReadStandardError, [process, this]() {
        QMessageBox::information(this, "Error", process->readAllStandardError());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [process, this](int exitCode, QProcess::ExitStatus exitStatus) {
                QMessageBox::information(this, "Finalizado",
                                         "Código de salida: " + QString::number(exitCode) +
                                             "\nEstado: " + (exitStatus == QProcess::NormalExit ? "Normal" : "Crash"));
                process->deleteLater();
            });
}
