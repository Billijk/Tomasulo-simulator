#ifndef NCYCLEWIDGET_H
#define NCYCLEWIDGET_H

#include <QWidget>
#include "simulator/instructions.h"
#include "simulator/simulator.h"
#include "simulator/reservation.h"

namespace Ui {
class nCycleWidget;
}

class nCycleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit nCycleWidget(QWidget *parent = 0);
    ~nCycleWidget();

private:
    Ui::nCycleWidget *ui;

public slots:
    void nCyclesSlot();

signals:
    void nCyclesSignal(int);
};

#endif // NCYCLEWIDGET_H
