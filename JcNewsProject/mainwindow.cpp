#include "mainwindow.h"
#include "newscomponent.h"
#include "configuration.h"
#include "./ui_mainwindow.h"

#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
