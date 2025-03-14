#include "categorieswindow.h"
#include "ui_categorieswindow.h"

CategoriesWindow::CategoriesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CategoriesWindow)
{
    ui->setupUi(this);
}

CategoriesWindow::~CategoriesWindow()
{
    delete ui;
}
