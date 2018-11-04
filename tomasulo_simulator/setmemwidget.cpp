#include "setmemwidget.h"
#include "ui_setmemwidget.h"

setMemWidget::setMemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setMemWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(setMemSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

setMemWidget::~setMemWidget()
{
    delete ui;
}

void setMemWidget::setMemSlot(){
    QString addrQString = ui->addrEdit->text();
    int addrInt = addrQString.toInt();
    unsigned int addr = uint(addrInt);
    QString valueQString = ui->valueEdit->text();
    float value = valueQString.toFloat();
    setMemSignal(addr, value);
    this->hide();
}
