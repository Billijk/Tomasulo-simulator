#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include "addopswidget.h"
#include "setopwidget.h"
#include "removeopwidget.h"
#include "simulator/instructions.h"
#include "simulator/simulator.h"
#include "simulator/reservation.h"
#include "ncyclewidget.h"
#include "setmemwidget.h"
#include "setregwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Simulator *simulator;
    addOpsWidget *myAddOpsWidget;
    setOpwidget *mySetOpWidget;
    removeOpWidget *myRemoveOpWidget;
    nCycleWidget *myNCyclesWidget;
    setMemWidget *mySetMemWidget;
    setRegWidget *mySetRegWidget;

    std::vector<QString> InstrList;

    QStandardItemModel *opsQueueTableModel;
    QStandardItemModel *runningStatusTableModel;
    QStandardItemModel *loadQueueTableModel;
    QStandardItemModel *storeQueueTableModel;
    QStandardItemModel *memTableModel;
    QStandardItemModel *reservationStationsTableModel;
    QStandardItemModel *FUTableModel;

    bool finished;

    void init();
    void initOpsQueueTableView();
    void initRunningStatusTableView();
    void initLoadQueueTableView();
    void initStoreQueueTableView();
    void initMemTableView();
    void initReservationStationsTableView();
    void initFUTableView();
    void addOp(std::string instr);
    void addOpsFromFile(QString fileName);
    void setOp(int addr, std::string instr);
    void setMem(unsigned int addr, float value);
    void setReg(std::string name, float value);
    void removeOp(int addr);
    void nSteps(unsigned);

    void refreshMyWindow();
    void refreshMyMem();
    void refreshMyReg();
    void get_all_reservations(std::map<std::string, Reservation*>&res);
    void get_load_buffers(std::map<std::string, Reservation*>&res);
    void get_save_buffers(std::map<std::string, Reservation*>&res);
    void get_ops_queue(std::vector<Instruction*>& res);
    void get_used_memory(std::map<unsigned int, float>& res);
    void get_running_status(std::vector<Instruction*>& res);
    void get_total_cycles();
    void get_all_registers(std::map<std::string, float>& res);
    int get_memory_rest_cycles();
    int get_adder_rest_cycles();
    int get_multiplier_rest_cycles();
    int get_pc();


private slots:
    void stepSlot();
    void resetSlot();
    void nStepSlot();
    void finishStepsSlot();
    void addOpSlot();
    void addOpsFromFileSlot();
    void setOpSlot();
    void setRegSlot();
    void setMemSlot();
    void removeOpSlot();
    void addOpDoneSlot(std::string instr);
    void setOpDoneSlot(int addr, std::string instr);
    void setMemDoneSlot(unsigned int addr, float value);
    void setRegDoneSlot(std::string name, float value);
    void nStepDoneSlot(int cycle);
    void removeOpDonwSlot(int addr);
};

#endif // MAINWINDOW_H

