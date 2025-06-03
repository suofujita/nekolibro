#include "accountswindow.h"
#include "ui_accountswindow.h"

AccountsWindow::AccountsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AccountsWindow)
{
    ui->setupUi(this);
}

AccountsWindow::~AccountsWindow()
{
    delete ui;
}
