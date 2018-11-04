#include "setregwidget.h"
#include "ui_setregwidget.h"

setRegWidget::setRegWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setRegWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(setRegSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

setRegWidget::~setRegWidget()
{
    delete ui;
}

void setRegWidget::setRegSlot(){
    QString nameQString = ui->regEdit->text();
    std::string name = nameQString.toStdString();
    QString valueQString = ui->valueEdit->text();
    float value = valueQString.toFloat();
    setRegSignal(name, value);
    this->hide();
}
