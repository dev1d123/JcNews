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
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <iostream>

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

    QString rutaFuente = ":/styles/font1.qss";
    if (fuente == "Fuente 2") rutaFuente = ":/styles/font2.qss";
    else if (fuente == "Fuente 3") rutaFuente = ":/styles/font3.qss";

    QString estiloFinal;

    QFile fileTema(rutaTema);
    if (fileTema.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += QString::fromUtf8(fileTema.readAll());
        fileTema.close();
    }

    QFile fileFuente(rutaFuente);
    if (fileFuente.open(QFile::ReadOnly | QFile::Text)) {
        estiloFinal += "\n" + QString::fromUtf8(fileFuente.readAll());
        fileFuente.close();
    }

    qApp->setStyleSheet(estiloFinal);

    connect(ui->configuracionButton, &QPushButton::clicked, this, [this]() {
        configuration* dialog = new configuration(this);
        dialog->exec();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actualizarButton_clicked()
{
    QString scriptPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../../apiScraping/main.py");
    QSettings settings("JcNews", "config");
    QString rutaGuardada = settings.value("dir_news").toString();

    if (rutaGuardada.isEmpty()) {
        QMessageBox::warning(this, "Ruta no encontrada", "No se ha configurado una ruta válida en 'dir_news'.");
        return;
    }

    QMessageBox::information(this, "Ejecución", "Ejecutando script:\n" + scriptPath + "\nCon destino:\n" + rutaGuardada);

    QProcess* process = new QProcess(this);
    process->start("python", QStringList() << scriptPath << rutaGuardada);

    connect(process, &QProcess::readyReadStandardOutput, [process, this]() {
        QString output = process->readAllStandardOutput();
        if (!output.trimmed().isEmpty()) {
            QMessageBox::information(this, "Salida estándar", output);
        }
    });

    connect(process, &QProcess::readyReadStandardError, [process, this]() {
        QString errors = process->readAllStandardError();
        if (!errors.trimmed().isEmpty()) {
            QMessageBox::critical(this, "Error", errors);
        }
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [process, this, rutaGuardada](int exitCode, QProcess::ExitStatus exitStatus) {
        QMessageBox::information(this, "Finalizado",
                                 "Código de salida: " + QString::number(exitCode) +
                                 "\nEstado: " + (exitStatus == QProcess::NormalExit ? "Normal" : "Crash"));
        process->deleteLater();

        QDateTime now = QDateTime::currentDateTime();
        ui->actualizarLine->setText(now.toString("dd/MM/yyyy hh:mm:ss"));

        QString jsonPath = QDir(rutaGuardada).filePath("metadatos_noticias.json");
        QFile file(jsonPath);
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "No se pudo abrir el archivo JSON: " << jsonPath.toStdString() << std::endl;
            return;
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            std::cerr << "Error al parsear JSON: " << parseError.errorString().toStdString() << std::endl;
            return;
        }

        QWidget* contenedor = ui->noticiasScroll->widget();
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(contenedor->layout());
        if (!layout) {
            layout = new QVBoxLayout(contenedor);
            contenedor->setLayout(layout);
        }

        // Limpiar widgets previos
        QLayoutItem* child;
        while ((child = layout->takeAt(0)) != nullptr) {
            if (QWidget* widget = child->widget()) {
                widget->deleteLater();
            }
            delete child;
        }
        QMap<QString, QString> traduccionesTemas = {
            {"business", "negocios"},
            {"sports", "deportes"},
            {"technology", "tecnología"},
            {"politics", "política"},
            {"health", "salud"},
            {"science", "ciencia"},
            {"entertainment", "entretenimiento"},
            {"world", "mundo"},
            {"economy", "economía"},
            {"education", "educación"},
            {"travel", "viajes"},
            {"food", "comida"},
            {"environment", "medio ambiente"},
            {"top", "principales"},
            {"finance", "finanzas"},
            {"law", "ley"},
            {"culture", "cultura"},
            {"fashion", "moda"},
            {"crime", "crimen"},
            {"weather", "clima"},
            {"real estate", "bienes raíces"}
        };

        QJsonArray noticias = doc.array();
        for (const QJsonValue& value : noticias) {
            QJsonObject obj = value.toObject();
            NewsComponent* comp = new NewsComponent(this);

            comp->setFixedHeight(150);
            comp->setDate(QDateTime::fromString(obj["fecha"].toString(), Qt::ISODate));
            comp->setFuente(obj["fuente"].toString());
            comp->setTitle(obj["titulo"].toString());

            QStringList temasTraducidos;
            QJsonArray temasJsonArray = obj["temas"].toArray();
            for (const QJsonValue& tema : temasJsonArray) {
                QString temaIngles = tema.toString();
                QString temaEsp = traduccionesTemas.value(temaIngles, temaIngles); // si no se encuentra, deja el original
                temasTraducidos << temaEsp;
            }
            QString temasConcatenados = temasTraducidos.join(", ");
            comp->setTema(temasConcatenados);

            layout->addWidget(comp);
        }

        layout->addStretch();
    });
}
