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
    /* Thiết lập Qcompleter */
    setupCompleter();
    /* Hiện thị bảng danh mục sản phẩm */
    showData();

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

    QStringList categoriesList = pNekoLibro->getCategoriesList();

    QString name = ui->name_edit->text();
    QString author = ui->author_edit->text();
    QString out_price = ui->out_price_edit->text();
    QString in_price = ui->in_price_edit->text();
    QString type = ui->type->text();

    //out_price.replace(".", "");
    //in_price.replace(".", "");

    if(name.isEmpty() || author.isEmpty() || out_price.isEmpty() || in_price.isEmpty() || type.isEmpty()){
        QMessageBox::warning(this,"Lỗi","Vui lòng điền đầy đủ thông tin");
        return;
    }
    QSqlDatabase::database().transaction();
    QSqlQuery query;
    query.prepare("SELECT * FROM Categories WHERE `Tên sản phẩm` = ? AND `Tên tác giả` = ?");
    query.addBindValue(name);
    query.addBindValue(author);

    if(query.exec() && !query.next()) {
        query.prepare("INSERT INTO Categories(`Tên sản phẩm`,`Tên tác giả`,`Giá bán`,`Giá nhập`,`Phân loại`)"
                      "VALUES( ? , ? , ?, ?, ?)");
        query.addBindValue(name);
        query.addBindValue(author);
        query.addBindValue(out_price);
        query.addBindValue(in_price);
        query.addBindValue(type);

        if(query.exec()) {
            QSqlDatabase::database().commit();
            /* Thêm thành công sau đó cập nhật CSDL */
           // model1->select();
            showData();
            resetData();
            QMessageBox::information(this,"Thêm sản phẩm","Thêm sản phẩm thành công");
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

    // Duyệt qua tất cả các dòng trong model
    for (int row = 0; row < modelForData->rowCount(); ++row) {
        QStandardItem *checkItem = modelForData->item(row, 0);
        if (checkItem && checkItem->checkState() == Qt::Checked) {
            found = true;

            int id = modelForData->data(modelForData->index(row, 1)).toInt(); // Lấy ID của sản phẩm

            query.prepare("DELETE FROM Categories WHERE ID = ?");
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



void CategoriesWindow::setupCompleter() {
    /* Kiểm tra kết nối CSDL */
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Lỗi", "Không thể kết nối với cơ sở dữ liệu!");
        return;
    }

    QStringList nameList;    /* Lấy danh sách tên sản phẩm từ CSDL */
    QStringList authorList;  /* Lấy danh sách tác giả từ CSDL */
    QStringList typeList;     /* Lấy phân loại từ CSDL */

    QSqlQuery query;

    // Lấy danh sách tên sản phẩm
    query.prepare("SELECT DISTINCT `Tên sản phẩm` FROM Categories");
    if (query.exec()) {
        while (query.next()) {
            nameList << query.value(0).toString();
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể truy vấn cơ sở dữ liệu: " + query.lastError().text());
    }

    // Lấy danh sách tên tác giả
    query.prepare("SELECT DISTINCT `Tên tác giả` FROM Categories");
    if (query.exec()) {
        while (query.next()) {
            authorList << query.value(0).toString();
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể truy vấn cơ sở dữ liệu: " + query.lastError().text());
    }

    // Lấy danh sách phân loại
    query.prepare("SELECT DISTINCT `Phân loại` FROM Categories");
    if (query.exec()) {
        while (query.next()) {
            typeList << query.value(0).toString();
        }
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể truy vấn cơ sở dữ liệu: " + query.lastError().text());
    }

    // ===> Thiết lập QCompleter cho tên sản phẩm
    auto *nameModel = new QStringListModel(nameList, this);
    auto *nameCompleter = new QCompleter(nameModel, this);
    nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    nameCompleter->setFilterMode(Qt::MatchContains);
    ui->name_edit->setCompleter(nameCompleter);

    // ===> Thiết lập QCompleter cho tên tác giả
    auto *authorModel = new QStringListModel(authorList, this);
    auto *authorCompleter = new QCompleter(authorModel, this);
    authorCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    authorCompleter->setFilterMode(Qt::MatchContains);
    ui->author_edit->setCompleter(authorCompleter);

    // ===> Thiết lập QCompleter cho phân loại
    auto *typeModel = new QStringListModel(typeList, this);
    auto *typeCompleter = new QCompleter(typeModel, this);
    typeCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    typeCompleter->setFilterMode(Qt::MatchContains);
    ui->type->setCompleter(typeCompleter);
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
                                          << "Phân loại");
    } else {
        modelForData->removeRows(0, modelForData->rowCount());  // xóa tất cả dữ liệu trong mô hình
    }

    QSqlQuery query;
    query.prepare("SELECT `ID`, `Tên sản phẩm`, `Tên tác giả`, `Giá bán`, `Giá nhập`, `Phân loại` FROM Categories");
    QLocale locale = QLocale(QLocale::Vietnamese);
    if (query.exec()) {
        int row = 0;
        while (query.next()) {  // Lặp qua tất cả các dòng kết quả
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
            modelForData->setItem(row, 2, new QStandardItem(query.value(1).toString()));
            modelForData->setItem(row, 3, new QStandardItem(query.value(2).toString()));
            modelForData->setItem(row, 4, new QStandardItem(locale.toString(query.value(3).toDouble(), 'f', 0))); // Giá bán có dấu chấm
            modelForData->setItem(row, 5, new QStandardItem(locale.toString(query.value(4).toDouble(), 'f', 0))); // Giá nhập có dấu chấm
            modelForData->setItem(row, 6, new QStandardItem(query.value(5).toString()));

            // Không cho phép chỉnh sửa các ô dữ liệu
            for (int col = 2; col <= 6; ++col) {
                modelForData->item(row, col)->setFlags(modelForData->item(row, col)->flags() & ~Qt::ItemIsEditable);
            }
            row++;
        }
    }
    // Kết nối model với bảng (nếu chưa kết nối)
    if (ui->categories_table->model() != modelForData) {
        ui->categories_table->setModel(modelForData);
    }
    // Cho phép sắp xếp theo cột
    ui->categories_table->setSortingEnabled(true);
    /* Design */
    //Căn giữa tiêu đề cột
    ui->categories_table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->categories_table->resizeColumnToContents(0);
    ui->categories_table->resizeColumnToContents(1);
}

void CategoriesWindow::resetData(){
    ui->author_edit->clear();
    ui->out_price_edit->clear();
    ui->in_price_edit->clear();
    ui->name_edit->clear();
    ui->type->clear();
}

void CategoriesWindow::searchByText() {
    QString searchText = ui->search->text();
    /* case 1 */
    if (searchText.isEmpty()) {
        // Nếu không có từ khóa tìm kiếm, hiển thị toàn bộ dữ liệu
        showData();
    }
    /* case 2 */
    else {
        modelForData->removeRows(0, modelForData->rowCount());  // Xóa tất cả dữ liệu trong mô hình trước

        QSqlQuery query;
        query.prepare("SELECT `ID`, `Tên sản phẩm`, `Tên tác giả`, `Giá bán`, `Giá nhập`, `Phân loại` FROM Categories WHERE `Tên sản phẩm` LIKE ? OR `Tên tác giả` LIKE ?");
        query.addBindValue("%" + searchText + "%");
        query.addBindValue("%" + searchText + "%");

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
                modelForData->setItem(row, 2, new QStandardItem(query.value(1).toString()));
                modelForData->setItem(row, 3, new QStandardItem(query.value(2).toString()));
                modelForData->setItem(row, 4, new QStandardItem(query.value(3).toString()));
                modelForData->setItem(row, 5, new QStandardItem(query.value(4).toString()));
                modelForData->setItem(row, 6, new QStandardItem(query.value(5).toString()));

                row++;
            }
        }

        // Hiển thị dữ liệu đã lọc
        ui->categories_table->setModel(modelForData);
    }
}



