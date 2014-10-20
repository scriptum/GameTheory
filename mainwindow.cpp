#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    solver = new Solver;
    solver->table = ui->tableWidget;
    solver->spin = ui->spinBox;
    updateTable(3);
    ui->progressBar->hide();
    connect(solver, SIGNAL(started()), ui->progressBar, SLOT(show()));
    connect(solver, SIGNAL(started()), ui->textBrowser, SLOT(clear()));
    connect(solver, SIGNAL(finished()), ui->progressBar, SLOT(hide()));
    connect(solver, SIGNAL(terminated()), ui->progressBar, SLOT(hide()));
    connect(solver, SIGNAL(sendText(QString)), ui->textBrowser, SLOT(append(QString)));
    connect(solver, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//вычисляет размер коалиции всего за 3 такта на быстрых процессорах, однако макс. размер коалиции - 14
//с другой стороны... при размере коалиции 14 размерность задачи где-то = 978185727308538673980007692457627547968139250
inline unsigned int coalitionSize(unsigned int n)
{
    return (n * 0x200040008001ULL & 0x111111111111111ULL) % 0xf;
}

QString intToCoalition(unsigned int n)
{
    QString res;
    for(int i = 0; i < 32; i++)
    {
        if(n & (1 << i)) res += QString::number(i+1) + " ";
        else res += "  ";
    }
    return res;
}

void MainWindow::updateTable(int num)
{
    int max = (1 << num) - 1;
    ui->tableWidget->setRowCount(max);
    QTableWidgetItem *item;
    for(int i = 0; i < max; i++)
    {
        item = new QTableWidgetItem(intToCoalition(i+1));
        item->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(i,0,item);
        item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        ui->tableWidget->setItem(i,1,item);
    }
}

void MainWindow::random()
{
    int max = (1 << ui->spinBox->value()) - 1;
    for(int i = 0; i < max; i++)
    {
        ui->tableWidget->item(i,1)->setText(QString::number(5*(coalitionSize(i+1)-1)+qrand()%4));
    }
}

void MainWindow::solve()
{
    if(!solver->isRunning())
        solver->start();
}
