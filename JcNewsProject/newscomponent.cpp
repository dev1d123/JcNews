#include "newscomponent.h"
#include "newsvisualizer.h"
#include "ui_newscomponent.h"

NewsComponent::NewsComponent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewsComponent)
{
    ui->setupUi(this);

    /*
     * CONEXION CON NEWSVISUALIZER
     */
    connect(ui->newsButton, &QPushButton::clicked, this, [this]() {
        NewsVisualizer dialog(this);
        dialog.exec();
    });
}

NewsComponent::~NewsComponent()
{
    delete ui;
}

void NewsComponent::setDate(const QDateTime& date) {
    ui->dateEdit->setDateTime(date);
}

void NewsComponent::setFuente(const QString& fuente) {
    ui->fuenteEdit->setText(fuente);
}

void NewsComponent::setTema(const QString& tema) {
    ui->temaEdit->setText(tema);
}

void NewsComponent::setTitle(const QString& titulo) {
    ui->titleEdit->setText(titulo);
}
