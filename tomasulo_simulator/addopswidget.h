#ifndef ADDOPSWIDGET_H
#define ADDOPSWIDGET_H

#include <QWidget>
#include "simulator/instructions.h"
#include "simulator/simulator.h"
#include "simulator/reservation.h"

namespace Ui {
class addOpsWidget;
}

class addOpsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit addOpsWidget(QWidget *parent = 0);
    ~addOpsWidget();

private:
    Ui::addOpsWidget *ui;

public slots:
    void addOpSlot();

signals:
    void addOpSignal(std::string);
};

#endif // ADDOPSWIDGET_H
