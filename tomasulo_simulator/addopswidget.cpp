#include "addopswidget.h"
#include "ui_addopswidget.h"

addOpsWidget::addOpsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addOpsWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(addOpSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

addOpsWidget::~addOpsWidget()
{
    delete ui;
}

void addOpsWidget::addOpSlot(){
    QString instructionQString = ui->addOpsEdit->text();
    std::string instruction = instructionQString.toStdString();
    addOpSignal(instruction);
    this->hide();
}
