#include "nekolibro.h"
#include "./ui_nekolibro.h"

#include "categorieswindow.h"
#include "imexport.h"
NekoLibro::NekoLibro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NekoLibro)
{
    ui->setupUi(this);

    /* connect signals & slots */
    connect(ui->logout, &QPushButton::clicked,this,&NekoLibro::clickedLogOut);
    connect(ui->sales, &QPushButton::clicked,this,&NekoLibro::openSalesWindow);
    connect(ui->employees, &QPushButton::clicked,this,&NekoLibro::openEmployeesWindow);
    connect(ui->categories,&QPushButton::clicked,this,&NekoLibro::openCategoriesWindow);
    connect(ui->import_export,&QPushButton::clicked,this,&NekoLibro::openImExportWindow);
}

NekoLibro::~NekoLibro()
{
    delete ui;
}

void NekoLibro::clickedLogOut(){
    this->close();

    /* Giải phóng vùng nhớ */
    if(pSaleWindow) {
        delete pSaleWindow;
        pSaleWindow = nullptr;
    }
    if(pEmployeesWindow) {
        delete pEmployeesWindow;
        pEmployeesWindow = nullptr;
    }

    pLogin = new login();
    pLogin->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pLogin->show();
}

void NekoLibro::openSalesWindow(){
    pSaleWindow = new SalesWindow;
    pSaleWindow->setAttribute(Qt::WA_DeleteOnClose);  // cửa sổ đóng thì giải phóng vùng nhớ
    pSaleWindow->show();
}

void NekoLibro::openEmployeesWindow(){
    pEmployeesWindow = new EmployeesWindow;
    pEmployeesWindow->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pEmployeesWindow->show();
}

void NekoLibro::openCategoriesWindow(){
    pCategoriesWindow = new CategoriesWindow;
    pCategoriesWindow->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pCategoriesWindow->show();
}

QStringList NekoLibro::getCategoriesList(){
    QStringList categoriesList;
    QSqlQuery query("SELECT name FROM Categories");
    while (query.next()) {
        categoriesList << query.value(0).toString(); // Lấy cột đầu tiên là tên sản phẩm
    }
    return categoriesList;
}

void NekoLibro::openImExportWindow(){
   // if(!pImExportWindow) {
        pImExportWindow = new ImExport;
   // }
    pImExportWindow->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pImExportWindow->show();
}




