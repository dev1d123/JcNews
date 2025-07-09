#include "statistics.h"
#include "ui_statistics.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QSettings>
#include <QDebug>

Statistics::Statistics(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Statistics)
{
    ui->setupUi(this);
    setWindowTitle("Estadísticas");

    connect(ui->atrasButton, &QPushButton::clicked, this, &QDialog::reject);


}

Statistics::~Statistics()
{
    delete ui;
}
