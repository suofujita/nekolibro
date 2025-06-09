#include "viewinvoicesdetails.h"
#include "ui_viewinvoicesdetails.h"

ViewInvoicesDetails::ViewInvoicesDetails(const QString &invoiceId, InvoiceType type, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ViewInvoicesDetails), invoiceIdCurrent(invoiceId), InvoiceTypeCurrent(type)
{
    ui->setupUi(this);

    /* Kết nối với cơ sở dữ liệu */
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection");
        db.setDatabaseName("nekolibro.db");
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Lỗi mở cơ sở dữ liệu:" << db.lastError().text();
        } else {
            qDebug() << "Mở cơ sở dữ liệu thành công!";
        }
    }


    /* signals & slots*/
    connect(ui->close_tab, &QPushButton::clicked,this,&ViewInvoicesDetails::closeTab);

    /* Bảng chứa lịch sử xuất hàng */
    ui->products->setColumnCount(3);
    ui->products->setHorizontalHeaderLabels({"Mã sản phẩm", "Tên sản phẩm", "Số lượng"});
    ui->products->horizontalHeader()->setStretchLastSection(true);
    QHeaderView *headerExportLogs = ui->products->horizontalHeader();
    headerExportLogs->setSectionResizeMode(1, QHeaderView::Stretch);
    headerExportLogs->setSectionResizeMode(0, QHeaderView::Fixed);
    headerExportLogs->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->products->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->products->setColumnWidth(2, 100);
    ui->products->horizontalHeader()->setStretchLastSection(false);
    qDebug() << invoiceIdCurrent;
    loadInvoiceDetails();
}

ViewInvoicesDetails::~ViewInvoicesDetails()
{
    delete ui;
}

void ViewInvoicesDetails::closeTab(){
    this->close();
}

void ViewInvoicesDetails::loadInvoiceDetails() {
    if(InvoiceTypeCurrent != Import && InvoiceTypeCurrent != Export && InvoiceTypeCurrent != Retail)
        return;
    if (InvoiceTypeCurrent == Import) {
        QSqlQuery query;
        query.prepare(R"(
        SELECT
            Products.id,
            Products.title || ' - ' || Authors.name AS full_title,
            ImportInvoicesItems.quantity,
            ImportInvoices.bill_num,
            ImportInvoices.date,
            ImportInvoices.total_products
        FROM ImportInvoicesItems
        JOIN Products ON ImportInvoicesItems.product_id = Products.id
        JOIN Authors ON Products.author_id = Authors.id
        JOIN ImportInvoices ON ImportInvoicesItems.invoice_id = ImportInvoices.id
        WHERE ImportInvoicesItems.invoice_id = ?
        )");
        query.addBindValue(invoiceIdCurrent);


        if (!query.exec()) {
            QMessageBox::warning(this, "Lỗi", "Không thể tải chi tiết hóa đơn nhập:\n" + query.lastError().text());
            return;
        }

        ui->products->setRowCount(0);
        int row = 0;
        while (query.next()) {
            QString productId = query.value(0).toString();
            QString fullTitle = query.value(1).toString();
            int quantity = query.value(2).toInt();

            ui->products->insertRow(row);
            ui->products->setItem(row, 0, new QTableWidgetItem(productId));
            ui->products->setItem(row, 1, new QTableWidgetItem(fullTitle));
            ui->products->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
            row++;


            ui->bill_num->setText(query.value(3).toString());
            ui->date->setText(query.value(4).toString());
            ui->total_quantities->setText(query.value(5).toString());
        }
    }
    if (InvoiceTypeCurrent == Export) {
        QSqlQuery query;
        query.prepare(R"(
        SELECT
            Products.id,
            Products.title || ' - ' || Authors.name AS full_title,
            ExportInvoicesItems.quantity,
            ExportInvoices.bill_num,
            ExportInvoices.date,
            ExportInvoices.total_products
        FROM ExportInvoicesItems
        JOIN Products ON ExportInvoicesItems.product_id = Products.id
        JOIN Authors ON Products.author_id = Authors.id
        JOIN ExportInvoices ON ExportInvoicesItems.invoice_id = ExportInvoices.id
        WHERE ExportInvoicesItems.invoice_id = ?
        )");
        query.addBindValue(invoiceIdCurrent);


        if (!query.exec()) {
            QMessageBox::warning(this, "Lỗi", "c" + query.lastError().text());
            return;
        }

        ui->products->setRowCount(0);
        int row = 0;
        while (query.next()) {
            QString productId = query.value(0).toString();
            QString fullTitle = query.value(1).toString();
            int quantity = query.value(2).toInt();

            ui->products->insertRow(row);
            ui->products->setItem(row, 0, new QTableWidgetItem(productId));
            ui->products->setItem(row, 1, new QTableWidgetItem(fullTitle));
            ui->products->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
            row++;


            ui->bill_num->setText(query.value(3).toString());
            ui->date->setText(query.value(4).toString());
            ui->total_quantities->setText(query.value(5).toString());
        }
    }
    if (InvoiceTypeCurrent == Retail) {
        QSqlQuery query;
        query.prepare(R"(
        SELECT
            Products.id,
            Products.title || ' - ' || Authors.name AS full_title,
            RetailInvoicesItems.quantity,
            RetailInvoices.bill_num,
            RetailInvoices.date,
            RetailInvoices.total_quanties
        FROM RetailInvoicesItems
        JOIN Products ON RetailInvoicesItems.product_id = Products.id
        JOIN Authors ON Products.author_id = Authors.id
        JOIN RetailInvoices ON RetailInvoicesItems.invoice_id = RetailInvoices.id
        WHERE RetailInvoicesItems.invoice_id = ?
        )");
        query.addBindValue(invoiceIdCurrent);


        if (!query.exec()) {
            QMessageBox::warning(this, "Lỗi", "Không thể tải chi tiết hóa đơn nhập:\n" + query.lastError().text());
            return;
        }

        ui->products->setRowCount(0);
        int row = 0;
        while (query.next()) {
            QString productId = query.value(0).toString();
            QString fullTitle = query.value(1).toString();
            int quantity = query.value(2).toInt();

            ui->products->insertRow(row);
            ui->products->setItem(row, 0, new QTableWidgetItem(productId));
            ui->products->setItem(row, 1, new QTableWidgetItem(fullTitle));
            ui->products->setItem(row, 2, new QTableWidgetItem(QString::number(quantity)));
            row++;


            ui->bill_num->setText(query.value(3).toString());
            ui->date->setText(query.value(4).toString());
            ui->total_quantities->setText(query.value(5).toString());
        }
    }
}
