#include "newsvisualizer.h"
#include "fakeresults.h"
#include "ui_newsvisualizer.h"

NewsVisualizer::NewsVisualizer(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewsVisualizer)
    , networkManager(new QNetworkAccessManager(this))  // Inicializar
{
    ui->setupUi(this);
    setWindowTitle("Noticia");

    // Mostrar resumen inicial
    actualizarResumen();


    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);

    // Conexión con FakeResults
    connect(ui->fakeButton, &QPushButton::clicked, this, [this]() {
        FakeResults dialog(this);
        dialog.exec();
    });
    connect(networkManager, &QNetworkAccessManager::finished, this, &NewsVisualizer::imagenDescargada);
}

NewsVisualizer::~NewsVisualizer()
{
    delete ui;
}

void NewsVisualizer::setIdUrl(const QString& id) {
    idUrl = id;
    actualizarResumen();
}

void NewsVisualizer::setTextPath(const QString& path) {
    textPath = path;
    actualizarResumen();
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
void NewsVisualizer::setImgUrl(const QString& url) {
    imgUrl = url;
    actualizarResumen();

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


void NewsVisualizer::actualizarResumen()
{
    if (!ui) return;

    QString resumen;
    resumen += "ID URL: " + idUrl + "\n";
    resumen += "Ruta de texto: " + textPath + "\n";
    resumen += "Enlace de imagen: " + imgUrl + "\n";

    ui->contenidoText->setPlainText(resumen);
}

