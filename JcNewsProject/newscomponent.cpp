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
