#include "newscomponent.h"
#include "ui_newscomponent.h"

NewsComponent::NewsComponent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewsComponent)
{
    ui->setupUi(this);
}

NewsComponent::~NewsComponent()
{
    delete ui;
}
