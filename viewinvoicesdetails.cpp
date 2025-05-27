#include "viewinvoicesdetails.h"
#include "ui_viewinvoicesdetails.h"

ViewInvoicesDetails::ViewInvoicesDetails(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ViewInvoicesDetails)
{
    ui->setupUi(this);
}

ViewInvoicesDetails::~ViewInvoicesDetails()
{
    delete ui;
}
