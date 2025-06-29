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
        dialog.setIdUrl(this->idUrl);
        dialog.setTextPath(this->textPath);
        dialog.setImgUrl(this->imgUrl);

        dialog.setFecha(ui->dateEdit->dateTime());
        dialog.setFuente(ui->fuenteEdit->text());             // QLineEdit está bien con .text()
        dialog.setTema(ui->temaEdit->text());                 // QLineEdit está bien
        dialog.setTitulo(ui->titleEdit->toPlainText());       // QTextEdit usa toPlainText()

        dialog.exec();

    });

}

NewsComponent::~NewsComponent()
{
    delete ui;
}
void NewsComponent::setIdUrl(const QString& id) {
    this->idUrl = id;
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

void NewsComponent::setTextPath(const QString& path) {
    this->textPath = path;
}

void NewsComponent::setImgUrl(const QString& url) {
    this->imgUrl = url;
}


