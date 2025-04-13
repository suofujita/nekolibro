#include "saleswindow.h"
#include "ui_saleswindow.h"

SalesWindow::SalesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SalesWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/shopping-cart.png"));
    setWindowTitle("Neko Libro - Bán hàng");
    /* Thêm phóng to/thu nhỏ cho dialog + wait */
    this->setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    /* Bảng chứa sản phẩm */
    ui->products->setColumnCount(4);
    ui->products->setHorizontalHeaderLabels({"Tên sản phẩm", "Giá bán", "Số lượng", "Thành tiền"});
    ui->products->horizontalHeader()->setStretchLastSection(true);
    //Cố định chiều rộng cột
    ui->products->setColumnWidth(0, 300); // "Tên sản phẩm"
    ui->products->setColumnWidth(1, 100); // "Giá bán"
    ui->products->setColumnWidth(2, 80);  // "Số lượng"
    ui->products->setColumnWidth(3, 120); // "Thành tiền"
    // Không tự kéo giãn cột cuối
    ui->products->horizontalHeader()->setStretchLastSection(false);

    /* Chỉ cho người dùng nhập */
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
                    QTimer::singleShot(0, this, [this]() {
                        ui->search->clear();
                    });
            }
            });
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
        ui->products->setItem(row, 0, new QTableWidgetItem(nameWithAuthor)); // Sử dụng nameWithAuthor
        ui->products->setItem(row, 1, new QTableWidgetItem(price));

        // Thêm QSpinBox để chỉnh số lượng
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setMinimum(1);
        spinBox->setMaximum(9999);
        spinBox->setValue(1);
        ui->products->setCellWidget(row, 2, spinBox);
    }
    ui->search->clear(); // Clear sau khi chọn
}
