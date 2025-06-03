#include "nekolibro.h"
#include "./ui_nekolibro.h"

#include "categorieswindow.h"
#include "imexport.h"
#include "saleswindow.h"
#include "reports.h"
#include "settings.h"
#include "employeeswindow.h"
#include "accountswindow.h"

/* Phải khởi tạo biến stactic không chương trinh sẽ bị lỗi undefined */
QString NekoLibro::currentUser = "";
QString NekoLibro::currentFullName = "";

NekoLibro::NekoLibro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NekoLibro)
{
    ui->setupUi(this);
    setWindowTitle("Trang chủ - Neko Libro");
    setWindowIcon(QIcon(":/image/cat.png"));

    /* connect signals & slots */
    /* Dùng QToolButton tiện hơn */
    connect(ui->logout, &QToolButton::clicked,this,&NekoLibro::clickedLogOut);
    connect(ui->sales, &QToolButton::clicked,this,&NekoLibro::openSalesWindow);
    connect(ui->employees, &QToolButton::clicked,this,&NekoLibro::openEmployeesWindow);
    connect(ui->im_export,&QToolButton::clicked,this,&NekoLibro::openImExportWindow);
    connect(ui->categories, &QToolButton::clicked, this, &NekoLibro::openCategoriesWindow);       // success 12/4/25 7:51PM using QToolButton
    connect(ui->reports, &QToolButton::clicked,this,&NekoLibro::openReportWindow);
    connect(ui->settings, &QToolButton::clicked,this,&NekoLibro::openSettingsWindow);
    connect(ui->accounts, &QToolButton::clicked,this,&NekoLibro::openAccountsWindow);

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

    menuCategories = new QMenu();
    menuCategories->addAction("Thêm sản phẩm mới", this, &NekoLibro::gotoAddBook);
    ui->categories->setMenu(menuCategories);

    menuImExport = new QMenu();
    menuImExport->addAction("Nhập hàng", this, &NekoLibro::gotoImportInvoice);
    menuImExport->addAction("Xuất hàng", this, &NekoLibro::gotoExportInvoice);
    menuImExport->addAction("Lịch sử nhập hàng", this, &NekoLibro::gotoImportLogs);
    menuImExport->addAction("Lịch sử xuất hàng", this, &NekoLibro::gotoExportLogs);
    ui->im_export->setMenu(menuImExport);

    time = new QTimer(this);
    connect(time, &QTimer::timeout, this, &NekoLibro::showTime);
    time->start(1000);
    showTime();
}

NekoLibro::~NekoLibro()
{
    if (pSaleWindow) delete pSaleWindow;
    if (pEmployeesWindow) delete pEmployeesWindow;
    if (pCategoriesWindow) delete pCategoriesWindow;
    if (pImExportWindow) delete pImExportWindow;
    if (db.isOpen()) db.close();
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    delete ui;
}

void NekoLibro::openSalesWindow(){
    if (!pSaleWindow) {
        pSaleWindow = new SalesWindow(this);
        pSaleWindow->setWindowFlag(Qt::Window);
        childWindows.append(pSaleWindow);  // Đưa vào danh sách để quản lý sau
    }

    pSaleWindow->show();
    pSaleWindow->raise();
    pSaleWindow->activateWindow();
}

void NekoLibro::openEmployeesWindow(){
    if (!pEmployeesWindow) {
        pEmployeesWindow = new EmployeesWindow(this);
        pEmployeesWindow->setWindowFlag(Qt::Window);
        childWindows.append(pEmployeesWindow);  // Đưa vào danh sách để quản lý sau
    }

    pEmployeesWindow->show();
    pEmployeesWindow->raise();
    pEmployeesWindow->activateWindow();
}

void NekoLibro::openCategoriesWindow() {
    if (!pCategoriesWindow) {
        pCategoriesWindow = new CategoriesWindow(this);
        childWindows.append(pCategoriesWindow);
        /* Đưa về trang chủ */
        pCategoriesWindow->toMainCategories();
    }

    pCategoriesWindow->show();
    pCategoriesWindow->raise();
    pCategoriesWindow->activateWindow();
}

void NekoLibro::openReportWindow() {
    if (!pReportWindow) {
        pReportWindow = new reports(this);
        pReportWindow->setWindowFlag(Qt::Window);
        childWindows.append(pReportWindow);  // Đưa vào danh sách để quản lý sau
    }

    pReportWindow->show();
    pReportWindow->raise();
    pReportWindow->activateWindow();
}

void NekoLibro::openSettingsWindow(){
    if (!pSettingWindow) {
        pSettingWindow = new Settings(this);
        pSettingWindow->setWindowFlag(Qt::Window);
        childWindows.append(pSettingWindow);  // Đưa vào danh sách để quản lý sau
    }

    pSettingWindow->show();
    pSettingWindow->raise();
    pSettingWindow->activateWindow();
}

void NekoLibro::openImExportWindow()
{
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();
        pImExportWindow->setWindowFlag(Qt::Window);  // Cửa sổ riêng biệt
        childWindows.append(pImExportWindow);   // đưa vào danh sách cửa sổ con của NekoLibro
        /* Đưa về trang chủ */
        pImExportWindow->toMainImExport();
    }

    pImExportWindow->show();
    pImExportWindow->raise();
    pImExportWindow->activateWindow();
}

void NekoLibro::openAccountsWindow()
{
    if (!pAccountsWindow) {
        pAccountsWindow = new AccountsWindow();
        pAccountsWindow->setWindowFlag(Qt::Window);  // Cửa sổ riêng biệt
        childWindows.append(pAccountsWindow);   // đưa vào danh sách cửa sổ con của NekoLibro
    }

    pAccountsWindow->show();
    pAccountsWindow->raise();
    pAccountsWindow->activateWindow();
}

QStringList NekoLibro::getCategoriesList(){
    QStringList categoriesList;
    QSqlQuery query("SELECT name FROM Categories");
    while (query.next()) {
        categoriesList << query.value(0).toString(); // Lấy cột đầu tiên là tên sản phẩm
    }
    return categoriesList;
}

void NekoLibro::gotoAddBook() {
    if (!pCategoriesWindow) {
        pCategoriesWindow = new CategoriesWindow(this);

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

void NekoLibro::showTime() {
    QDateTime currentDateTime = QDateTime::currentDateTime();  // Lấy ngày và giờ hiện tại
    QString dateTimeString = currentDateTime.toString("dd-MM-yyyy hh:mm:ss");  // Định dạng ngày và giờ
    ui->time->setText(dateTimeString);  // Hiển thị ngày và giờ trong QLabel
}

void NekoLibro::setCurrentUser(QString username) {
    currentUser = username;
}

QString NekoLibro::getCurrentUser(){
    return currentUser;
}

void NekoLibro::showUserName(){
    ui->username->setText(getCurrentUser());
}

void NekoLibro::showFullName(){
    QSqlQuery query;
    query.prepare("SELECT fullname FROM AccountUsers WHERE username = ?");
    query.addBindValue(getCurrentUser());
    qDebug() << "Đang dùng username:" << getCurrentUser();
    if(query.exec() && query.next()){
        currentFullName = query.value(0).toString();
    }
    ui->fullname->setText(currentFullName);
}

/* Mã hóa mật khẩu SHA256 */
QString NekoLibro::hashPassword(const QString &password){
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

int NekoLibro::getAuthorId(const QString &name) {
    if (name.trimmed().isEmpty()) {
        qDebug() << "Tên tác giả trống!";
        return -1;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Không thể mở cơ sở dữ liệu:" << db.lastError().text();
            return -1;
        }
    }

    QSqlQuery query(db);

    // Kiểm tra xem tác giả đã tồn tại chưa
    query.prepare("SELECT id FROM Authors WHERE name = ?");
    query.addBindValue(name.trimmed());

    if (!query.exec()) {
        qDebug() << "Lỗi truy vấn SELECT Authors:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();  // Tác giả đã tồn tại
    }
    return -1;
}


int NekoLibro::getCategoryId(const QString &categoryName) {
    if (categoryName.trimmed().isEmpty()) {
        qDebug() << "Tên phân loại trống!";
        return -1;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Không thể mở cơ sở dữ liệu:" << db.lastError().text();
            return -1;
        }
    }

    QSqlQuery query(db);

    // Kiểm tra xem phân loại đã tồn tại chưa
    query.prepare("SELECT id FROM Category WHERE name = ?");
    query.addBindValue(categoryName.trimmed());

    if (!query.exec()) {
        qDebug() << "Lỗi truy vấn SELECT Category:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();  // Tác giả đã tồn tại
    }
    return -1;
}

void NekoLibro::gotoImportInvoice(){
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();

        connect(pImExportWindow, &QObject::destroyed, [this]() {
            pImExportWindow = nullptr;
        });

        // Chỉ gọi sau khi show xong
        QTimer::singleShot(100, this, [this]() {
            if (pImExportWindow) {
                QMetaObject::invokeMethod(pImExportWindow, "gotoImportInvoice", Qt::QueuedConnection);
            }
        });

        pImExportWindow->show();
    } else {
        pImExportWindow->show();
        pImExportWindow->raise();
        pImExportWindow->activateWindow();

        // Không cần delay nếu cửa sổ đã khởi tạo đầy đủ
        pImExportWindow->gotoImportInvoice();
    }
}

void NekoLibro::gotoExportInvoice(){
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();

        connect(pImExportWindow, &QObject::destroyed, [this]() {
            pImExportWindow = nullptr;
        });

        // Chỉ gọi sau khi show xong
        QTimer::singleShot(100, this, [this]() {
            if (pImExportWindow) {
                QMetaObject::invokeMethod(pImExportWindow, "gotoExportInvoice", Qt::QueuedConnection);
            }
        });

        pImExportWindow->show();
    } else {
        pImExportWindow->show();
        pImExportWindow->raise();
        pImExportWindow->activateWindow();

        // Không cần delay nếu cửa sổ đã khởi tạo đầy đủ
        pImExportWindow->gotoExportInvoice();
    }
}

void NekoLibro::gotoImportLogs(){
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();

        connect(pImExportWindow, &QObject::destroyed, [this]() {
            pImExportWindow = nullptr;
        });

        // Chỉ gọi sau khi show xong
        QTimer::singleShot(100, this, [this]() {
            if (pImExportWindow) {
                QMetaObject::invokeMethod(pImExportWindow, "gotoExportInvoice", Qt::QueuedConnection);
            }
        });

        pImExportWindow->show();
    } else {
        pImExportWindow->show();
        pImExportWindow->raise();
        pImExportWindow->activateWindow();

        // Không cần delay nếu cửa sổ đã khởi tạo đầy đủ
        pImExportWindow->gotoImportLogs();
    }
}

void NekoLibro::gotoExportLogs(){
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();

        connect(pImExportWindow, &QObject::destroyed, [this]() {
            pImExportWindow = nullptr;
        });

        // Chỉ gọi sau khi show xong
        QTimer::singleShot(100, this, [this]() {
            if (pImExportWindow) {
                QMetaObject::invokeMethod(pImExportWindow, "gotoExportInvoice", Qt::QueuedConnection);
            }
        });

        pImExportWindow->show();
    } else {
        pImExportWindow->show();
        pImExportWindow->raise();
        pImExportWindow->activateWindow();

        // Không cần delay nếu cửa sổ đã khởi tạo đầy đủ
        pImExportWindow->gotoExportLogs();
    }
}

void NekoLibro::clearChildWindows() {
    for (QWidget *w : childWindows) {
        if (w) {
            w->close();    // Đảm bảo cửa sổ đóng
            delete w;      // Giải phóng bộ nhớ
        }
    }
    childWindows.clear();   // Xóa danh sách sau khi đã xóa tất cả

    // Đặt lại con trỏ thành nullptr
    pSaleWindow = nullptr;
    pEmployeesWindow = nullptr;
    pCategoriesWindow = nullptr;
    pImExportWindow = nullptr;
    pReportWindow = nullptr;
    pSettingWindow = nullptr;
    pAccountsWindow = nullptr;
}

void NekoLibro::clickedLogOut() {
    clearChildWindows();  // Xóa tất cả cửa sổ con

    if (!pLogin) {
        pLogin = new login();  // không truyền con trỏ this vì ....
        pLogin->setAttribute(Qt::WA_DeleteOnClose);
    }
    pLogin->show();   // mở login window

    this->close();  // Đóng cửa sổ chính,...  ở đây mình đóng
}
