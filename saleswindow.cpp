#include "saleswindow.h"
#include "ui_saleswindow.h"

SalesWindow::SalesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SalesWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/cat.png"));
    setWindowTitle("Neko Libro - Bán hàng");
    /* Thêm phóng to/thu nhỏ cho dialog + wait */
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    /* Bảng chứa sản phẩm */
    ui->products->setColumnCount(4);
    ui->products->setHorizontalHeaderLabels({"Tên sản phẩm", "Giá bán", "Số lượng", "Thành tiền"});
    ui->products->horizontalHeader()->setStretchLastSection(true);
    // Thiết kế các cột hiện thị dữ liệu hợp lý
    QHeaderView *header = ui->products->horizontalHeader();
    // Cột 0: Tên sản phẩm — cho co giãn theo cửa sổ
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    // Cột 1, 2, 3: giữ cố định
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);
    // Thiết lập chiều rộng cố định cho các cột không co giãn
    // chú ý không cho chỉnh sửa các cột để không bị mess dữ liệu
    ui->products->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->products->setColumnWidth(1, 100); // Giá bán
    ui->products->setColumnWidth(2, 80);  // Số lượng
    ui->products->setColumnWidth(3, 120); // Thành tiền
    // Không tự kéo giãn cột cuối
    ui->products->horizontalHeader()->setStretchLastSection(false);

    /* Chỉ mục cho người dùng nhập */
    ui->search->setPlaceholderText("Nhập tên sản phẩm hoặc tên tác giả...");

    connect(ui->search, &QLineEdit::textChanged, this, &SalesWindow::searchBooks);

    // Khởi tạo model chứa gợi ý "Tên sản phẩm - Tên tác giả"
    QStringList suggestions;
    QMap<QString, QString> suggestionToIdMap;

    QSqlQuery query("SELECT ID, `Tên sản phẩm`, `Tên tác giả` FROM Categories");
    while (query.next()) {
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString author = query.value(2).toString();
        QString displayText = name + " - " + author;
        suggestions << displayText;
        suggestionToIdMap[displayText] = id;
    }

    // Tạo completer từ danh sách gợi ý
    QCompleter *completer = new QCompleter(suggestions, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->search->setCompleter(completer);

    // Khi chọn gợi ý → gọi selectedBooks()
    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, [this, suggestionToIdMap](const QString &selectedText) {
                QString id = suggestionToIdMap.value(selectedText);
                if (!id.isEmpty()) {
                    QAction *fakeAction = new QAction(this);
                    fakeAction->setData(id);
                    selectedBooks(fakeAction);
                    //Trì hoãn việc clear để tránh bị Qt ghi đè lại nội dung
                    /* Đến đây có vấn đề là enter nhưng ô tìm kiếm không bị xóa */
                    /* đã giải quyết được rồi 13.4.25 23.30*/
                    QTimer::singleShot(0, this, [this]() {
                        ui->search->clear();
                    });
            }
            });
    time = new QTimer(this);
    connect(time, &QTimer::timeout, this, &SalesWindow::showTime);
    time->start(1000);
    showTime();
    showFullName();
    ui->label->setFixedSize(30,30);
    ui->logo->setFixedSize(100,100);
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
    query.prepare("SELECT `ID`, `Tên sản phẩm`, `Tên tác giả`, `Giá bán` FROM Categories WHERE `Tên sản phẩm` LIKE ? OR `Tên tác giả` LIKE ?");
    query.addBindValue("%" + text + "%");
    query.addBindValue("%" + text + "%");

    if (query.exec()) {
        while (query.next()) {
            QString id = query.value(0).toString();
            QString name = query.value(1).toString();
            QString author = query.value(2).toString();

            // Ghép "Tên sản phẩm - Tên tác giả"
            QString displayText = name + " - " + author;

            QAction *action = new QAction(displayText, this);
            action->setData(id);
            connect(action, &QAction::triggered, [this, action]() {
                selectedBooks(action);
            });
        }
    }
}
void SalesWindow::selectedBooks(QAction *action)
{
    QString productId = action->data().toString();

    QSqlQuery query;
    query.prepare("SELECT `ID`, `Tên sản phẩm`, `Tên tác giả`, `Giá bán` FROM Categories WHERE id = ?");
    query.addBindValue(productId);

    if (query.exec() && query.next()) {
        QString name = query.value(1).toString();
        QString author = query.value(2).toString();
        QString price = query.value(3).toString();

        int row = ui->products->rowCount();
        ui->products->insertRow(row);

        // Kết hợp "Tên sản phẩm" và "Tên tác giả" thành một chuỗi
        QString nameWithAuthor = name + " - " + author;

        // Thêm dữ liệu vào bảng
        QLocale locale(QLocale::Vietnamese);
        double priceValue = price.toDouble();
        QString formattedPrice = locale.toString(priceValue, 'f', 0);
        ui->products->setItem(row, 0, new QTableWidgetItem(nameWithAuthor)); // Sử dụng nameWithAuthor
        ui->products->setItem(row, 1, new QTableWidgetItem(formattedPrice));

        // Thêm QSpinBox để chỉnh số lượng
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMinimum(1);
        spinBox->setMaximum(9999);
        spinBox->setValue(1);
        ui->products->setCellWidget(row, 2, spinBox);

        // Cập nhật thành tiền
        double totalAmount = priceValue * spinBox->value();  // Thành tiền = Giá bán * Số lượng
        QString formattedTotal = locale.toString(totalAmount, 'f', 0); // 'f' cho dạng float, 0 số sau dấu thập phân
        ui->products->setItem(row, 3, new QTableWidgetItem(formattedTotal));


        // Khi người dùng thay đổi số lượng, tự động tính lại thành tiền
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this, row, priceValue](int newValue) {
            double totalAmount = priceValue * newValue;
            QLocale locale(QLocale::Vietnamese);
            QString formattedTotal = locale.toString(totalAmount, 'f', 0);
            ui->products->setItem(row, 3, new QTableWidgetItem(formattedTotal));
            updateTotals();
        });

    }
    ui->search->clear(); // Clear sau khi chọn
    updateTotals();
}

void SalesWindow::updateTotals()
{
    int totalQuantity = 0;
    double totalPrice = 0.0;

    for (int row = 0; row < ui->products->rowCount(); ++row) {
        QSpinBox *spinBox = qobject_cast<QSpinBox*>(ui->products->cellWidget(row, 2));
        if (!spinBox) continue;

        int quantity = spinBox->value();
        totalQuantity += quantity;

        QString priceText = ui->products->item(row, 1)->text();
        QString cleaned = priceText;
        cleaned.remove('.');  // Xử lý dấu chấm ngăn cách hàng nghìn

        double price = cleaned.toDouble();
        totalPrice += price * quantity;
    }

    QLocale locale = QLocale::Vietnamese;
    QString formattedTotal = locale.toString(totalPrice, 'f', 0);

    ui->num_book->setText(QString::number(totalQuantity));
    ui->total->setText(formattedTotal);
}


void SalesWindow::showTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();  // Lấy ngày và giờ hiện tại
    QString dateTimeString = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");  // Định dạng ngày và giờ
    ui->date_time->setText(dateTimeString);  // Hiển thị ngày và giờ trong QLabel
}


void SalesWindow::showFullName(){
    QString currentUser = NekoLibro::currentUser;
    QSqlQuery query;
    query.prepare("SELECT fullname FROM Users WHERE username = ?");
    query.addBindValue(currentUser);
    if(query.exec() && query.next()){
        currentFullName = query.value(0).toString();
    }
    ui->name->setText(currentFullName);
}



