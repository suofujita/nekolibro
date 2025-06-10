#include "productdetails.h"
#include "ui_productdetails.h"

ProductDetails::ProductDetails(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProductDetails)
{
    ui->setupUi(this);

    connect(ui->search,&QLineEdit::textChanged,this,&ProductDetails::searchByText);
    showData();

    connect(ui->products, &QTableView::doubleClicked,this, &ProductDetails::onProductClicked);

}

ProductDetails::~ProductDetails()
{
    delete ui;
}

void ProductDetails::searchByText() {
    QString searchText = ui->search->text().trimmed();

    if (searchText.isEmpty()) {
        showData();
        return;
    }

    modelForData->removeRows(0, modelForData->rowCount());

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            Products.id,
            Products.isbn,
            Products.title,
            Authors.name,
            Products.selling_price,
            Products.purchase_price,
            Category.name,
            Products.stock
        FROM Products
        JOIN Authors  ON Products.author_id   = Authors.id
        JOIN Category ON Products.category_id = Category.id
        WHERE   LOWER(Products.title) LIKE LOWER(?)
            OR LOWER(Authors.name)  LIKE LOWER(?)
            OR       Products.isbn  LIKE ? )");

    QString keyword = "%" + searchText + "%";
    query.addBindValue(keyword);  // title
    query.addBindValue(keyword);  // author
    query.addBindValue(keyword);  // isbn

    QLocale vn(QLocale::Vietnamese);

    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            int id = query.value(0).toInt();
            QStandardItem *idItem = new QStandardItem(QString::number(id));
            idItem->setData(id, Qt::UserRole);
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
            modelForData->setItem(row, 0, idItem);
            modelForData->setItem(row, 1, new QStandardItem(query.value(1).toString()));
            modelForData->setItem(row, 2, new QStandardItem(query.value(2).toString()));  // Tên sách
            modelForData->setItem(row, 3, new QStandardItem(query.value(3).toString()));  // Tác giả
            modelForData->setItem(row, 4, new QStandardItem(vn.toString(query.value(4).toDouble(), 'f', 0)));  // Giá bán
            modelForData->setItem(row, 5, new QStandardItem(vn.toString(query.value(5).toDouble(), 'f', 0)));  // Giá nhập
            modelForData->setItem(row, 6, new QStandardItem(query.value(6).toString()));                       // Phân loại
            modelForData->setItem(row, 7, new QStandardItem(query.value(7).toString()));                       // Tồn kho

            /* Khoá chỉnh sửa */
            for (int col = 0; col <= 7; ++col) {
                modelForData->item(row, col)->setFlags(modelForData->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
            ++row;
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể tìm kiếm: " + query.lastError().text());
    }

    ui->products->setModel(modelForData);
    ui->products->setSortingEnabled(true);
    ui->products->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->products->resizeColumnToContents(0);
    ui->products->resizeColumnToContents(1);   // ISBN
}

void ProductDetails::showData() {
    if (!modelForData) {
        modelForData = new QStandardItemModel(this);
        modelForData->setHorizontalHeaderLabels(QStringList()
                                                << "ID"
                                                << "ISBN"
                                                << "Tên sản phẩm"
                                                << "Tác giả"
                                                << "Giá bán"
                                                << "Giá nhập"
                                                << "Phân loại"
                                                << "Tồn kho");
    } else {
        modelForData->removeRows(0, modelForData->rowCount());
    }

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            Products.id,
            Products.isbn,
            Products.title,
            Authors.name,
            Products.selling_price,
            Products.purchase_price,
            Category.name,
            Products.stock
        FROM Products
        LEFT JOIN Authors ON Products.author_id = Authors.id
        LEFT JOIN Category ON Products.category_id = Category.id
    )");

    QLocale locale(QLocale::Vietnamese);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            int id = query.value(0).toInt();
            QString isbn = query.value(1).toString();
            QString title = query.value(2).toString();
            QString author = query.value(3).toString();
            double selling_price = query.value(4).toDouble();
            double purchase_price = query.value(5).toDouble();
            QString category = query.value(6).toString();
            int stock = query.value(7).toInt();

            QStandardItem *idItem = new QStandardItem(QString::number(id));
            idItem->setData(id, Qt::UserRole);
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

            modelForData->setItem(row, 0, idItem);
            modelForData->setItem(row, 1, new QStandardItem(isbn));
            modelForData->setItem(row, 2, new QStandardItem(title));
            modelForData->setItem(row, 3, new QStandardItem(author));
            modelForData->setItem(row, 4, new QStandardItem(locale.toString(selling_price, 'f', 0)));
            modelForData->setItem(row, 5, new QStandardItem(locale.toString(purchase_price, 'f', 0)));
            modelForData->setItem(row, 6, new QStandardItem(category));
            modelForData->setItem(row, 7, new QStandardItem(QString::number(stock)));

            // Không cho chỉnh sửa từ cột 0 đến 7
            for (int col = 0; col <= 7; ++col) {
                modelForData->item(row, col)->setFlags(modelForData->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }

            ++row;
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể lấy dữ liệu sản phẩm: " + query.lastError().text());
    }

    if (ui->products->model() != modelForData) {
        ui->products->setModel(modelForData);
    }

    ui->products->setSortingEnabled(true);
    ui->products->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // Resize cột ID và ISBN theo nội dung
    ui->products->resizeColumnToContents(0); // ID
    ui->products->resizeColumnToContents(1); // ISBN

    // Đặt cột "Tên sản phẩm" giãn hết phần còn lại
    ui->products->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // Các cột còn lại resize theo nội dung
    for (int col = 3; col < modelForData->columnCount(); ++col) {
        ui->products->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }
}

void ProductDetails::onProductClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    int row = index.row();
    QString id     = modelForData->item(row, 0)->text();  // ID
    QString title  = modelForData->item(row, 2)->text();  // Tên sản phẩm
    QString author = modelForData->item(row, 3)->text();  // Tác giả
    QLocale locale(QLocale::Vietnamese);
    double price   = locale.toDouble(modelForData->item(row, 4)->text()); // giá bán
    int stock      = modelForData->item(row, 7)->text().toInt(); // Tồn kho (cột 5)
    emit productSelected(id, title, author, price, stock);
    this->close(); // Đóng cửa sổ chi tiết sau khi chọn
}


