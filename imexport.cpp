#include "imexport.h"
#include "ui_imexport.h"

ImExport::ImExport(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImExport)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/cat.png"));
    setWindowTitle("Xuất/Nhập Hàng - Neko Libro");
    ui->logo_im_export->setFixedSize(135,135);
    ui->import_main->setFixedSize(200,200);
    ui->export_main->setFixedSize(200,200);
    ui->import_logs_main->setFixedSize(200,200);
    ui->export_logs_main->setFixedSize(200,200);
    ui->logo_import->setFixedSize(100,100);
    ui->logo_search->setFixedSize(50,50);

    /* Trang chủ nhập/ xuất hàng */
    connect(ui->import_main, &QPushButton::clicked,this,&ImExport::gotoImportInvoice);
    connect(ui->export_main, &QPushButton::clicked,this,&ImExport::gotoExportInvoice);
    connect(ui->import_logs_main, &QPushButton::clicked,this,&ImExport::gotoImportLogs);
    connect(ui->export_logs_main, &QPushButton::clicked,this,&ImExport::gotoExportLogs);
    /* Phiếu nhập hàng */
    connect(ui->save_invoice_import,&QPushButton::clicked,this,&ImExport::saveImportInvoices);
    connect(ui->add_new_category_from_import, &QPushButton::clicked,this, &ImExport::toAddNewCategory);
    connect(ui->minimum_import,&QPushButton::clicked,this,&ImExport::delayTabImport);
    connect(ui->back_to_main_from_import,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->import_logs_from_import,&QPushButton::clicked,this,&ImExport::viewImportLogs);
    /* Phiếu xuất hàng */

    /* Lịch sử nhập hàng */

    /* Lịch sử xuất hàng */

    /* Bảng chứa sản phẩm nhập */
    ui->import_books->setColumnCount(3);
    ui->import_books->setHorizontalHeaderLabels({"Mã sản phẩm", "Tên sản phẩm","Số lượng"});
    ui->import_books->horizontalHeader()->setStretchLastSection(true);
    QHeaderView *header = ui->import_books->horizontalHeader();
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->import_books->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->import_books->setColumnWidth(2, 100);
    ui->import_books->horizontalHeader()->setStretchLastSection(false);
    /* Bảng chứa sản phẩm xuất */
    /* Kết nối với cơ sở dữ liệu */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
    }
    ui->stackedWidget->setCurrentIndex(0);
    ui->search_product->setPlaceholderText("Vui lòng nhập mã ISBN, tên sản phẩm hoặc tên tác giả ...");
    connect(ui->search_product, &QLineEdit::textChanged,this,&ImExport::searchBooks);
    setCompleter();
}

ImExport::~ImExport()
{
    delete ui;
}

void ImExport::gotoExportInvoice(){
    ui->stackedWidget->setCurrentIndex(2);
}

void ImExport::gotoImportInvoice(){
    ui->stackedWidget->setCurrentIndex(1);
    showTime();
    autoCreateBillNum();
}

void ImExport::gotoImportLogs(){
    ui->stackedWidget->setCurrentIndex(3);
}

void ImExport::gotoExportLogs(){
    ui->stackedWidget->setCurrentIndex(4);
}

void ImExport::toMainImExport(){
    ui->stackedWidget->setCurrentIndex(0);
}

void ImExport::toAddNewCategory(){
    if (!pCategoriesWindow) {
        pCategoriesWindow = new CategoriesWindow();

        connect(pCategoriesWindow, &QObject::destroyed, [this]() {
            pCategoriesWindow = nullptr;
        });

        // Chỉ gọi toAddBook sau khi show xong
        QTimer::singleShot(100, this, [this]() {
            if (pCategoriesWindow) {
                QMetaObject::invokeMethod(pCategoriesWindow, "toAddBook", Qt::QueuedConnection);
            }
        });

        pCategoriesWindow->show();
    } else {
        pCategoriesWindow->show();
        pCategoriesWindow->raise();
        pCategoriesWindow->activateWindow();

        // Không cần delay nếu cửa sổ đã khởi tạo đầy đủ
        pCategoriesWindow->toAddBook();
    }
}
void ImExport::delayTabImport(){
    showMinimized();
}
void ImExport::showTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();  // Lấy ngày và giờ hiện tại
    QString dateTimeString = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");  // Định dạng ngày và giờ
    ui->date_import->setText(dateTimeString);  // Hiển thị ngày và giờ trong QLabel
}

void ImExport::autoCreateBillNum(){
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QString prefix = "NekoLibro-NH";
    QString dateStr = QDate::currentDate().toString("ddMMyyyy");
    QString base = prefix+dateStr+"-";
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM ImportInvoices WHERE bill_num LIKE ?");
    query.addBindValue(base + "%");
    query.exec();
    int count = 0;
    if(query.next()){
        count = query.value(0).toInt();
    }
    QString billNum = QString("%1%2-%3").arg(prefix).arg(dateStr).arg(count+1,4,10,QChar('0'));
    ui->bill_num_import->setText(billNum);
}

void ImExport::viewImportLogs(){
    ui->stackedWidget->setCurrentIndex(2);
}

void ImExport::selectedBooks(QAction *action){
    QString productId = action->data().toString();

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        WHERE Products.id = ?
    )");
    query.addBindValue(productId);

    if (query.exec() && query.next()) {
        QString name = query.value(1).toString();
        QString author = query.value(2).toString();

        int row = ui->import_books->rowCount();
        ui->import_books->insertRow(row);

        // Kết hợp "Tên sản phẩm" và "Tên tác giả" thành một chuỗi
        QString nameWithAuthor = name + " - " + author;

        // Thêm dữ liệu vào bảng
        ui->import_books->setItem(row, 0, new QTableWidgetItem(productId));
        ui->import_books->setItem(row, 1, new QTableWidgetItem(nameWithAuthor)); // Sử dụng nameWithAuthor

        // Thêm QSpinBox để chỉnh số lượng
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMinimum(1);
        spinBox->setMaximum(9999);
        spinBox->setValue(1);
        ui->import_books->setCellWidget(row, 2, spinBox);

        // Khi người dùng thay đổi số lượng
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]() {
            updateTotals();
        });
    }
    ui->search_product->clear(); // Clear sau khi chọn
    updateTotals();
}

void ImExport::setCompleter()
{
    QStringList suggestions;
    QMap<QString, QString> suggestionToIdMap;

    QSqlQuery query;
    QString sql = R"(
        SELECT Products.id, Products.title, Authors.name, Products.isbn
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
    )";

    if (!query.exec(sql)) {
        QMessageBox::warning(this, "Lỗi", "Không thể tải dữ liệu để tạo gợi ý: " + query.lastError().text());
        return;
    }

    while (query.next()) {
        QString id = query.value(0).toString();
        QString title = query.value(1).toString();
        QString author = query.value(2).toString();
        QString isbn = query.value(3).toString();

        // Chuỗi hiển thị gợi ý: "Mã ISBN - Tên sản phẩm - Tên tác giả"
        QString displayText = isbn + " - " + title + " - " + author;

        // Thêm gợi ý chỉ khi chưa có để tránh trùng lặp
        if (!suggestions.contains(displayText)) {
            suggestions << displayText;
            suggestionToIdMap[displayText] = id;
        }
    }

    QCompleter *completer = new QCompleter(suggestions, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->search_product->setCompleter(completer);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, [this, suggestionToIdMap](const QString &selectedText) {
                QString id = suggestionToIdMap.value(selectedText);
                if (!id.isEmpty()) {
                    QAction *fakeAction = new QAction(this);
                    fakeAction->setData(id);
                    selectedBooks(fakeAction);
                    QTimer::singleShot(0, this, [this]() {
                        ui->search_product->clear();
                    });
                }
            });
}

void ImExport::updateTotals(){
    totalQuantity = 0;

    for (int row = 0; row < ui->import_books->rowCount(); ++row) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->import_books->cellWidget(row, 2));
        if (!spinBox) continue;

        int quantity = spinBox->value();
        totalQuantity += quantity;
    }
  ui->quantity->setText(QString::number(totalQuantity));  // cập nhật lại số lượng sản phẩm
}

void ImExport::searchBooks(const QString &text)
{
    if (text.trimmed().isEmpty())
        return;

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        WHERE Products.title LIKE ?
              OR Authors.name LIKE ?
              OR Products.isbn LIKE ?
    )");
    QString likeText = "%" + text + "%";
    query.addBindValue(likeText);
    query.addBindValue(likeText);
    query.addBindValue(likeText);

    if (query.exec()) {
        while (query.next()) {
            QString id = query.value(0).toString();
            QString title = query.value(1).toString();
            QString author = query.value(2).toString();

            QString displayText = title + " - " + author;

            QAction *action = new QAction(displayText, this);
            action->setData(id);
            connect(action, &QAction::triggered, [this, action]() {
                selectedBooks(action);
            });
        }
    } else {
        qDebug() << "Lỗi truy vấn:" << query.lastError().text();
    }
}

void ImExport::removeBooksFromImportTable(int row){
    if(row<0 || row > ui->import_books->rowCount()) {
        return ;
    }
    ui->import_books->removeRow(row);
    updateTotals();
}

void ImExport::saveImportInvoices()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QSqlQuery query(db);

    db.transaction(); // bắt đầu transaction

    // 1. Lưu vào bảng ImportInvoices
    query.prepare("INSERT INTO ImportInvoices (bill_num, date, total_products) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(ui->bill_num_import->text());
    query.addBindValue(QDate::currentDate().toString("dd-MM-yyyy"));
    query.addBindValue(totalQuantity);

    if (!query.exec()) {
        qDebug() << "Error inserting invoice:" << query.lastError();
        db.rollback();
        return;
    }
    QString billNum = ui->bill_num_import->text();
    // 2. Lưu chi tiết thông tin sản phẩm trong đơn hàng
    for(int row=0;row < ui->import_books->rowCount();row++){
        QString product_id = ui->import_books->item(row, 0)->text();
        int productId = product_id.toInt();
        int invoiceId = getInvoiceId(billNum);
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->import_books->cellWidget(row, 2));
        int quantity = spinBox ? spinBox->value() : 1;

        QSqlQuery insertItem;
        insertItem.prepare(R"(
            INSERT INTO ImportInvoicesItems (invoice_id, product_id, quantity)
            VALUES (?, ?, ?)
        )");
        insertItem.addBindValue(invoiceId);
        insertItem.addBindValue(productId);
        insertItem.addBindValue(quantity);

        if (!insertItem.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Lỗi", "Không thể lưu chi tiết hóa đơn: " + insertItem.lastError().text());
            return;
        }
    }

    // 3. Cập nhật tồn kho
    updatedStock();
    db.commit();  // lưu thành công
    QMessageBox::information(this, "Thành công", "Đã lưu hóa đơn!");
    this->close();
}

int ImExport::getInvoiceId(const QString &numBill){
    QSqlQuery query;
    query.prepare("SELECT id FROM ImportInvoices WHERE bill_num = ?");
    query.addBindValue(numBill);

    if (!query.exec()) {
        qWarning() << "Lỗi truy vấn lấy invoice_id:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Không tìm thấy hóa đơn với bill_num =" << numBill;
        return -1;
    }
}

void ImExport::updatedStock() {
    QSqlQuery query;

    for (int row = 0; row < ui->import_books->rowCount(); ++row) {
        QTableWidgetItem *item = ui->import_books->item(row, 0);
        if (!item) continue;
        QString productId = item->text();

        // Lấy số lượng từ QSpinBox trong cột 2
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->import_books->cellWidget(row, 2));
        if (!spinBox) continue;
        int quantity = spinBox->value();

        query.prepare("UPDATE Products SET stock = stock + :qty WHERE id = :id");
        query.bindValue(":qty", quantity);
        query.bindValue(":id", productId);

        if (!query.exec()) {
            qDebug() << "Lỗi cập nhật tồn kho cho sách ID:" << productId << "->" << query.lastError().text();
        }
    }
}


