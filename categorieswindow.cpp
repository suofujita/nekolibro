#include "categorieswindow.h"
#include "ui_categorieswindow.h"

#include "nekolibro.h"
CategoriesWindow::CategoriesWindow(NekoLibro *parent)
    : QDialog(static_cast<QWidget*>(parent)), ui(new Ui::CategoriesWindow), pNekoLibro(parent)
{
    ui->setupUi(this);
    qDebug() << "success";
    //pNekoLibro = parent;
    connect(ui->to_add_book, &QPushButton::clicked,this, &CategoriesWindow::toAddBook);
    connect(ui->to_main_categories,&QPushButton::clicked,this,&CategoriesWindow::toMainCategories);
    connect(ui->save_book,&QPushButton::clicked,this,&CategoriesWindow::addBook);
    connect(ui->remove_book,&QPushButton::clicked,this,&CategoriesWindow::removeBook);

    connect(ui->search,&QLineEdit::textChanged,this,&CategoriesWindow::searchByText);
    connect(ui->add_new_author, &QPushButton::clicked,this, &CategoriesWindow::addNewAuthor);
    connect(ui->add_new_type, &QPushButton::clicked,this, &CategoriesWindow::addNewCategory);

    connect(ui->excel, &QPushButton::clicked, this, &CategoriesWindow::importFromExcel);

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

    /* Hiện thị bảng danh mục sản phẩm */
    showData();
    ui->search->setPlaceholderText("Nhập tên sản phẩm hoặc tên tác giả...");
    loadComboBoxes();
    resetData();

    ui->author_edit->setPlaceholderText("Nhập tên tác giả mới...");
    ui->type_edit->setPlaceholderText("Nhập danh mục mới...");

    pSalesWindow = new SalesWindow();
}

CategoriesWindow::~CategoriesWindow()
{
        delete ui;
}

void CategoriesWindow::toAddBook()
{
    if (!ui) {
        qDebug() << "ERROR: ui is NULL!";
        return;
    }
    if (!ui->stackedWidget) {
        qDebug() << "ERROR: stackedWidget is NULL!";
        return;
    }

    int pageCount = ui->stackedWidget->count();
    qDebug() << "StackedWidget total pages: " << pageCount;

    if (pageCount <= 1) {
        qDebug() << "ERROR: Page index 1 does not exist!";
        return;
    }

    // Kiểm tra cơ sở dữ liệu nếu cần
    QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
    if (!db.isOpen()) {
        qDebug() << "ERROR: Database not open in toAddBook!";
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);
}

void CategoriesWindow::toMainCategories(){
    ui->stackedWidget->setCurrentIndex(0);
}

void CategoriesWindow::addBook(){
    QString isbn = ui->isbn_edit->text().trimmed();
    QString title = ui->title_edit->text().trimmed();
    QString author = ui->author_box->currentText().trimmed();
    QString selling_price = ui->selling_price->text().trimmed();
    QString purchase_price = ui->purchase_price->text().trimmed();
    QString type = ui->type_box->currentText().trimmed();

    if(isbn.isEmpty()||title.isEmpty() || author.isEmpty() || selling_price.isEmpty() || purchase_price.isEmpty() || type.isEmpty()){
        QMessageBox::warning(this,"Lỗi","Vui lòng điền đầy đủ thông tin");
        return;
    }
    int author_id = NekoLibro::getAuthorId(author);
    int category_id = NekoLibro::getCategoryId(type);
    if (author_id == -1 || category_id == -1) {
        QMessageBox::critical(this, "Lỗi", "Không thể lấy author/category ID");
        return;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("SELECT * FROM Products WHERE isbn = ? ");
    query.addBindValue(isbn);

    if(query.exec() && !query.next()) {
        query.prepare("INSERT INTO Products (isbn,title,author_id,purchase_price,selling_price,category_id)"
                      "VALUES( ? , ? , ?, ?, ?, ?)");
        query.addBindValue(isbn);
        query.addBindValue(title);
        query.addBindValue(author_id);
        query.addBindValue(purchase_price);
        query.addBindValue(selling_price);
        query.addBindValue(category_id);

        if(query.exec()) {
            QSqlDatabase::database().commit();
            /* Thêm thành công sau đó cập nhật CSDL */
            showData();
            resetData();
            QMessageBox::information(this,"Thêm sản phẩm","Thêm sản phẩm thành công");
            pSalesWindow->setCompleter();
        }
        else {
            QSqlDatabase::database().rollback();
            QMessageBox::critical(this, "Lỗi", "Không thể thêm sản phẩm: " + query.lastError().text());
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Sản phẩm đã tồn tại!");
    }
    query.finish();
}

void CategoriesWindow::removeBook() {
    bool found = false;
    QSqlDatabase::database().transaction();
    QSqlQuery query;

    for (int row = 0; row < modelForData->rowCount(); ++row) {
        QStandardItem *checkItem = modelForData->item(row, 0);
        if (checkItem && checkItem->checkState() == Qt::Checked) {
            found = true;
            int id = modelForData->item(row, 1)->data(Qt::UserRole).toInt();

            query.prepare("DELETE FROM Products WHERE id = ?");
            query.addBindValue(id);

            if (!query.exec()) {
                QSqlDatabase::database().rollback();
                QMessageBox::critical(this, "Lỗi", "Không thể xoá sản phẩm: " + query.lastError().text());
                return;
            }
        }
    }

    if (found) {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, "Xoá sản phẩm", "Xoá sản phẩm thành công!");
        showData(); // Cập nhật lại bảng
    } else {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn ít nhất một sản phẩm để xoá!");
    }
}

void CategoriesWindow::showData() {
    if (!modelForData) {
        modelForData = new QStandardItemModel(this);
        modelForData->setHorizontalHeaderLabels(QStringList()
                                                << " "
                                                << "ID"
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
            QString title = query.value(1).toString();
            QString author = query.value(2).toString();
            double selling_price = query.value(3).toDouble();
            double purchase_price = query.value(4).toDouble();
            QString category = query.value(5).toString();
            int stock = query.value(6).toInt();

            QStandardItem *checkItem = new QStandardItem();
            checkItem->setCheckable(true);
            checkItem->setCheckState(Qt::Unchecked);
            checkItem->setFlags(checkItem->flags() | Qt::ItemIsEditable);

            QStandardItem *idItem = new QStandardItem(QString::number(id));
            idItem->setData(id, Qt::UserRole);
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

            modelForData->setItem(row, 0, checkItem);
            modelForData->setItem(row, 1, idItem);
            modelForData->setItem(row, 2, new QStandardItem(title));
            modelForData->setItem(row, 3, new QStandardItem(author));
            modelForData->setItem(row, 4, new QStandardItem(locale.toString(selling_price, 'f', 0)));
            modelForData->setItem(row, 5, new QStandardItem(locale.toString(purchase_price, 'f', 0)));
            modelForData->setItem(row, 6, new QStandardItem(category));
            modelForData->setItem(row, 7, new QStandardItem(QString::number(stock)));

            // Không cho chỉnh sửa từ cột 2 đến 7
            for (int col = 2; col <= 7; ++col) {
                modelForData->item(row, col)->setFlags(modelForData->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
            row++;
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể lấy dữ liệu sản phẩm: " + query.lastError().text());
    }

    if (ui->categories_table->model() != modelForData) {
        ui->categories_table->setModel(modelForData);
    }

    ui->categories_table->setSortingEnabled(true);
    ui->categories_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->categories_table->resizeColumnToContents(0);
    ui->categories_table->resizeColumnToContents(1);

    // Resize cột số thứ tự và ID theo nội dung
    ui->categories_table->resizeColumnToContents(0); // checkbox
    ui->categories_table->resizeColumnToContents(1); // ID

    // Đặt cột "Tên sản phẩm" (index = 2) giãn hết phần còn lại
    ui->categories_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // Các cột còn lại resize theo nội dung
    for (int col = 3; col < modelForData->columnCount(); ++col) {
        ui->categories_table->horizontalHeader()->setSectionResizeMode(col, QHeaderView::ResizeToContents);
    }
}


void CategoriesWindow::resetData(){
    ui->isbn_edit->clear();
    ui->title_edit->clear();
    ui->author_edit->clear();
    ui->selling_price->clear();
    ui->purchase_price->clear();
    ui->title_edit->clear();
}

void CategoriesWindow::searchByText() {
    QString searchText = ui->search->text().trimmed();

    if (searchText.isEmpty()) {
        showData();  // Nếu không có từ khóa thì hiển thị toàn bộ
        return;
    }

    modelForData->removeRows(0, modelForData->rowCount());  // Xoá dữ liệu cũ

    QSqlQuery query;
    query.prepare(R"(
        SELECT
            Products.id,
            Products.title,
            Authors.name,
            Products.selling_price,
            Products.purchase_price,
            Category.name,
            Products.stock
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        JOIN Category ON Products.category_id = Category.id
        WHERE Products.title LIKE ? OR Authors.name LIKE ?
    )");
    QString keyword = "%" + searchText + "%";
    query.addBindValue(keyword);
    query.addBindValue(keyword);

    QLocale locale(QLocale::Vietnamese);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            int id = query.value(0).toInt();
            QStandardItem *idItem = new QStandardItem(QString::number(id));
            idItem->setData(id, Qt::UserRole);
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);

            QStandardItem *checkItem = new QStandardItem();
            checkItem->setCheckable(true);
            checkItem->setCheckState(Qt::Unchecked);
            checkItem->setFlags(checkItem->flags() | Qt::ItemIsEditable);

            modelForData->setItem(row, 0, checkItem);
            modelForData->setItem(row, 1, idItem);
            modelForData->setItem(row, 2, new QStandardItem(query.value(1).toString())); // Tên sản phẩm
            modelForData->setItem(row, 3, new QStandardItem(query.value(2).toString())); // Tác giả
            modelForData->setItem(row, 4, new QStandardItem(locale.toString(query.value(3).toDouble(), 'f', 0))); // Giá bán
            modelForData->setItem(row, 5, new QStandardItem(locale.toString(query.value(4).toDouble(), 'f', 0))); // Giá nhập
            modelForData->setItem(row, 6, new QStandardItem(query.value(5).toString())); // Phân loại
            modelForData->setItem(row, 7, new QStandardItem(query.value(6).toString())); // Tồn kho

            for (int col = 2; col <= 7; ++col) {
                modelForData->item(row, col)->setFlags(modelForData->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
            row++;
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể tìm kiếm: " + query.lastError().text());
    }

    ui->categories_table->setModel(modelForData);
    ui->categories_table->setSortingEnabled(true);
    ui->categories_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->categories_table->resizeColumnToContents(0);
    ui->categories_table->resizeColumnToContents(1);
}


void CategoriesWindow::addNewAuthor() {
    QString authorName = ui->author_edit->text().trimmed();
    if (authorName.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập tên tác giả!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM Authors WHERE name = ?");
    query.addBindValue(authorName);
    if (query.exec() && query.next()) {
        QMessageBox::information(this, "Thông báo", "Tác giả đã tồn tại.");
        return;
    }

    query.prepare("INSERT INTO Authors(name) VALUES(?)");
    query.addBindValue(authorName);
    if (query.exec()) {
        ui->author_box->addItem(authorName);  // Cập nhật ComboBox
        ui->author_edit->clear();
        QMessageBox::information(this, "Thành công", "Đã thêm tác giả mới.");
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể thêm tác giả: " + query.lastError().text());
    }
    loadComboBoxes();
}

void CategoriesWindow::addNewCategory() {
    QString categoryName = ui->type_edit->text().trimmed();
    if (categoryName.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập tên thể loại!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM Category WHERE name = ?");
    query.addBindValue(categoryName);
    if (query.exec() && query.next()) {
        QMessageBox::information(this, "Thông báo", "Thể loại đã tồn tại.");
        return;
    }

    query.prepare("INSERT INTO Category(name) VALUES(?)");
    query.addBindValue(categoryName);
    if (query.exec()) {
        ui->type_box->addItem(categoryName);  // Cập nhật ComboBox
        ui->type_edit->clear();
        QMessageBox::information(this, "Thành công", "Đã thêm thể loại mới.");
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể thêm thể loại: " + query.lastError().text());
    }
    loadComboBoxes();
}

void CategoriesWindow::loadComboBoxes() {
    // ComboBox Tác giả
    ui->author_box->clear();  // Xoá dữ liệu cũ
    QSqlQuery authorQuery("SELECT name FROM Authors");
    while (authorQuery.next()) {
        ui->author_box->addItem(authorQuery.value(0).toString());
    }

    // ComboBox Phân loại
    ui->type_box->clear();  // Xoá dữ liệu cũ
    QSqlQuery categoryQuery("SELECT name FROM Category");
    while (categoryQuery.next()) {
        ui->type_box->addItem(categoryQuery.value(0).toString());
    }
}

void CategoriesWindow::importFromExcel() {
    QString filePath = QFileDialog::getOpenFileName(this, "Chọn file Excel", "", "Excel (*.xlsx)");
    if (filePath.isEmpty()) return;

    QSqlDatabase db = QSqlDatabase::database();
    QXlsx::Document xlsx(filePath);
    if (!xlsx.load()) {
        QMessageBox::warning(this, "Lỗi", "Không thể mở file Excel.");
        return;
    }

    // Đếm số dòng thực tế
    int totalRows = 0;
    for (int r = 2; ; ++r) {
        if (xlsx.read(r, 2).toString().trimmed().isEmpty())
            break;
        totalRows++;
    }

    QProgressDialog progress("Đang nhập dữ liệu từ Excel...", "Hủy", 0, totalRows, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    int row = 2;
    int processed = 0;

    while (processed < totalRows) {
        progress.setValue(processed);
        if (progress.wasCanceled()) break;

        QString isbn = xlsx.read(row, 2).toString().trimmed();
        if (isbn.isEmpty()) break;

        QString name = xlsx.read(row, 3).toString().trimmed();
        QString authorName = xlsx.read(row, 4).toString().trimmed();
        double sellingPrice = xlsx.read(row, 5).toDouble();
        double purchasePrice = xlsx.read(row, 6).toDouble();
        QString categoryName = xlsx.read(row, 7).toString().trimmed();

        // Kiểm tra ISBN
        QSqlQuery checkBook(db);
        checkBook.prepare("SELECT id FROM Products WHERE isbn = ?");
        checkBook.addBindValue(isbn);
        checkBook.exec();
        if (checkBook.next()) {
            row++; processed++;
            continue;
        }

        // Tác giả
        int authorId = -1;
        QSqlQuery checkAuthor(db);
        checkAuthor.prepare("SELECT id FROM Authors WHERE name = ?");
        checkAuthor.addBindValue(authorName);
        checkAuthor.exec();
        if (checkAuthor.next()) {
            authorId = checkAuthor.value(0).toInt();
        } else {
            QSqlQuery insertAuthor(db);
            insertAuthor.prepare("INSERT OR IGNORE INTO Authors (name) VALUES (?)"); // nếu đã tồn tại
            insertAuthor.addBindValue(authorName);
            if (insertAuthor.exec())
                authorId = insertAuthor.lastInsertId().toInt();
            else {
                row++; processed++;
                continue;
            }
        }

        // Phân loại
        int categoryId = -1;
        QSqlQuery checkCategory(db);
        checkCategory.prepare("SELECT id FROM Category WHERE name = ?");
        checkCategory.addBindValue(categoryName);
        checkCategory.exec();
        if (checkCategory.next()) {
            categoryId = checkCategory.value(0).toInt();
        } else {
            QSqlQuery insertCategory(db);
            insertCategory.prepare("INSERT OR IGNORE INTO Category (name) VALUES (?)");  // nếu đã tồn tại
            insertCategory.addBindValue(categoryName);
            if (insertCategory.exec())
                categoryId = insertCategory.lastInsertId().toInt();
            else {
                row++; processed++;
                continue;
            }
        }

        // Thêm sách
        QSqlQuery insertBook(db);
        insertBook.prepare("INSERT INTO Products (isbn, title, author_id, purchase_price, selling_price, category_id) "
                           "VALUES (?, ?, ?, ?, ?, ?)");
        insertBook.addBindValue(isbn);
        insertBook.addBindValue(name);
        insertBook.addBindValue(authorId);
        insertBook.addBindValue(purchasePrice);
        insertBook.addBindValue(sellingPrice);
        insertBook.addBindValue(categoryId);
        insertBook.exec();

        row++;
        processed++;
    }

    progress.setValue(totalRows);
    QMessageBox::information(this, "Hoàn tất", "Đã hoàn tất nhập sách từ Excel.");
}
