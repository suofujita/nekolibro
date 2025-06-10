#include "saleswindow.h"
#include "ui_saleswindow.h"
#include "nekolibro.h"
#include "productdetails.h"

SalesWindow::SalesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SalesWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/cat.png"));
    setWindowTitle("Bán hàng - Neko Libro");

    /* Thêm phóng to/thu nhỏ cho dialog + wait */
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    /* Bảng chứa sản phẩm */
    ui->products->setColumnCount(5);
    ui->products->setHorizontalHeaderLabels({"Mã sản phẩm", "Tên sản phẩm", "Giá bán", "Số lượng", "Thành tiền"});
    ui->products->horizontalHeader()->setStretchLastSection(true);
    // Thiết kế các cột hiện thị dữ liệu hợp lý
    QHeaderView *header = ui->products->horizontalHeader();
    // Cột 1: Tên sản phẩm — cho co giãn theo cửa sổ
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    // Cột 0, 2, 3, 4: giữ cố định
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    header->setSectionResizeMode(4, QHeaderView::Fixed);
    // Thiết lập chiều rộng cố định cho các cột không co giãn
    // chú ý không cho chỉnh sửa các cột để không bị mess dữ liệu
    ui->products->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->products->setColumnWidth(2, 100); // Giá bán
    ui->products->setColumnWidth(3, 80);  // Số lượng
    ui->products->setColumnWidth(4, 120); // Thành tiền
    // Không tự kéo giãn cột cuối
    ui->products->horizontalHeader()->setStretchLastSection(false);

    /* Chỉ mục cho người dùng nhập */
    ui->search->setPlaceholderText("Nhập mã ISBN, tên sản phẩm hoặc tên tác giả...");

    connect(ui->search, &QLineEdit::textChanged, this, &SalesWindow::searchBooks);

    setCompleter();

    time = new QTimer(this);
    connect(time, &QTimer::timeout, this, &SalesWindow::showTime);
    time->start(1000);
    showTime();
    showFullName();
    ui->label->setFixedSize(30,30);
    ui->logo->setFixedSize(100,100);

    /* Muốn khi nhập thì dữ liệu nhậo định dạng ví 10000 -> 10.000)*/
    bool *formatting = new bool(false); // Cờ để tránh lặp vô hạn

    connect(ui->money_customer, &QLineEdit::textChanged, this, [=](const QString &text) {
        if (!*formatting) {
            moneyReturn(text);  // Gọi lại tính tiền thối nếu đang không định dạng
        }
    });

    connect(ui->money_customer, &QLineEdit::editingFinished, this, [=]() {
        *formatting = true;  // Đánh dấu là đang định dạng

        QString rawText = ui->money_customer->text();
        rawText.remove('.'); // Bỏ dấu chấm
        bool ok;
        double number = rawText.toDouble(&ok);

        if (ok) {
            QLocale locale(QLocale::Vietnamese);
            QString formatted = locale.toString(number, 'f', 0);
            ui->money_customer->setText(formatted); // Định dạng lại
            //moneyReturn(QString::number(number));   // Gọi lại cập nhật tiền thối
            moneyReturn(rawText);
        }

        *formatting = false; // Bỏ cờ định dạng
    });

    connect(ui->cancel, &QPushButton::clicked,this, &SalesWindow::cancelBill);
    connect(ui->save_bill, &QPushButton::clicked,this,&SalesWindow::saveBill);
    connect(ui->remove, &QPushButton::clicked,this, &SalesWindow::removeProductFromBill);
    connect(ui->products, &QTableWidget::cellPressed,this, &SalesWindow::loadStock);
    connect(ui->add,&QPushButton::clicked,this,&SalesWindow::addClicked);
    autoCreateBillNum();
}
SalesWindow::~SalesWindow()
{
    delete ui;
}

void SalesWindow::searchBooks(const QString &text)
{
    if (text.trimmed().isEmpty())
        return;

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name, Products.selling_price
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

void SalesWindow::selectedBooks(QAction *action)
{
    QString productId = action->data().toString();

    QSqlQuery query;
    query.prepare(R"(
        SELECT Products.id, Products.title, Authors.name, Products.selling_price, Products.stock
        FROM Products
        JOIN Authors ON Products.author_id = Authors.id
        WHERE Products.id = ?
    )");
    query.addBindValue(productId);

    if (query.exec() && query.next()) {
        QString title = query.value(1).toString();
        QString author = query.value(2).toString();
        double price = query.value(3).toDouble();
        int stock = query.value(4).toInt();

        // Gọi hàm đã tách riêng
        addProductToTable(productId, title, author, price, stock);
    } else {
        QMessageBox::warning(this, "Lỗi", "Không thể lấy thông tin sản phẩm.");
    }

    ui->search->clear(); // Xoá sau khi chọn
    updateTotals();
}


void SalesWindow::updateTotals()
{
    totalQuantity = 0;
    totalPrice = 0.0;

    for (int row = 0; row < ui->products->rowCount(); ++row) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->products->cellWidget(row, 3));
        if (!spinBox) continue;

        int quantity = spinBox->value();
        totalQuantity += quantity;

        QTableWidgetItem *priceItem = ui->products->item(row, 2);
        double price = priceItem->data(Qt::UserRole).toDouble();
        totalPrice += price * quantity;
    }

    QLocale locale = QLocale::Vietnamese;
    QString formattedTotal = locale.toString(totalPrice, 'f', 0);

    ui->num_book->setText(QString::number(totalQuantity));  // cập nhật lại số lượng sản phẩm
    ui->total->setText(formattedTotal);                     // cập nhật lại tổng hóa đơn

    // Sau khi cập nhật tổng tiền → tính lại tiền thối
    moneyReturn(ui->money_customer->text());
}


void SalesWindow::showTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();  // Lấy ngày và giờ hiện tại
    QString dateTimeString = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");  // Định dạng ngày và giờ
    ui->date_time->setText(dateTimeString);  // Hiển thị ngày và giờ trong QLabel
}


void SalesWindow::showFullName(){
    QString currentUser = NekoLibro::currentUser;
    QSqlQuery query;
    query.prepare("SELECT id FROM AccountUsers WHERE username = ?");
    query.addBindValue(currentUser);
    if(query.exec() && query.next()){
        currentUserId = query.value(0).toInt();
    }

    query.prepare("SELECT fullname FROM AccountUsers WHERE id = ?");
    query.addBindValue(currentUserId);
    if(query.exec() && query.next()){
        currentFullName = query.value(0).toString();
    }
    ui->name->setText(currentFullName);
}

void SalesWindow::moneyReturn(const QString &text)
{
    QString cleanedText = text;
    cleanedText.remove('.');               // Xoá dấu chấm ngăn cách hàng nghìn
    cleanedText = cleanedText.trimmed();   // Xoá khoảng trắng đầu/cuối

    bool ok = false;
    double paidAmount = QLocale::c().toDouble(cleanedText, &ok);

    if (!ok) {
        ui->money_return->setText("<b>Không hợp lệ</b>");
        ui->money_return->setStyleSheet("color: red;");
        return;
    }

    double change = paidAmount - totalPrice;
    QLocale locale = QLocale::Vietnamese;
    QString formattedChange = locale.toString(change, 'f', 0);
    QString result = QString("<b>%1</b>").arg(formattedChange);

    if (change < 0) {
        ui->money_return->setText(result);
        ui->money_return->setStyleSheet("font-weight: bold; color: red;");
    } else if (change > 0) {
        ui->money_return->setText(result);
        ui->money_return->setStyleSheet("font-weight: bold; color: green;");
    } else {
        ui->money_return->setText(result);
        ui->money_return->setStyleSheet("font-weight: bold; color: black;");
    }
}

void SalesWindow::cancelBill(){
    showMinimized();// thu nhỏ không đóng
}

void SalesWindow::autoCreateBillNum(){
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QString prefix = "NekoLibro-GD";
    QString dateStr = QDate::currentDate().toString("ddMMyyyy");
    QString base = prefix+dateStr+"-";
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM RetailInvoices WHERE bill_num LIKE ?");
    query.addBindValue(base + "%");
    query.exec();
    int count = 0;
    if(query.next()){
        count = query.value(0).toInt();
    }
    QString billNum = QString("%1%2-%3").arg(prefix).arg(dateStr).arg(count+1,4,10,QChar('0'));
    ui->bill_num->setText(billNum);
}
// Du lieu dau vao
// du lieu dau ra cho nhung bang nao

void SalesWindow::saveBill()
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QSqlQuery query(db);

    db.transaction(); // bắt đầu transaction

    // 1. Lưu vào bảng Invoices
    query.prepare("INSERT INTO RetailInvoices (bill_num, date, total_quanties, total_bill, user_id) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(ui->bill_num->text());
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.addBindValue(totalQuantity);
    query.addBindValue(totalPrice);
    query.addBindValue(currentUserId);

    if (!query.exec()) {
        qDebug() << "Error inserting invoice:" << query.lastError();
        db.rollback();
        return;
    }
    QString billNum = ui->bill_num->text();
    // 2. Lưu chi tiết thông tin sản phẩm trong đơn hàng
    for(int row=0;row < ui->products->rowCount();row++){
        QString product_id = ui->products->item(row, 0)->text();
        int productId = product_id.toInt();
        int invoiceId = getInvoiceId(billNum);
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->products->cellWidget(row, 3));
        int quantity = spinBox ? spinBox->value() : 1;

        QSqlQuery insertItem;
        insertItem.prepare(R"(
            INSERT INTO RetailInvoicesItems (invoice_id, product_id, quantity)
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
    db.commit();  // lưu thành công
    QMessageBox::information(this, "Thành công", "Đã lưu hóa đơn!");
    accept();
    this->close();
}

void SalesWindow::setCompleter()
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
    ui->search->setCompleter(completer);

    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, [this, suggestionToIdMap](const QString &selectedText) {
                QString id = suggestionToIdMap.value(selectedText);
                if (!id.isEmpty()) {
                    QAction *fakeAction = new QAction(this);
                    fakeAction->setData(id);
                    selectedBooks(fakeAction);

                    QTimer::singleShot(0, this, [this]() {
                        ui->search->clear();
                    });
                }
            });
}

int SalesWindow::getInvoiceId(const QString &numBill)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM RetailInvoices WHERE bill_num = ?");
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

void SalesWindow::removeProductFromBill()
{
    int currentRow = ui->products->currentRow();
    if (currentRow >= 0) {
        ui->products->removeRow(currentRow);
        updateTotals(); // Cập nhật lại tổng sau khi xóa
    } else {
        QMessageBox::warning(this, "Chưa chọn sản phẩm", "Vui lòng chọn sản phẩm muốn xóa khỏi hóa đơn.");
    }
}


void SalesWindow::loadStock(int row, int col){
    int stock;
    if(col == 0 || col == 1) {
        stock = getStock(row);
        if (stock>=0) {
            ui->stock->setText(QString::number(stock));
        }
        else {
            ui->stock->setText("N/A");
        }
    }
    return;
}

int SalesWindow::getStock(int row){
    if(row<0)
        return -1;
    QString productId = ui->products->item(row,0)->text();
    QSqlQuery query;
    query.prepare("SELECT stock FROM Products WHERE id = ?");
    query.addBindValue(productId);

    if (!query.exec()) {
        qWarning() << "Lỗi truy vấn lấy id:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        qWarning() << "Không tìm thấy sản phẩm";
        return -1;
    }
}

void SalesWindow::addProductToTable(const QString &productId, const QString &name, const QString &author, double price, int stock)
{
    if (stock <= 0) {
        QMessageBox::warning(this, "Hết hàng", "Sản phẩm này đã hết hàng.");
        return;
    }
    // Kiểm tra nếu sản phẩm đã có trong bảng
    for (int r = 0; r < ui->products->rowCount(); ++r) {
        QTableWidgetItem *item = ui->products->item(r, 0);
        if (item && item->text() == productId) {
            QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->products->cellWidget(r, 3));
            if (spinBox) {
                int currentQty = spinBox->value();
                if (currentQty + 1 > stock) {
                    QMessageBox::warning(this, "Vượt tồn kho",
                                         QString("Chỉ còn %1 sản phẩm trong kho.").arg(stock));
                    return;
                }
                spinBox->setValue(currentQty + 1);
                updateTotals();
                ui->search->clear();
                return;
            }
        }
    }

    int row = ui->products->rowCount();
    ui->products->insertRow(row);

    // Kết hợp "Tên sản phẩm" và "Tên tác giả" thành một chuỗi
    QString nameWithAuthor = name + " - " + author;

    // Thêm dữ liệu vào bảng
    QLocale locale(QLocale::Vietnamese);
    double priceValue = price;
    QString formattedPrice = locale.toString(priceValue, 'f', 0);

    ui->products->setItem(row, 0, new QTableWidgetItem(productId));
    ui->products->setItem(row, 1, new QTableWidgetItem(nameWithAuthor)); // Sử dụng nameWithAuthor
    QTableWidgetItem *priceItem = new QTableWidgetItem(formattedPrice);
    priceItem->setData(Qt::UserRole, priceValue); // Lưu giá trị thật dưới dạng double
    ui->products->setItem(row, 2, priceItem);

    // Thêm QSpinBox để chỉnh số lượng
    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setMinimum(1);
    spinBox->setMaximum(stock);
    spinBox->setValue(1);
    ui->products->setCellWidget(row, 3, spinBox);

    // Cập nhật thành tiền
    double totalAmount = priceValue * spinBox->value();  // Thành tiền = Giá bán * Số lượng
    QString formattedTotal = locale.toString(totalAmount, 'f', 0); // 'f' cho dạng float, 0 số sau dấu thập phân
    ui->products->setItem(row, 4, new QTableWidgetItem(formattedTotal));

    // Khi người dùng thay đổi số lượng, tự động tính lại thành tiền
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, row, priceValue](int newValue) {
        double totalAmount = priceValue * newValue;
        QLocale locale(QLocale::Vietnamese);
        QString formattedTotal = locale.toString(totalAmount, 'f', 0);
        ui->products->setItem(row, 4, new QTableWidgetItem(formattedTotal));
        updateTotals();
    });
    updateTotals();
}

void SalesWindow::addClicked(){
    auto productDetails = new ProductDetails(this);

    // Kết nối signal từ ProductDetailsWindow đến slot thêm sản phẩm
    connect(productDetails, &ProductDetails::productSelected,
            this, &SalesWindow::addProductToTable);

    productDetails->show(); // Hiển thị cửa sổ chọn sản phẩm
}




