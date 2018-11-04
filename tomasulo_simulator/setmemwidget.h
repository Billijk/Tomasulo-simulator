#ifndef SETMEMWIDGET_H
#define SETMEMWIDGET_H

#include <QWidget>

namespace Ui {
class setMemWidget;
}

class setMemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit setMemWidget(QWidget *parent = 0);
    ~setMemWidget();

private:
    Ui::setMemWidget *ui;

public slots:
    void setMemSlot();

signals:
    void setMemSignal(unsigned int addr, float value);
};

#endif // SETMEMWIDGET_H
