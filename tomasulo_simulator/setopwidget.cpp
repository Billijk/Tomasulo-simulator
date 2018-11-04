#include "setopwidget.h"
#include "ui_setopwidget.h"

setOpwidget::setOpwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setOpwidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(setOpSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

setOpwidget::~setOpwidget()
{
    delete ui;
}

void setOpwidget::setOpSlot(){
    QString indexQString = ui->indexEdit->text();
    QString instructionQString = ui->setOpEdit->text();
    std::string instruction = instructionQString.toStdString();
    int index = indexQString.toInt();
    setOpSignal(index, instruction);
    this->hide();
}
