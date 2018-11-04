#ifndef SETOPWIDGET_H
#define SETOPWIDGET_H

#include <QWidget>
#include "simulator/instructions.h"
#include "simulator/simulator.h"
#include "simulator/reservation.h"

namespace Ui {
class setOpwidget;
}

class setOpwidget : public QWidget
{
    Q_OBJECT

public:
    explicit setOpwidget(QWidget *parent = 0);
    ~setOpwidget();

private:
    Ui::setOpwidget *ui;

public slots:
    void setOpSlot();

signals:
    void setOpSignal(int addr, std::string instr);
};

#endif // SETOPWIDGET_H
