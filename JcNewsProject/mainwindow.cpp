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
    QString pythonPath = "C:/Users/USUARIO/Documents/apiScraping/env/Scripts/python.exe";
    QString scriptPath = QDir::currentPath() + "/apiScraping/main.py";

    QProcess *process = new QProcess(this);
    process->setProgram(pythonPath);
    process->setArguments(QStringList() << scriptPath);
    process->setWorkingDirectory(QDir::currentPath() + "/apiScraping");

    connect(process, &QProcess::readyReadStandardOutput, [process]() {
        QString output = process->readAllStandardOutput();
        qDebug() << "Salida del script:" << output;
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [](int exitCode, QProcess::ExitStatus status) {
                if (status == QProcess::NormalExit && exitCode == 0) {
                    QMessageBox::information(nullptr, "Éxito", "Script ejecutado correctamente.");
                } else {
                    QMessageBox::warning(nullptr, "Error", "Falló la ejecución del script de Python.");
                }
            });

    process->start();
}


