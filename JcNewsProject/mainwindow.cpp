#include "mainwindow.h"
#include "newscomponent.h"
#include "configuration.h"
#include "./ui_mainwindow.h"

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
