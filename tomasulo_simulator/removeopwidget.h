#ifndef REMOVEOPWIDGET_H
#define REMOVEOPWIDGET_H

#include <QWidget>
#include "simulator/instructions.h"
#include "simulator/simulator.h"
#include "simulator/reservation.h"

namespace Ui {
class removeOpWidget;
}

class removeOpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit removeOpWidget(QWidget *parent = 0);
    ~removeOpWidget();

private:
    Ui::removeOpWidget *ui;

public slots:
    void removeOpSlot();

signals:
    void removeOpSignal(int addr);
};

#endif // REMOVEOPWIDGET_H
