#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){
    //init views
    initOpsQueueTableView();
    initRunningStatusTableView();
    initLoadQueueTableView();
    initStoreQueueTableView();
    initMemTableView();
    initReservationStationsTableView();
    initFUTableView();

    //finished
    finished = true;

    //init widgets
    myAddOpsWidget = new addOpsWidget();
    myAddOpsWidget->hide();
    mySetOpWidget = new setOpwidget();
    mySetOpWidget->hide();
    myRemoveOpWidget = new removeOpWidget();
    myRemoveOpWidget->hide();
    myNCyclesWidget = new nCycleWidget();
    myNCyclesWidget->hide();
    mySetMemWidget = new setMemWidget();
    mySetMemWidget->hide();
    mySetRegWidget = new setRegWidget();
    mySetRegWidget->hide();

    //init signal-slots
    connect(ui->oneCycleButton, SIGNAL(clicked(bool)), this, SLOT(stepSlot()));
    connect(ui->actionOne_Step, SIGNAL(triggered(bool)), this, SLOT(stepSlot()));

    connect(ui->resetButton, SIGNAL(clicked(bool)), this, SLOT(resetSlot()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), this, SLOT(resetSlot()));

    connect(ui->addOpsButton, SIGNAL(clicked(bool)), this, SLOT(addOpSlot()));
    connect(ui->actionAdd_Op, SIGNAL(triggered(bool)), this, SLOT(addOpSlot()));

    connect(ui->addOpsFromFileButton, SIGNAL(clicked(bool)), this, SLOT(addOpsFromFileSlot()));
    connect(ui->actionAdd_Ops_From_File, SIGNAL(triggered(bool)), this, SLOT(addOpsFromFileSlot()));

    connect(ui->setOpButton, SIGNAL(clicked(bool)), this, SLOT(setOpSlot()));
    connect(ui->actionSet_Op, SIGNAL(triggered(bool)), this, SLOT(setOpSlot()));

    connect(ui->actionSet_Memory, SIGNAL(triggered(bool)), this, SLOT(setMemSlot()));

    connect(ui->actionSet_Register, SIGNAL(triggered(bool)), this, SLOT(setRegSlot()));

    connect(ui->removeButton, SIGNAL(clicked(bool)), this, SLOT(removeOpSlot()));
    connect(ui->actionDelete_Op, SIGNAL(triggered(bool)), this, SLOT(removeOpSlot()));

    connect(ui->nStepsButton, SIGNAL(clicked(bool)), this, SLOT(nStepSlot()));
    connect(ui->actionN_Steps, SIGNAL(triggered(bool)), this, SLOT(nStepSlot()));

    connect(ui->finishStepsButton, SIGNAL(clicked(bool)), this, SLOT(finishStepsSlot()));
    connect(ui->actionFinish_Steps, SIGNAL(triggered(bool)), this, SLOT(finishStepsSlot()));

    connect(this->myAddOpsWidget, SIGNAL(addOpSignal(std::string)), this, SLOT(addOpDoneSlot(std::string)));
    connect(this->mySetOpWidget, SIGNAL(setOpSignal(int,std::string)), this, SLOT(setOpDoneSlot(int,std::string)));
    connect(this->myRemoveOpWidget, SIGNAL(removeOpSignal(int)), this, SLOT(removeOpDonwSlot(int)));
    connect(this->myNCyclesWidget, SIGNAL(nCyclesSignal(int)), this, SLOT(nStepDoneSlot(int)));
    connect(this->mySetMemWidget, SIGNAL(setMemSignal(uint,float)), this, SLOT(setMemDoneSlot(uint,float)));
    connect(this->mySetRegWidget, SIGNAL(setRegSignal(std::string,float)), this, SLOT(setRegDoneSlot(std::string,float)));

    //Instr Types
    this->simulator = new Simulator();
    InstrList.push_back("ERROR");
    InstrList.push_back("ADDD");
    InstrList.push_back("SUBD");
    InstrList.push_back("MULD");
    InstrList.push_back("DIVD");
    InstrList.push_back("LD");
    InstrList.push_back("ST");
}

void MainWindow::initOpsQueueTableView(){
    //titles
    this->opsQueueTableModel = new QStandardItemModel();
    this->opsQueueTableModel->setColumnCount(4);
    this->opsQueueTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Name"));
    this->opsQueueTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Desti"));
    this->opsQueueTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Sourcej"));
    this->opsQueueTableModel->setHeaderData(3  , Qt::Horizontal, QString::fromLocal8Bit("Sourcek"));
    ui->opsQueueTableView->setModel(this->opsQueueTableModel);
    ui->opsQueueTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->opsQueueTableView->horizontalHeader()->setStretchLastSection(true);
    ui->opsQueueTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->opsQueueTableView->update();
}

void MainWindow::initRunningStatusTableView(){
    //titles
    this->runningStatusTableModel = new QStandardItemModel();
    this->runningStatusTableModel->setColumnCount(3);
    this->runningStatusTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Issue Ops"));
    this->runningStatusTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Finish Ops"));
    this->runningStatusTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("WB Result"));
    ui->runningStatusTableView->setModel(this->runningStatusTableModel);
    ui->runningStatusTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->runningStatusTableView->horizontalHeader()->setStretchLastSection(true);
    ui->runningStatusTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->runningStatusTableView->update();
}

void MainWindow::initLoadQueueTableView(){
    //titles
    this->loadQueueTableModel = new QStandardItemModel();
    this->loadQueueTableModel->setColumnCount(3);
    this->loadQueueTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Busy"));
    this->loadQueueTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Address"));
    this->loadQueueTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Cache"));
    ui->loadQueueTableView->setModel(this->loadQueueTableModel);
    ui->loadQueueTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->loadQueueTableView->horizontalHeader()->setStretchLastSection(true);
    ui->loadQueueTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->loadQueueTableModel->setItem(0, 0, new QStandardItem("No"));
    this->loadQueueTableModel->setItem(1, 0, new QStandardItem("No"));
    this->loadQueueTableModel->setItem(2, 0, new QStandardItem("No"));
}

void MainWindow::initStoreQueueTableView(){
    //titles
    this->storeQueueTableModel = new QStandardItemModel();
    this->storeQueueTableModel->setColumnCount(3);
    this->storeQueueTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Busy"));
    this->storeQueueTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Address"));
    this->storeQueueTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Qi"));
    ui->storeQueueTableView->setModel(this->storeQueueTableModel);
    ui->storeQueueTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->storeQueueTableView->horizontalHeader()->setStretchLastSection(true);
    ui->storeQueueTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->storeQueueTableModel->setItem(0, 0, new QStandardItem("No"));
    this->storeQueueTableModel->setItem(1, 0, new QStandardItem("No"));
    this->storeQueueTableModel->setItem(2, 0, new QStandardItem("No"));
}

void MainWindow::initMemTableView(){
    //TODO
    this->memTableModel = new QStandardItemModel();
    this->memTableModel->setColumnCount(2);
    this->memTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Position"));
    this->memTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Data"));
    ui->memTableView->setModel(this->memTableModel);
    ui->memTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->memTableView->horizontalHeader()->setStretchLastSection(true);
    ui->memTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::initReservationStationsTableView(){
    //titles
    this->reservationStationsTableModel = new QStandardItemModel();
    this->reservationStationsTableModel->setColumnCount(8);
    this->reservationStationsTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Time"));
    this->reservationStationsTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Name"));
    this->reservationStationsTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Busy"));
    this->reservationStationsTableModel->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("Op"));
    this->reservationStationsTableModel->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Vi"));
    this->reservationStationsTableModel->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("Vk"));
    this->reservationStationsTableModel->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("Qi"));
    this->reservationStationsTableModel->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Qk"));
    ui->reservationStationsTableView->setModel(this->reservationStationsTableModel);
    ui->reservationStationsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->reservationStationsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->reservationStationsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->reservationStationsTableModel->setItem(0, 1, new QStandardItem("ADD_1"));
    this->reservationStationsTableModel->setItem(0, 2, new QStandardItem("No"));
    this->reservationStationsTableModel->setItem(1, 1, new QStandardItem("ADD_2"));
    this->reservationStationsTableModel->setItem(1, 2, new QStandardItem("No"));
    this->reservationStationsTableModel->setItem(2, 1, new QStandardItem("ADD_3"));
    this->reservationStationsTableModel->setItem(2, 2, new QStandardItem("No"));
    this->reservationStationsTableModel->setItem(3, 1, new QStandardItem("MUL_1"));
    this->reservationStationsTableModel->setItem(3, 2, new QStandardItem("No"));
    this->reservationStationsTableModel->setItem(4, 1, new QStandardItem("MUL_2"));
    this->reservationStationsTableModel->setItem(4, 2, new QStandardItem("No"));
}

void MainWindow::initFUTableView(){
    //titles
    this->FUTableModel = new QStandardItemModel();
    this->FUTableModel->setColumnCount(10);
    this->FUTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("F0"));
    this->FUTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("F1"));
    this->FUTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("F2"));
    this->FUTableModel->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("F3"));
    this->FUTableModel->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("F4"));
    this->FUTableModel->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("F5"));
    this->FUTableModel->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("F6"));
    this->FUTableModel->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("F7"));
    this->FUTableModel->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("F8"));
    this->FUTableModel->setHeaderData(9, Qt::Horizontal, QString::fromLocal8Bit("F9"));
    ui->fuTableView->setModel(this->FUTableModel);
    ui->fuTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->fuTableView->horizontalHeader()->setStretchLastSection(true);
    ui->fuTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for(int i=0; i<10; i++){
        this->FUTableModel->setItem(0, i, new QStandardItem(""));
    }
    for(int i=0; i<10; i++){
        this->FUTableModel->setItem(1, i, new QStandardItem("0"));
    }
}

void MainWindow::stepSlot(){
    int fail = simulator->step();
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
    if(finished){
        QMessageBox::information(NULL, "FINISHED", "All Assignments Cleared!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void MainWindow::resetSlot(){
    int fail = simulator->reset();
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
}

void MainWindow::nStepSlot(){
    this->myNCyclesWidget->show();
}

void MainWindow::finishStepsSlot(){
    while(!finished){
        simulator->step();
        refreshMyWindow();
    }
    if(finished){
        QMessageBox::information(NULL, "FINISHED", "All Assignments Cleared!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void MainWindow::addOpSlot(){
    this->myAddOpsWidget->show();
}

void MainWindow::addOpsFromFileSlot(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("open file"), " ",  tr("Allfile(*.*);;"));
    std::cout << fileName.toStdString() << std::endl;
    addOpsFromFile(fileName);
}

void MainWindow::setOpSlot(){
    this->mySetOpWidget->show();
}

void MainWindow::setMemSlot(){
    this->mySetMemWidget->show();
}

void MainWindow::setRegSlot(){
    this->mySetRegWidget->show();
}

void MainWindow::removeOpSlot(){
    this->myRemoveOpWidget->show();
}

void MainWindow::nStepDoneSlot(int cycle){
    unsigned ucycle = unsigned(cycle);
    this->nSteps(ucycle);
}

void MainWindow::addOpDoneSlot(std::string instr){
    this->addOp(instr);
}

void MainWindow::setOpDoneSlot(int addr, std::string instr){
    this->setOp(addr, instr);
}

void MainWindow::setMemDoneSlot(unsigned int addr, float value){
    this->setMem(addr, value);
}

void MainWindow::setRegDoneSlot(std::string name, float value){
    this->setReg(name, value);
}

void MainWindow::removeOpDonwSlot(int addr){
    this->removeOp(addr);
}

void MainWindow::nSteps(unsigned ucycle){
    int fail = simulator->step(ucycle);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
    if(finished){
        QMessageBox::information(NULL, "FINISHED", "All Assignments Cleared!", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void MainWindow::addOp(std::string instr){
    int fail = simulator->add_instruction(instr);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong Op,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
}

void MainWindow::addOpsFromFile(QString fileName){
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(NULL, "failed", "Wrong File,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return ;
    }
    QTextStream txtInput(&f);
    QString lineStr;
    while(!txtInput.atEnd()){
        lineStr = txtInput.readLine();
        addOp(lineStr.toStdString());
    }
    f.close();
}

void MainWindow::setOp(int addr, std::string instr){
    int fail = simulator->set_instruction(addr-1, instr);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
}

void MainWindow::setMem(unsigned int addr, float value){
    int fail = simulator->set_memory(addr, value);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyMem();
}

void MainWindow::setReg(std::string name, float value){
    int fail = simulator->set_register(name, value);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
}

void MainWindow::removeOp(int addr){
    int fail = simulator->remove_instruction(addr-1);
    if(fail){
        QMessageBox::warning(NULL, "failed", "Wrong operation,please try again.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
    refreshMyWindow();
}

void MainWindow::refreshMyWindow(){

    std::map<std::string, Reservation*> res;
    get_load_buffers(res);
    res.clear();
    get_save_buffers(res);
    res.clear();
    get_all_reservations(res);
    res.clear();

    get_total_cycles();

    std::vector<Instruction*> instrRes;
    get_ops_queue(instrRes);
    instrRes.clear();

    refreshMyMem();

    refreshMyReg();

}

void MainWindow::refreshMyMem(){
    std::map<unsigned int, float> memRes;
    get_used_memory(memRes);
    memRes.clear();
}

void MainWindow::refreshMyReg(){
    std::map<std::string, float> regRes;
    get_all_registers(regRes);
    regRes.clear();
}

void MainWindow::get_all_reservations(std::map<std::string, Reservation*>&res){
    this->simulator->get_add_reservations(res);

    std::map<std::string, Reservation*>::iterator it;
    int index = 0;
    for(it=res.begin(); it!=res.end(); it++){

        std::cout << it->second->get_name() << std::endl;
        std::cout << index << std::endl;

        //rest time
        this->reservationStationsTableModel->setItem(index, 0, new QStandardItem(QString::number(it->second->get_rest_cycles())));

        //name
        QString name = QString::fromStdString(it->second->get_name());
        this->reservationStationsTableModel->setItem(index, 1, new QStandardItem(name));

        //busy
        if(it->second->is_buzy()){
            this->reservationStationsTableModel->setItem(index, 2, new QStandardItem("YES"));
        }else{
            this->reservationStationsTableModel->setItem(index, 2, new QStandardItem("NO"));
            for(int i=3; i<8; i++){
                this->reservationStationsTableModel->setItem(index, i, new QStandardItem(""));
            }

            index ++;
            continue ;
        }

        //Op
        int opIndex = int(it->second->get_op());
        QString Op = "";
        if(opIndex < 7 && opIndex >=0){
            Op = InstrList[int(it->second->get_op())];
        }else{
            Op = "***";
        }
        this->reservationStationsTableModel->setItem(index, 3, new QStandardItem(Op));


        //Vi
        QString Vi = QString("%1").arg(it->second->get_vj());
        this->reservationStationsTableModel->setItem(index, 4, new QStandardItem(Vi));

        //Vk
        QString Vk = QString("%1").arg(it->second->get_vk());
        this->reservationStationsTableModel->setItem(index, 5, new QStandardItem(Vk));


        //Qi
        QString Qi = QString::fromStdString(it->second->get_qj());
        this->reservationStationsTableModel->setItem(index, 6, new QStandardItem(Qi));

        //Qk
        QString Qk = QString::fromStdString(it->second->get_qk());
        this->reservationStationsTableModel->setItem(index, 7, new QStandardItem(Qk));

        index ++;
    }

    res.clear();
    this->simulator->get_mul_reservations(res);
    for(it=res.begin(); it!=res.end(); it++){

        //rest time
        this->reservationStationsTableModel->setItem(index, 0, new QStandardItem(QString::number(it->second->get_rest_cycles())));

        //name
        QString name = QString::fromStdString(it->second->get_name());
        this->reservationStationsTableModel->setItem(index, 1, new QStandardItem(name));

        //busy
        if(it->second->is_buzy()){
            this->reservationStationsTableModel->setItem(index, 2, new QStandardItem("YES"));
        }else{
            this->reservationStationsTableModel->setItem(index, 2, new QStandardItem("NO"));
            for(int i=3; i<8; i++){
                this->reservationStationsTableModel->setItem(index, i, new QStandardItem(""));
            }

            index ++;
            continue ;
        }

        //Op
        int opIndex = int(it->second->get_op());
        QString Op = "";
        if(opIndex < 7 && opIndex >=0){
            Op = InstrList[int(it->second->get_op())];
        }else{
            Op = "***";
        }
        this->reservationStationsTableModel->setItem(index, 3, new QStandardItem(Op));


        //Vi
        QString Vi = QString("%1").arg(it->second->get_vj());
        this->reservationStationsTableModel->setItem(index, 4, new QStandardItem(Vi));

        //Vk
        QString Vk = QString("%1").arg(it->second->get_vk());
        this->reservationStationsTableModel->setItem(index, 5, new QStandardItem(Vk));

        //Qi
        QString Qi = QString::fromStdString(it->second->get_qj());
        this->reservationStationsTableModel->setItem(index, 6, new QStandardItem(Qi));

        //Qk
        QString Qk = QString::fromStdString(it->second->get_qk());
        this->reservationStationsTableModel->setItem(index, 7, new QStandardItem(Qk));

        index ++;
    }

    ui->reservationStationsTableView->update();
}

void MainWindow::get_load_buffers(std::map<std::string, Reservation*>&res){
    this->simulator->get_load_buffers(res);
    std::map<std::string, Reservation*>::iterator it;
    int index = 0;
    for(it=res.begin(); it!=res.end(); it++){
        if(it->second->is_buzy()){
            this->loadQueueTableModel->setItem(index, 0, new QStandardItem("YES"));
        }else{
            this->loadQueueTableModel->setItem(index, 0, new QStandardItem("NO"));
            for(int i=1; i<3; i++){
                this->loadQueueTableModel->setItem(index, i, new QStandardItem(""));
            }
            index ++;
            continue ;
        }

        QString address = QString("%1").arg(it->second->get_vj());
        this->loadQueueTableModel->setItem(index, 1, new QStandardItem(address));

        QString cache = QString::fromStdString(it->second->get_qj());
        this->loadQueueTableModel->setItem(index, 2, new QStandardItem(cache));
        index ++;
    }

    ui->loadQueueTableView->update();
}

void MainWindow::get_save_buffers(std::map<std::string, Reservation*>&res){
    this->simulator->get_save_buffers(res);
    std::map<std::string, Reservation*>::iterator it;
    int index = 0;
    for(it=res.begin(); it!=res.end(); it++){
        if(it->second->is_buzy()){
            this->storeQueueTableModel->setItem(index, 0, new QStandardItem("YES"));
        }else{
            this->storeQueueTableModel->setItem(index, 0, new QStandardItem("NO"));
            for(int i=1; i<3; i++){
                this->storeQueueTableModel->setItem(index, i, new QStandardItem(""));
            }
            index ++;
            continue;
        }

        QString address = QString("%1").arg(it->second->get_vj());
        this->storeQueueTableModel->setItem(index, 1, new QStandardItem(address));

        QString Qi = QString::fromStdString(it->second->get_qj());
        this->storeQueueTableModel->setItem(index, 2, new QStandardItem(Qi));
        index ++;
    }

    ui->storeQueueTableView->update();
}

void MainWindow::get_ops_queue(std::vector<Instruction*>& res){
    this->simulator->get_all_instructions(res);
    int index = 0;
    this->opsQueueTableModel->clear();
    this->opsQueueTableModel->setColumnCount(4);
    this->opsQueueTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Name"));
    this->opsQueueTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Desti"));
    this->opsQueueTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Sourcej"));
    this->opsQueueTableModel->setHeaderData(3  , Qt::Horizontal, QString::fromLocal8Bit("Sourcek"));

    for(int i=0; i<res.size(); i++){
        int instrIndex = res[i]->get_type();
        QString Op = "";
        if(instrIndex < 7 && instrIndex >=0){
            Op = InstrList[int(res[i]->get_type())];
        }else{
            Op = "***";
        }
        this->opsQueueTableModel->setItem(index, 0, new QStandardItem(Op));

        this->opsQueueTableModel->setItem(index, 1, new QStandardItem(QString::fromStdString(res[i]->get_dst())));
        this->opsQueueTableModel->setItem(index, 2, new QStandardItem(QString::fromStdString(res[i]->get_src1())));
        this->opsQueueTableModel->setItem(index, 3, new QStandardItem(QString::fromStdString(res[i]->get_src2())));

        index ++;
    }

    get_running_status(res);

    ui->opsQueueTableView->update();
}

void MainWindow::get_running_status(std::vector<Instruction *> &res){
    int index = 0;
    this->runningStatusTableModel->clear();
    this->runningStatusTableModel->setColumnCount(3);
    this->runningStatusTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Issue Ops"));
    this->runningStatusTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Finish Ops"));
    this->runningStatusTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("WB Result"));
    bool finish = true;
    for(int i=0; i<res.size(); i++){
        InstrStatus status = res[i]->get_status();
        if(status == WAITING){
            this->runningStatusTableModel->setItem(index, 0, new QStandardItem(""));
            this->runningStatusTableModel->setItem(index, 1, new QStandardItem(""));
            this->runningStatusTableModel->setItem(index, 2, new QStandardItem(""));
            finish = false;
        }else if(status == ISSUED){
            this->runningStatusTableModel->setItem(index, 0, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 1, new QStandardItem(""));
            this->runningStatusTableModel->setItem(index, 2, new QStandardItem(""));
            finish = false;
        }else if(status == EXECUTING){
            this->runningStatusTableModel->setItem(index, 0, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 1, new QStandardItem(QString::fromLocal8Bit("EXECUTING")));
            this->runningStatusTableModel->setItem(index, 2, new QStandardItem(""));
            finish = false;
        }else if(status == EXECUTED){
            this->runningStatusTableModel->setItem(index, 0, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 1, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 2, new QStandardItem(""));
            finish = false;
        }else if(status == DONE){
            this->runningStatusTableModel->setItem(index, 0, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 1, new QStandardItem(QString::fromLocal8Bit("DONE")));
            this->runningStatusTableModel->setItem(index, 2, new QStandardItem(QString::fromLocal8Bit("DONE")));
        }
        index ++;
    }

    if(finish){
        this->finished = true;

    }else{
        this->finished = false;
    }

    ui->runningStatusTableView->update();
}

void MainWindow::get_used_memory(std::map<unsigned int, float> &res){
    this->simulator->get_used_memory(res);
    int index = 0;
    std::map<unsigned int, float>::iterator it;
    this->memTableModel->clear();
    this->memTableModel->setColumnCount(2);
    this->memTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Position"));
    this->memTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Data"));
    for(it=res.begin(); it!=res.end(); it++){
        this->memTableModel->setItem(index, 0, new QStandardItem(QString::number(it->first, 10)));
        QString data = QString("%1").arg(it->second);
        this->memTableModel->setItem(index, 1, new QStandardItem(data));

        index ++;
    }

    ui->memTableView->update();
}

void MainWindow::get_total_cycles(){
    int cycles = this->simulator->get_total_cycles();
    ui->CyclesBrowser->setText(QString::number(cycles, 10));
    ui->CyclesBrowser->update();
}

void MainWindow::get_all_registers(std::map<std::string, float> &res){
    this->simulator->get_all_registers(res);
    int index = 0;
    std::map<std::string, float>::iterator it;
    for(it=res.begin(); it!=res.end(); it++){
        this->FUTableModel->setItem(1, index, new QStandardItem(QString::number(it->second)));

        index ++;
    }

    std::map<std::string, std::string> qsRes;
    this->simulator->get_all_register_qs(qsRes);
    index = 0;
    std::map<std::string, std::string>::iterator qsit;
    for(qsit=qsRes.begin(); qsit!=qsRes.end(); qsit++){
        if(qsit->second == "None"){
            this->FUTableModel->setItem(0, index, new QStandardItem(""));
        }else{
            this->FUTableModel->setItem(0, index, new QStandardItem(QString::fromStdString(qsit->second)));
        }
        index ++;
    }

    ui->fuTableView->update();
}
