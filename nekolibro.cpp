#include "nekolibro.h"
#include "./ui_nekolibro.h"

#include "categorieswindow.h"
#include "imexport.h"
#include "saleswindow.h"


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

void NekoLibro::clickedLogOut(){
    this->close();

    /* Giải phóng vùng nhớ */
    if(pSaleWindow) {
        delete pSaleWindow;
        pSaleWindow = nullptr;
    }
    if(pEmployeesWindow) {
        delete pEmployeesWindow;
        pEmployeesWindow = nullptr;
    }
    if (!pLogin) {
        pLogin = new login(this);
        pLogin->setAttribute(Qt::WA_DeleteOnClose);
    }
    pLogin->show();
}

void NekoLibro::openSalesWindow(){
    pSaleWindow = new SalesWindow;
    pSaleWindow->setAttribute(Qt::WA_DeleteOnClose);  // cửa sổ đóng thì giải phóng vùng nhớ
    pSaleWindow->show();
}

void NekoLibro::openEmployeesWindow(){
    pEmployeesWindow = new EmployeesWindow;
    pEmployeesWindow->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pEmployeesWindow->show();
}

void NekoLibro::openCategoriesWindow() {
    if (!pCategoriesWindow) {
        pCategoriesWindow = new CategoriesWindow(this);
        pCategoriesWindow->setAttribute(Qt::WA_DeleteOnClose);

        /* Đưa về trang chủ */
        pCategoriesWindow->toMainCategories();
        /* Xóa con trỏ khi đóng cửa sổ */
        connect(pCategoriesWindow, &QObject::destroyed, [this]() {
            pCategoriesWindow = nullptr;
        });
    }

    pCategoriesWindow->show();
    pCategoriesWindow->raise();
    pCategoriesWindow->activateWindow();
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
    query.prepare("SELECT full_name FROM UserProfiles WHERE username = ?");
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

void NekoLibro::openImExportWindow()
{
    if (!pImExportWindow) {
        pImExportWindow = new ImExport();
        pImExportWindow->setWindowFlag(Qt::Window);  // Cửa sổ riêng biệt
        pImExportWindow->setAttribute(Qt::WA_DeleteOnClose);

        /* Đưa về trang chủ */
        pImExportWindow->toMainImExport();
        /* Xóa con trỏ khi đóng cửa sổ */
        connect(pImExportWindow, &QObject::destroyed, [this]() {
            pImExportWindow = nullptr;
        });
    }

    pImExportWindow->show();
    pImExportWindow->raise();
    pImExportWindow->activateWindow();
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
