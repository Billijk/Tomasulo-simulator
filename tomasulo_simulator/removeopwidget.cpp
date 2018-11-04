#include "removeopwidget.h"
#include "ui_removeopwidget.h"

removeOpWidget::removeOpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::removeOpWidget)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked(bool)), this, SLOT(removeOpSlot()));
    connect(ui->cancelButton, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

removeOpWidget::~removeOpWidget()
{
    delete ui;
}

void removeOpWidget::removeOpSlot(){
    QString indexQString = ui->indexEdit->text();
    int index = indexQString.toInt();
    removeOpSignal(index);
    this->hide();
}
