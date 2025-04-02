#include "imexport.h"
#include "ui_imexport.h"

ImExport::ImExport(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImExport)
{
    ui->setupUi(this);

    connect(ui->to_main,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->to_main_2,&QDialogButtonBox::accepted,this,&ImExport::toMainImExport);    // tra cứu lịch sử nhập hàng
    connect(ui->to_main_3,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->to_main_4,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->to_main_5,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->to_main_6,&QDialogButtonBox::accepted,this,&ImExport::toMainImExport);      // tra cứu lịch sử xuất hàng


    connect(ui->to_export_infor, &QPushButton::clicked,this,&ImExport::toExportInfor);
    connect(ui->to_import_infor,&QPushButton::clicked,this,&ImExport::toImportInfor);
    connect(ui->to_export_histories,&QPushButton::clicked,this,&ImExport::toExportHistories);
    connect(ui->to_import_histories,&QPushButton::clicked,this,&ImExport::toImportHistories);


    /* Kết nối với cơ sở dữ liệu */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
    }
}

ImExport::~ImExport()
{
    delete ui;
}

void ImExport::toMainImExport(){
    ui->stackedWidget->setCurrentIndex(0);
}

void ImExport::toExportInfor(){
    ui->stackedWidget->setCurrentIndex(4);
}

void ImExport::toImportInfor(){
    ui->stackedWidget->setCurrentIndex(2);
}

void ImExport::toExportHistories(){
    ui->stackedWidget->setCurrentIndex(3);
}
void ImExport::toImportHistories(){
    ui->stackedWidget->setCurrentIndex(1);
}
