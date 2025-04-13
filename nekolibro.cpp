#include "nekolibro.h"
#include "./ui_nekolibro.h"

#include "categorieswindow.h"
#include "imexport.h"
NekoLibro::NekoLibro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::NekoLibro)
{
    ui->setupUi(this);

    /* connect signals & slots */
    /* Dùng QToolButton tiện hơn */
    connect(ui->logout, &QPushButton::clicked,this,&NekoLibro::clickedLogOut);
    connect(ui->sales, &QPushButton::clicked,this,&NekoLibro::openSalesWindow);
    connect(ui->employees, &QPushButton::clicked,this,&NekoLibro::openEmployeesWindow);

    //connect(ui->categories,&QPushButton::clicked,this,&NekoLibro::openCategoriesWindow);

    connect(ui->import_export,&QPushButton::clicked,this,&NekoLibro::openImExportWindow);

    connect(ui->categories, &QToolButton::clicked, this, &NekoLibro::openCategoriesWindow);       // success 12/4/25 7:51PM

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


    pLogin = new login();
    pLogin->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
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

void NekoLibro::openImExportWindow(){
   // if(!pImExportWindow) {
        pImExportWindow = new ImExport;
   // }
    pImExportWindow->setAttribute(Qt::WA_DeleteOnClose); // cửa sổ đóng thì giải phóng vùng nhớ
    pImExportWindow->show();
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
    query.prepare("SELECT fullname FROM Users WHERE username = ?");
    query.addBindValue(getCurrentUser());
    qDebug() << "Đang dùng username:" << getCurrentUser();
    if(query.exec() && query.next()){
        currentFullName = query.value(0).toString();
    }
    ui->fullname->setText(currentFullName);
}




