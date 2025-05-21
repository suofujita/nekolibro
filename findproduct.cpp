#include "findproduct.h"
#include "ui_findproduct.h"

findProduct::findProduct(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::findProduct)
{
    ui->setupUi(this);
}

findProduct::~findProduct()
{
    delete ui;
}
