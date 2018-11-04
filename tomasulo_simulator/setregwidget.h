#ifndef SETREGWIDGET_H
#define SETREGWIDGET_H

#include <QWidget>

namespace Ui {
class setRegWidget;
}

class setRegWidget : public QWidget
{
    Q_OBJECT

public:
    explicit setRegWidget(QWidget *parent = 0);
    ~setRegWidget();

private:
    Ui::setRegWidget *ui;

public slots:
    void setRegSlot();

signals:
    void setRegSignal(std::string name, float value);
};

#endif // SETREGWIDGET_H
