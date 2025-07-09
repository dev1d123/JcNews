#include "newsvisualizer.h"
#include "fakeresults.h"
#include "ui_newsvisualizer.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
NewsVisualizer::NewsVisualizer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewsVisualizer)
    , networkManager(new QNetworkAccessManager(this))  // Inicializar
{
    ui->setupUi(this);
    setWindowTitle("Noticia");


    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);


    connect(networkManager, &QNetworkAccessManager::finished, this, &NewsVisualizer::imagenDescargada);
}

NewsVisualizer::~NewsVisualizer()
{
    delete ui;
}

void NewsVisualizer::setIdUrl(const QString& id) {
    idUrl = id;
}

void NewsVisualizer::setTextPath(const QString& path) {
    textPath = path;

    QFile file(textPath);
    QString contenidoArchivo;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        contenidoArchivo = in.readAll();
        file.close();
    } else {
        contenidoArchivo = "No se pudo abrir el archivo:\n" + textPath;
    }

    ui->contenidoText->setPlainText(contenidoArchivo);
}


void NewsVisualizer::setImgUrl(const QString& url) {
    imgUrl = url;

    // Descargar imagen desde internet
    QUrl imageUrl(imgUrl);
    if (imageUrl.isValid()) {
        QNetworkRequest request(imageUrl);
        networkManager->get(request);
    } else {
        ui->imagenLabel->setText("URL no válida");
    }
}

void NewsVisualizer::imagenDescargada(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap imagen;
        if (imagen.loadFromData(data)) {
            QPixmap scaled = imagen.scaled(ui->imagenLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->imagenLabel->setPixmap(scaled);
        } else {
            ui->imagenLabel->setText("No se pudo cargar la imagen");
        }
    } else {
        ui->imagenLabel->setText("Error al descargar imagen");
    }

    reply->deleteLater();
}


void NewsVisualizer::setFecha(const QDateTime& fecha)
{
    ui->fechaEdit->setDateTime(fecha);
}

void NewsVisualizer::setFuente(const QString& fuente)
{
    ui->fuenteLine->setText(fuente);
}

void NewsVisualizer::setTema(const QString& tema)
{
    ui->temaLine->setText(tema);
}

void NewsVisualizer::setTitulo(const QString& titulo)
{
    ui->tituloLine->setText(titulo);
}





