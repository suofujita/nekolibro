#include "employeeswindow.h"
#include "ui_employeeswindow.h"

#include "createaccount.h"

EmployeesWindow::EmployeesWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EmployeesWindow)
{
    ui->setupUi(this);

    setWindowTitle("Quản lý nhân viên - Neko Libro");
    setWindowIcon(QIcon(":/image/cat.png"));

    /* Kết nối CSDL */
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

    /* signals slots*/
    connect(ui->close_tab,&QPushButton::clicked, this, &EmployeesWindow::closeTab);
    connect(ui->remove_user,&QPushButton::clicked, this, &EmployeesWindow::removeUser);
    connect(ui->add_new_user,&QPushButton::clicked, this, &EmployeesWindow::addNewUser);
    connect(ui->lock_user,&QPushButton::clicked, this, &EmployeesWindow::updateStatusUser);

    allTablesUI();
    loadLast6MonthPerUser();
    loadThisMonthData();
    loadUsersInfor();
}

EmployeesWindow::~EmployeesWindow()
{
    delete ui;
}

void EmployeesWindow::allTablesUI(){
    /* Thông tin */
    ui->infor->setColumnCount(9);
    ui->infor->setHorizontalHeaderLabels({"ID", "Username","Họ và tên", "Ngày sinh", "Số điện thoại","Email","Quê quán","Phân quyền", "Trạng thái"});
    ui->infor->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerInfor = ui->infor->horizontalHeader();
    for(int col=0; col<9 ;col++){
        headerInfor->setSectionResizeMode(col, QHeaderView::ResizeToContents);  // kích thước ô tự động giãn theo nội dung
    }
    ui->infor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infor->horizontalHeader()->setStretchLastSection(true);
    ui->infor->setSortingEnabled(true);

    /* 6 tháng gần đây*/
    ui->last6mon->setColumnCount(3);
    ui->last6mon->setHorizontalHeaderLabels({"ID", "Username","Tổng doanh thu"});
    ui->last6mon->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerLast6Month= ui->last6mon->horizontalHeader();
    for(int col=0; col<3 ;col++){
        headerLast6Month->setSectionResizeMode(col, QHeaderView::ResizeToContents);  // kích thước ô tự động giãn theo nội dung
    }
    ui->last6mon->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->last6mon->horizontalHeader()->setStretchLastSection(true);

    /* tháng này */
    ui->this_month->setColumnCount(3);
    ui->this_month->setHorizontalHeaderLabels({"ID", "Username","Doanh thu"});
    ui->this_month->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerThisMonth= ui->this_month->horizontalHeader();
    for(int col=0; col<3 ;col++){
        headerThisMonth->setSectionResizeMode(col, QHeaderView::ResizeToContents);  // kích thước ô tự động giãn theo nội dung
    }
    ui->this_month->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->this_month->horizontalHeader()->setStretchLastSection(true);
}

void EmployeesWindow::loadUsersInfor() {
    QSqlQuery query;
    query.prepare("SELECT id, username, fullname, date_of_birth, phone, email, hometown, role, active FROM AccountUsers");

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError();
        return;
    }

    ui->infor->setRowCount(0); // Xóa dữ liệu cũ nếu có
    int row = 0;
    /* sử dụng vòng lặp while mới lấy được hết các dòng */
    while (query.next()) {
        ui->infor->insertRow(row); // Thêm dòng mới

        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->infor->setItem(row, col, item);
        }

        row++;
    }
}

void EmployeesWindow::loadLast6MonthPerUser() {
    // Tính ngày bắt đầu và kết thúc
    QDate endDate = QDate::currentDate();
    QDate startDate = endDate.addDays(-180);

    QSqlQuery query;
    query.prepare(
        "SELECT A.username, A.fullname, COALESCE(SUM(R.total_bill), 0) AS total "
        "FROM AccountUsers A "
        "LEFT JOIN RetailInvoices R ON A.id = R.user_id "
        "WHERE R.date BETWEEN :start AND :end OR R.date IS NULL "
        "GROUP BY A.id, A.username, A.fullname "
        "ORDER BY total DESC"
        );

    query.bindValue(":start", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError();
        return;
    }

    ui->last6mon->setRowCount(0);
    int row = 0;

    while (query.next()) {
        ui->last6mon->insertRow(row);

        QLocale locale(QLocale::Vietnamese);
        double total = query.value(2).toDouble();
        QString formattedTotal = locale.toString(total, 'f', 0);
        QTableWidgetItem *totalItem = new QTableWidgetItem(formattedTotal);
        totalItem->setData(Qt::UserRole, total); // phải dùng nếu muốn so sánh đúng khi sử dụng bộ lọc

        ui->last6mon->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->last6mon->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->last6mon->setItem(row, 2, totalItem);

        row++;
    }
}

void EmployeesWindow::loadThisMonthData() {
    // Lấy ngày đầu và cuối của tháng hiện tại
    QDate endDate = QDate::currentDate();
    QDate startDate(endDate.year(), endDate.month(), 1);  // Ngày 1 đầu tháng

    QSqlQuery query;
    query.prepare(
        "SELECT A.username, A.fullname, COALESCE(SUM(R.total_bill), 0) AS total "
        "FROM AccountUsers A "
        "LEFT JOIN RetailInvoices R ON A.id = R.user_id "
        "AND R.date BETWEEN :start AND :end "
        "GROUP BY A.id, A.username, A.fullname "
        "ORDER BY total DESC"
        );

    query.bindValue(":start", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":end", endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError();
        return;
    }

    ui->this_month->setRowCount(0);
    int row = 0;

    while (query.next()) {
        ui->this_month->insertRow(row);

        QLocale locale(QLocale::Vietnamese);
        double total = query.value(2).toDouble();
        QString formattedTotal = locale.toString(total, 'f', 0);
        QTableWidgetItem *totalItem = new QTableWidgetItem(formattedTotal);
        totalItem->setData(Qt::UserRole, total); // phải dùng nếu muốn so sánh đúng khi sử dụng bộ lọc

        ui->this_month->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->this_month->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->this_month->setItem(row, 2, totalItem);

        row++;
    }
}

void EmployeesWindow::closeTab(){
    this->close();
}

void EmployeesWindow::removeUser() {
    int row = ui->infor->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tài khoản!");
        return;
    }

    QString username = ui->infor->item(row, 1)->text();

    int confirm = QMessageBox::question(this, "Xác nhận", "Bạn có chắc muốn xóa tài khoản \"" + username + "\" không?", QMessageBox::Yes | QMessageBox::No);

    if (confirm != QMessageBox::Yes)
        return;

    QSqlQuery query;
    query.prepare("DELETE FROM AccountUsers WHERE username = ?");
    query.addBindValue(username);

    if (!query.exec()) {
        QMessageBox::critical(this, "Lỗi", "Không thể xóa tài khoản!\n" + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Thành công", "Đã xóa tài khoản thành công!");
    loadUsersInfor();
}


void EmployeesWindow::addNewUser() {
    if (!pCreateAccount) {
        pCreateAccount = new createAccount(this);
        pCreateAccount->setAttribute(Qt::WA_DeleteOnClose);  // Tự xóa khi đóng
        connect(pCreateAccount, &createAccount::destroyed, this, [=]() {
            pCreateAccount = nullptr;
        });
    }
    pCreateAccount->show();
    pCreateAccount->raise();  // Đưa cửa sổ lên trên
    pCreateAccount->activateWindow(); // Lấy focus
}


void EmployeesWindow::updateStatusUser() {
    int row = ui->infor->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chọn một tài khoản!");
        return;
    }

    QString username = ui->infor->item(row, 1)->text();
    QSqlQuery query;

    // Lấy trạng thái hiện tại
    query.prepare("SELECT active FROM AccountUsers WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Lỗi", "Không thể lấy trạng thái tài khoản!");
        return;
    }

    int currentStatus = query.value(0).toInt();
    int newStatus = (currentStatus == 1) ? 0 : 1;

    // Cập nhật trạng thái mới
    query.prepare("UPDATE AccountUsers SET active = ? WHERE username = ?");
    query.addBindValue(newStatus);
    query.addBindValue(username);

    if (!query.exec()) {
        QMessageBox::critical(this, "Lỗi", "Không thể cập nhật trạng thái!\n" + query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Thành công", "Cập nhật trạng thái thành công!");
    loadUsersInfor();
}



