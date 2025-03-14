#include "employeeswindow.h"
#include "ui_employeeswindow.h"

EmployeesWindow::EmployeesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmployeesWindow)
{
    ui->setupUi(this);
}

EmployeesWindow::~EmployeesWindow()
{
    delete ui;
}
