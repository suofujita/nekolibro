#include "details.h"
#include "ui_details.h"

Details::Details(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Details)
{
    ui->setupUi(this);
}

Details::~Details()
{
    delete ui;
}
