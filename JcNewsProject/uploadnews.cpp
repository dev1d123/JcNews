#include "uploadnews.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QDialogButtonBox>

UploadNews::UploadNews(QWidget *parent)
    : QDialog(parent)
{
    // Crear widgets
    dirLine = new QLineEdit(this);
    titleLine = new QLineEdit(this);
    toolButton = new QToolButton(this);
    toolButton->setText("...");

    fromTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    fromTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    toTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    toTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    contentEdit = new QTextEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Dir + toolButton
    QHBoxLayout* dirLayout = new QHBoxLayout;
    dirLayout->addWidget(new QLabel("Archivo:", this));
    dirLayout->addWidget(dirLine);
    dirLayout->addWidget(toolButton);

    mainLayout->addLayout(dirLayout);

    // Title
    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->addWidget(new QLabel("Título:", this));
    titleLayout->addWidget(titleLine);
    mainLayout->addLayout(titleLayout);

    // From / To datetime
    QHBoxLayout* timeLayout = new QHBoxLayout;
    timeLayout->addWidget(new QLabel("Desde:", this));
    timeLayout->addWidget(fromTimeEdit);
    timeLayout->addWidget(new QLabel("Hasta:", this));
    timeLayout->addWidget(toTimeEdit);
    mainLayout->addLayout(timeLayout);

    // Content
    mainLayout->addWidget(new QLabel("Contenido:", this));
    mainLayout->addWidget(contentEdit);

    // Buttons
    mainLayout->addWidget(buttonBox);

    // Conexiones
    connect(toolButton, &QToolButton::clicked, this, &UploadNews::loadFile);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &UploadNews::showNewsInfo);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &UploadNews::reject);

    setLayout(mainLayout);
    setWindowTitle("Subir Noticia");
    resize(600, 400);
}

void UploadNews::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "Archivos de texto (*.txt *.json *.pdf);;Todos los archivos (*)");
    if (!fileName.isEmpty()) {
        dirLine->setText(fileName);
        parseFile(fileName);
    }
}

void UploadNews::parseFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    QString fullText = in.readAll();
    file.close();

    // Título: primeras líneas no vacías
    QStringList lines = fullText.split('\n', Qt::SkipEmptyParts);
    if (!lines.isEmpty())
        titleLine->setText(lines.first().trimmed());

    // Buscar fecha con regex (ejemplo YYYY-MM-DD)
    QRegularExpression dateRegex(R"((\d{4})[-/](\d{1,2})[-/](\d{1,2}))");
    QRegularExpressionMatch match = dateRegex.match(fullText);
    if (match.hasMatch()) {
        int year = match.captured(1).toInt();
        int month = match.captured(2).toInt();
        int day = match.captured(3).toInt();
        if (QDate::isValid(year, month, day)) {
            QDate date(year, month, day);
            QDateTime dt(date, QTime(0,0,0));
            fromTimeEdit->setDateTime(dt);
            toTimeEdit->setDateTime(dt.addDays(1));
        }

    }

    // Contenido: resto del texto sin la primera línea
    if (lines.size() > 1) {
        QString content;
        for (int i = 1; i < lines.size(); ++i) {
            content += lines[i] + "\n";
        }
        contentEdit->setPlainText(content.trimmed());
    }
}

void UploadNews::showNewsInfo()
{
    QString info;
    info += "Título: " + titleLine->text() + "\n";
    info += "Archivo: " + dirLine->text() + "\n";
    info += "Desde: " + fromTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm") + "\n";
    info += "Hasta: " + toTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm") + "\n";
    info += "Contenido:\n" + contentEdit->toPlainText();

    QMessageBox::information(this, "Información de la noticia", info);
}
