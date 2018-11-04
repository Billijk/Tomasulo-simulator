#include "ncyclewidget.h"
#include "ui_ncyclewidget.h"

nCycleWidget::nCycleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::nCycleWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(nCyclesSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

nCycleWidget::~nCycleWidget()
{
    delete ui;
}

void nCycleWidget::nCyclesSlot(){
    QString numberQString = ui->nCyclesEdit->text();
    int number = numberQString.toInt();
    nCyclesSignal(number);
    this->hide();
}
