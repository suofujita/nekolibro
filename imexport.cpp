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
    ui->logo_export->setFixedSize(100,100);
    ui->logo_search_export->setFixedSize(50,50);
    ui->logo_import_logs->setFixedSize(150,150);
    ui->logo_export_logs->setFixedSize(150,150);

    // Signals & slots
    /* Trang chủ nhập/ xuất hàng */
    connect(ui->import_main, &QPushButton::clicked,this,&ImExport::gotoImportInvoice);
    connect(ui->export_main, &QPushButton::clicked,this,&ImExport::gotoExportInvoice);
    connect(ui->import_logs_main, &QPushButton::clicked,this,&ImExport::gotoImportLogs);
    connect(ui->export_logs_main, &QPushButton::clicked,this,&ImExport::gotoExportLogs);
    /* Phiếu nhập hàng */
    connect(ui->save_invoice_import,&QPushButton::clicked,this,&ImExport::saveImportInvoices);
    connect(ui->add_new_category_from_import, &QPushButton::clicked,this, &ImExport::toAddNewCategory);
    connect(ui->minimum_import,&QPushButton::clicked,this,&ImExport::delayTab);
    connect(ui->back_to_main_from_import,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->import_logs_from_import,&QPushButton::clicked,this,&ImExport::viewImportLogs);
    connect(ui->remove_from_table_import,&QPushButton::clicked,this,&ImExport::removeBooksFromImportTable);
    /* Phiếu xuất hàng */
    connect(ui->save_export_invoice,&QPushButton::clicked,this,&ImExport::saveExportInvoice);
    connect(ui->minimum_export,&QPushButton::clicked,this,&ImExport::delayTab);
    connect(ui->back_to_main_from_export,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->export_logs_from_export,&QPushButton::clicked,this,&ImExport::viewExportLogs);
    connect(ui->remove_from_table_export,&QPushButton::clicked,this,&ImExport::removeBooksFromExportTable);
    /* Lịch sử nhập hàng */
    connect(ui->back_to_main_from_import_logs,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->minimum_import_logs,&QPushButton::clicked,this,&ImExport::delayTab);
    connect(ui->search_import_logs, &QPushButton::clicked, this, &ImExport::resultsImportLogs);
    connect(ui->all_import_logs, &QPushButton::clicked, this, &ImExport::loadAllImportInvoices);
    connect(ui->close_import_logs_window, &QPushButton::clicked,this,&ImExport::closeTab);
    /* Lịch sử xuất hàng */
    connect(ui->back_to_main_from_export_logs,&QPushButton::clicked,this,&ImExport::toMainImExport);
    connect(ui->minimum_export_logs,&QPushButton::clicked,this,&ImExport::delayTab);
    connect(ui->search_export_logs, &QPushButton::clicked, this, &ImExport::resultsExportLogs);
    connect(ui->all_export_logs, &QPushButton::clicked, this, &ImExport::loadAllExportInvoices);
    connect(ui->close_export_logs_window, &QPushButton::clicked,this, &ImExport::closeTab);
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
    ui->export_books->setColumnCount(3);
    ui->export_books->setHorizontalHeaderLabels({"Mã sản phẩm", "Tên sản phẩm","Số lượng"});
    ui->export_books->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerExport = ui->export_books->horizontalHeader();
    headerExport->setSectionResizeMode(1, QHeaderView::Stretch);  // kích thước ô tự động giãn theo nội dung
    headerExport->setSectionResizeMode(0, QHeaderView::Fixed);    // cố định fixed
    headerExport->setSectionResizeMode(2, QHeaderView::Fixed);    // cố định fixed
    ui->export_books->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->export_books->setColumnWidth(2, 100);
    ui->export_books->horizontalHeader()->setStretchLastSection(false);

    /* Bảng chứa lịch sử nhập hàng */
    ui->search_import_logs_result->setColumnCount(3);
    ui->search_import_logs_result->setHorizontalHeaderLabels({"Ngày", "Mã đơn hàng", "Số lượng"});
    ui->search_import_logs_result->horizontalHeader()->setStretchLastSection(true);
    QHeaderView *headerImportLogs = ui->search_import_logs_result->horizontalHeader();
    headerImportLogs->setSectionResizeMode(1, QHeaderView::Stretch);
    headerImportLogs->setSectionResizeMode(0, QHeaderView::Fixed);
    headerImportLogs->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->search_import_logs_result->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->search_import_logs_result->setColumnWidth(2, 100);
    ui->search_import_logs_result->horizontalHeader()->setStretchLastSection(false);

    /* Bảng chứa lịch sử xuất hàng */
    ui->search_export_logs_result->setColumnCount(3);
    ui->search_export_logs_result->setHorizontalHeaderLabels({"Ngày", "Mã đơn hàng", "Số lượng"});
    ui->search_export_logs_result->horizontalHeader()->setStretchLastSection(true);
    QHeaderView *headerExportLogs = ui->search_export_logs_result->horizontalHeader();
    headerExportLogs->setSectionResizeMode(1, QHeaderView::Stretch);
    headerExportLogs->setSectionResizeMode(0, QHeaderView::Fixed);
    headerExportLogs->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->search_export_logs_result->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->search_export_logs_result->setColumnWidth(2, 100);
    ui->search_export_logs_result->horizontalHeader()->setStretchLastSection(false);

    /* Kết nối với cơ sở dữ liệu */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
    }

    ui->search_product_import->setPlaceholderText("Vui lòng nhập mã ISBN, tên sản phẩm hoặc tên tác giả ...");
    connect(ui->search_product_import, &QLineEdit::textChanged,this,&ImExport::searchBooksForImport);
    ui->search_product_export->setPlaceholderText("Vui lòng nhập mã ISBN, tên sản phẩm hoặc tên tác giả ...");
    connect(ui->search_product_export, &QLineEdit::textChanged,this,&ImExport::searchBooksForExport);
    setCompleterForImport();
    setCompleterForExport();


    loadAllImportInvoices();
    loadAllExportInvoices();

    /* Thời gian thay đổi từng giây */
    time = new QTimer(this);
    connect(time, &QTimer::timeout, this, &ImExport::showTime);
    time->start(1000);

    /* Cài đặt xem chi tiết hóa đơn */
    connect(ui->search_import_logs_result,&QTableWidget::cellPressed, this, &ImExport::clickedImportBillNum);
    connect(ui->search_export_logs_result,&QTableWidget::cellPressed, this, &ImExport::clickedExportBillNum);
}

ImExport::~ImExport()
{
    delete ui;
}

void ImExport::gotoExportInvoice(){
    if(NekoLibro::role != "admin"){
        QMessageBox::information(this,"Lỗi", "Bạn không có quyền thực hiện chức năng này!");
        return;
    }
    ui->stackedWidget->setCurrentIndex(2);
    showTime();
    autoCreateExportBillNum();
}

void ImExport::gotoImportInvoice(){
    if(NekoLibro::role != "admin"){
        QMessageBox::information(this,"Lỗi", "Bạn không có quyền thực hiện chức năng này!");
        return;
    }
    ui->stackedWidget->setCurrentIndex(1);
    showTime();
    autoCreateImportBillNum();
}

void ImExport::gotoImportLogs(){
    ui->stackedWidget->setCurrentIndex(3);
    ui->start_date_import_ref->setDate(QDate::currentDate());
    ui->end_date_import_ref->setDate(QDate::currentDate());
    ui->start_date_import_ref->setCalendarPopup(true);
    ui->end_date_import_ref->setCalendarPopup(true);
}

void ImExport::gotoExportLogs(){
    ui->stackedWidget->setCurrentIndex(4);
    ui->start_date_export_ref->setDate(QDate::currentDate());
    ui->end_date_export_ref->setDate(QDate::currentDate());
    ui->start_date_export_ref->setCalendarPopup(true);
    ui->end_date_export_ref->setCalendarPopup(true);
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

void ImExport::delayTab(){
    showMinimized();
}

void ImExport::closeTab(){
    this->close();
}

void ImExport::showTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();  // Lấy ngày và giờ hiện tại
    QString dateTimeString = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");  // Định dạng ngày và giờ
    ui->date_import->setText(dateTimeString);  // Hiển thị ngày và giờ trong QLabel
    ui->date_export->setText(dateTimeString);
}

void ImExport::autoCreateImportBillNum(){
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    /* Tạo cho phiếu nhập hàng */
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

void ImExport::autoCreateExportBillNum(){
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    /* Tạo cho phiếu xuất hàng */
    QString prefix_ = "NekoLibro-XH";
    QString dateStr_ = QDate::currentDate().toString("ddMMyyyy");
    QString base_ = prefix_+ dateStr_+ "-";
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM ExportInvoices WHERE bill_num LIKE ?");
    query.addBindValue(base_ + "%");
    query.exec();
    int count_ = 0;
    if (query.next()){
        count_ = query.value(0).toInt();
    }
    QString billNum_ = QString("%1%2-%3").arg(prefix_).arg(dateStr_).arg(count_+1,4,10,QChar('0'));
    ui->bill_num_export->setText(billNum_);
}

void ImExport::viewImportLogs(){
    ui->stackedWidget->setCurrentIndex(3);
}

void ImExport::viewExportLogs(){
    ui->stackedWidget->setCurrentIndex(4);
}

void ImExport::selectedBooksForImport(QAction *action){
    QString productId = action->data().toString();

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name, Products.stock
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        WHERE Products.id = ?
    )");
    query.addBindValue(productId);

    if (query.exec() && query.next()) {
        QString title = query.value(1).toString();
        QString author = query.value(2).toString();

        // Gọi hàm đã tách riêng
        addProductForImport(productId, title, author);
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể lấy thông tin sản phẩm.");
    }

    ui->search_product_import->clear(); // Xoá sau khi chọn
}



void ImExport::setCompleterForImport()
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
    ui->search_product_import->setCompleter(completer);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, [this, suggestionToIdMap](const QString &selectedText) {
                QString id = suggestionToIdMap.value(selectedText);
                if (!id.isEmpty()) {
                    QAction *fakeAction = new QAction(this);
                    fakeAction->setData(id);
                    selectedBooksForImport(fakeAction);
                    QTimer::singleShot(0, this, [this]() {
                        ui->search_product_import->clear();
                    });
                }
            });
}

void ImExport::setCompleterForExport()
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
    ui->search_product_export->setCompleter(completer);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, [this, suggestionToIdMap](const QString &selectedText) {
                QString id = suggestionToIdMap.value(selectedText);
                if (!id.isEmpty()) {
                    QAction *fakeAction = new QAction(this);
                    fakeAction->setData(id);
                    selectedBooksForExport(fakeAction);
                    QTimer::singleShot(0, this, [this]() {
                        ui->search_product_export->clear();
                    });
                }
            });
}

void ImExport::updateTotals(){
    totalQuantityImport = 0;
    totalQuantityExport = 0;
    /* Cập nhật số lượng nhập hàng */
    for (int row = 0; row < ui->import_books->rowCount(); ++row) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->import_books->cellWidget(row, 2));
        if (!spinBox) continue;
        int quantity = spinBox->value();
        totalQuantityImport += quantity;
    }
    ui->quantity_import->setText(QString::number(totalQuantityImport));  // cập nhật lại số lượng sản phẩm

    /* Cập nhật số lượng xuất hàng */
    for (int row = 0; row < ui->export_books->rowCount(); ++row) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->export_books->cellWidget(row, 2));  // nếu ô đó không phải là spinbox kết quả trả về nullptr
        if (!spinBox) continue;  // nếu là nullptr thì bỏ qua cộng
        int quantity = spinBox->value();
        totalQuantityExport += quantity;
    }
    ui->quantity_export->setText(QString::number(totalQuantityExport));  // cập nhật lại số lượng sản phẩm
}

void ImExport::searchBooksForImport(const QString &text)
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
                selectedBooksForImport(action);
            });
        }
    } else {
        qDebug() << "Lỗi truy vấn:" << query.lastError().text();
    }
}

void ImExport::removeBooksFromImportTable(){
    int currentRow = ui->import_books->currentRow();
    if (currentRow >= 0) {
        ui->import_books->removeRow(currentRow);
        updateTotals(); // Cập nhật lại tổng sau khi xóa
    } else {
        QMessageBox::warning(this, "Chưa chọn sản phẩm", "Vui lòng chọn sản phẩm muốn xóa khỏi hóa đơn.");
    }
}

void ImExport::saveImportInvoices()
{
    if(totalQuantityImport==0){
        QMessageBox::critical(this, "Lỗi", "Vui lòng chọn ít nhất 1 sản phẩm!");
        return;
    }
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
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.addBindValue(totalQuantityImport);

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
        int invoiceId = getImportInvoiceId(billNum);
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
    updatedStockDueImport();
    db.commit();  // lưu thành công
    QMessageBox::information(this, "Thành công", "Đã lưu hóa đơn!");
    resetImportInvoiceForm();
}

int ImExport::getImportInvoiceId(const QString &numBill){
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

int ImExport::getExportInvoiceId(const QString &numBill){
    QSqlQuery query;
    query.prepare("SELECT id FROM ExportInvoices WHERE bill_num = ?");
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

void ImExport::updatedStockDueImport() {
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

void ImExport::updatedStockDueExport() {
    QSqlQuery query;

    for (int row = 0; row < ui->export_books->rowCount(); ++row) {
        // 1. Lấy id sản phẩm cột 0
        QTableWidgetItem *item = ui->export_books->item(row, 0);
        if (!item) continue;
        QString productId = item->text();

        // 2. Lấy số lượng từ QSpinBox trong cột 2
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->export_books->cellWidget(row, 2));
        if (!spinBox) continue;
        int quantity = spinBox->value();

        // 3. Cập nhật tồn kho
        query.prepare("UPDATE Products SET stock = stock - :qty WHERE id = :id");
        query.bindValue(":qty", quantity);
        query.bindValue(":id", productId);

        if (!query.exec()) {
            qDebug() << "Lỗi cập nhật tồn kho cho sách ID:" << productId << "->" << query.lastError().text();
        }
    }
}

void ImExport::resultsImportLogs() {
    QDate start_date = ui->start_date_import_ref->date();
    QDate end_date = ui->end_date_import_ref->date();


    QSqlQuery query;
    query.prepare("SELECT date, bill_num, total_products FROM ImportInvoices WHERE date >= ? AND date <= ?");
    query.addBindValue(start_date.toString("yyyy-MM-dd"));
    query.addBindValue(end_date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Lỗi truy vấn ImportInvoices:" << query.lastError().text();
        return;
    }

    ui->search_import_logs_result->setRowCount(0); // Xóa dữ liệu cũ
    int row = 0;

    while (query.next()) {
        ui->search_import_logs_result->insertRow(row);

        QString date = query.value(0).toString();
        QString billNum = query.value(1).toString();
        int totalProduct = query.value(2).toInt();

        ui->search_import_logs_result->setItem(row, 0, new QTableWidgetItem(date));
        ui->search_import_logs_result->setItem(row, 1, new QTableWidgetItem(billNum));
        ui->search_import_logs_result->setItem(row, 2, new QTableWidgetItem(QString::number(totalProduct)));

        row++;
    }
}

void ImExport::resultsExportLogs() {
    QDate start_date = ui->start_date_export_ref->date();
    QDate end_date = ui->end_date_export_ref->date();

    QSqlQuery query;
    query.prepare("SELECT date, bill_num, total_products FROM ExportInvoices WHERE date >= ? AND date <= ?");
    query.addBindValue(start_date.toString("yyyy-MM-dd"));
    query.addBindValue(end_date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Lỗi truy vấn ExportInvoices:" << query.lastError().text();
        return;
    }

    ui->search_export_logs_result->setRowCount(0); // Xóa dữ liệu cũ
    int row = 0;

    while (query.next()) {
        ui->search_export_logs_result->insertRow(row);

        QString date = query.value(0).toString();
        QString billNum = query.value(1).toString();
        int totalProduct = query.value(2).toInt();

        ui->search_export_logs_result->setItem(row, 0, new QTableWidgetItem(date));
        ui->search_export_logs_result->setItem(row, 1, new QTableWidgetItem(billNum));
        ui->search_export_logs_result->setItem(row, 2, new QTableWidgetItem(QString::number(totalProduct)));

        row++;
    }
}

void ImExport::loadAllImportInvoices() {
    QSqlQuery query("SELECT date, bill_num, total_products FROM ImportInvoices");

    ui->search_import_logs_result->setRowCount(0); // Xóa bảng cũ nếu có

    int row = 0;
    while (query.next()) {
        ui->search_import_logs_result->insertRow(row);

        ui->search_import_logs_result->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // Ngày
        ui->search_import_logs_result->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // Mã đơn
        ui->search_import_logs_result->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // Tổng sản phẩm

        row++;
    }
}

void ImExport::loadAllExportInvoices() {
    QSqlQuery query("SELECT date, bill_num, total_products FROM ExportInvoices");

    ui->search_export_logs_result->setRowCount(0); // Xóa bảng cũ nếu có

    int row = 0;
    while (query.next()) {
        ui->search_export_logs_result->insertRow(row);

        ui->search_export_logs_result->setItem(row, 0, new QTableWidgetItem(query.value(0).toString())); // Ngày
        ui->search_export_logs_result->setItem(row, 1, new QTableWidgetItem(query.value(1).toString())); // Mã đơn
        ui->search_export_logs_result->setItem(row, 2, new QTableWidgetItem(query.value(2).toString())); // Tổng sản phẩm

        row++;
    }
}

void ImExport::selectedBooksForExport(QAction *action){
    QString productId = action->data().toString();

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name, Products.stock
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        WHERE Products.id = ?
    )");
    query.addBindValue(productId);

    if (query.exec() && query.next()) {
        QString title = query.value(1).toString();
        QString author = query.value(2).toString();
        int stock = query.value(3).toInt();

        // Gọi hàm đã tách riêng
        addProductForExport(productId, title, author, stock);
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể lấy thông tin sản phẩm.");
    }

    ui->search_product_export->clear(); // Xoá sau khi chọn
}

void ImExport::removeBooksFromExportTable(){
    int currentRow = ui->export_books->currentRow();
    if (currentRow >= 0) {
        ui->export_books->removeRow(currentRow);
        updateTotals(); // Cập nhật lại tổng sau khi xóa
    } else {
        QMessageBox::warning(this, "Chưa chọn sản phẩm", "Vui lòng chọn sản phẩm muốn xóa khỏi hóa đơn.");
    }
}

void ImExport::searchBooksForExport(const QString &text)
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
                selectedBooksForExport(action);
            });
        }
    } else {
        qDebug() << "Lỗi truy vấn:" << query.lastError().text();
    }
}

void ImExport::saveExportInvoice()
{
    if(totalQuantityExport==0){
        QMessageBox::critical(this, "Lỗi", "Vui lòng chọn ít nhất 1 sản phẩm!");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QSqlQuery query(db);

    db.transaction(); // bắt đầu transaction

    // 1. Lưu vào bảng ExportInvoices
    query.prepare("INSERT INTO ExportInvoices (bill_num, date, total_products) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(ui->bill_num_export->text());
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.addBindValue(totalQuantityExport);

    if (!query.exec()) {
        qDebug() << "Error inserting invoice:" << query.lastError();
        db.rollback();
        return;
    }
    QString billNum = ui->bill_num_export->text();
    // 2. Lưu chi tiết thông tin sản phẩm trong đơn hàng
    for(int row=0;row < ui->export_books->rowCount();row++){
        QString product_id = ui->export_books->item(row, 0)->text();
        int productId = product_id.toInt();
        int invoiceId = getExportInvoiceId(billNum);
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->export_books->cellWidget(row, 2));
        int quantity = spinBox ? spinBox->value() : 1;

        QSqlQuery insertItem;
        insertItem.prepare(R"(
            INSERT INTO ExportInvoicesItems (invoice_id, product_id, quantity)
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
    updatedStockDueExport();
    db.commit();  // lưu thành công
    QMessageBox::information(this, "Thành công", "Đã lưu hóa đơn!");
    this->close();
    resetExportInvoiceForm();
}

void ImExport::clickedImportBillNum(int row, int col){
    if(col == 1) {
        ImportInvoiceDetails(row);
    }
}

void ImExport::ImportInvoiceDetails(int row)
{
    QString invoiceId ;
    QString billNum = ui->search_import_logs_result->item(row,1)->text();
    QSqlQuery query;
    query.prepare("SELECT id FROM ImportInvoices WHERE bill_num = ?");
    query.addBindValue(billNum);
    query.exec();
    if(query.next()){
        invoiceId = query.value(0).toString();
    }
    pViewInvoicesDetails = new ViewInvoicesDetails(invoiceId, InvoiceType::Import, this);
    pViewInvoicesDetails->exec();
    delete pViewInvoicesDetails;
    pViewInvoicesDetails = nullptr;
}

void ImExport::clickedExportBillNum(int row, int col){
    if(col == 1) {
        ExportInvoiceDetails(row);
    }
}

void ImExport::ExportInvoiceDetails(int row)
{
    QString invoiceId ;
    QString billNum = ui->search_export_logs_result->item(row,1)->text();
    QSqlQuery query;
    query.prepare("SELECT id FROM ExportInvoices WHERE bill_num = ?");
    query.addBindValue(billNum);
    query.exec();
    if(query.next()){
        invoiceId = query.value(0).toString();
    }
    pViewInvoicesDetails = new ViewInvoicesDetails(invoiceId, InvoiceType::Export, this);
    pViewInvoicesDetails->exec();
    delete pViewInvoicesDetails;
    pViewInvoicesDetails = nullptr;
}

void ImExport::addProductForImport(const QString &productId, const QString &name, const QString &author) {
    // Kiểm tra nếu sản phẩm đã có trong bảng
    for (int r = 0; r < ui->import_books->rowCount(); ++r) {
        QTableWidgetItem *item = ui->import_books->item(r, 0);
        if (item && item->text() == productId) {
            QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->import_books->cellWidget(r, 2));
            if (spinBox) {
                spinBox->setValue(spinBox->value() + 1);
                updateTotals();
                ui->search_product_import->clear();
                return;
            }
        }
    }

    int row = ui->import_books->rowCount();
    ui->import_books->insertRow(row);

    // Ghép tên sản phẩm với tên tác giả
    QString nameWithAuthor = name + " - " + author;

    // Cột 0: Mã sản phẩm
    ui->import_books->setItem(row, 0, new QTableWidgetItem(productId));

    // Cột 1: Tên sách + tác giả
    ui->import_books->setItem(row, 1, new QTableWidgetItem(nameWithAuthor));

    // Cột 2: QSpinBox số lượng nhập
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setMinimum(1);           // Không giới hạn tối đa
    spinBox->setMaximum(1000000);     // Giá trị lớn đủ để không bị giới hạn
    spinBox->setValue(1);
    ui->import_books->setCellWidget(row, 2, spinBox);

    // Khi thay đổi số lượng
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]() {
        updateTotals();
    });
    updateTotals();
}

void ImExport::addProductForExport(const QString &productId, const QString &name, const QString &author, int stock){
    if (stock <= 0) {
        QMessageBox::warning(this, "Hết hàng", "Sản phẩm này đã hết hàng.");
        return;
    }
    // Kiểm tra nếu sản phẩm đã có trong bảng
    for (int r = 0; r < ui->export_books->rowCount(); ++r) {
        QTableWidgetItem *item = ui->export_books->item(r, 0);
        if (item && item->text() == productId) {
            QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->export_books->cellWidget(r, 2));
            if (spinBox) {
                int currentQty = spinBox->value();
                if (currentQty + 1 > stock) {
                    QMessageBox::warning(this, "Vượt tồn kho",
                                         QString("Chỉ còn %1 sản phẩm trong kho.").arg(stock));
                    return;
                }
                spinBox->setValue(currentQty + 1);
                updateTotals();
                ui->search_product_export->clear();
                return;
            }
        }
    }

    int row = ui->export_books->rowCount();
    ui->export_books->insertRow(row);

    // Kết hợp "Tên sản phẩm" và "Tên tác giả" thành một chuỗi
    QString nameWithAuthor = name + " - " + author;

    // Thêm dữ liệu vào bảng

    ui->export_books->setItem(row, 0, new QTableWidgetItem(productId));
    ui->export_books->setItem(row, 1, new QTableWidgetItem(nameWithAuthor)); // Sử dụng nameWithAuthor

    // Thêm QSpinBox để chỉnh số lượng
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setMinimum(1);
    spinBox->setMaximum(stock);
    spinBox->setValue(1);
    ui->export_books->setCellWidget(row, 2, spinBox);

    // Khi người dùng thay đổi số lượng
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [=]() {
        updateTotals();
    });
    updateTotals();
}

void ImExport::resetImportInvoiceForm(){
    autoCreateImportBillNum();

    // Xóa dữ liệu bảng
    ui->import_books->setRowCount(0);

    // Reset tổng số lượng
    totalQuantityImport = 0;
}

void ImExport::resetExportInvoiceForm(){
    autoCreateExportBillNum();
    ui->export_books->setRowCount(0);
    totalQuantityExport = 0;
}
