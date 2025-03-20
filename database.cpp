#include "database.h"

QSqlDatabase db;

database::database() {}

bool database::connectDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db");

    if (!db.open()) {
        qDebug() << "Kết nối cơ sở dữ liệu thất bại: " << db.lastError().text();
        return false;
    }
    qDebug() << "Kết nối cơ sở dữ liệu thành công!";
    return true;
}

void database::createUsersTable() {
    if (db.isOpen()) {
        QSqlQuery query;
        if (!query.exec("CREATE TABLE IF NOT EXISTS Users ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "username TEXT UNIQUE NOT NULL,"
                        "password TEXT NOT NULL,"
                        "email TEXT UNIQUE NOT NULL,"
                        "isAdmin INTEGER DEFAULT 0)")) {
            qDebug() << "Lỗi khi tạo bảng:" << query.lastError().text();
        } else {
            qDebug() << "Tạo bảng thành công hoặc bảng đã tồn tại.";
        }
    }
}

void database::createAdminAccount() {
    if (db.isOpen()) {
        QSqlQuery query;
        // Kiểm tra xem tài khoản admin đã tồn tại chưa
        query.prepare("SELECT * FROM Users WHERE isAdmin = 1");
        if (query.exec() && !query.next()) {
            // Nếu chưa có tài khoản admin, tạo tài khoản admin mới
            query.prepare("INSERT INTO Users (username, password, email, isAdmin) "
                          "VALUES ('admin', '123456789', 'admin@email.com', 1)");

            if (query.exec()) {
                qDebug() << "Tài khoản admin đã được tạo thành công!";
            } else {
                qDebug() << "Lỗi khi tạo tài khoản admin:" << query.lastError().text();
            }
        } else {
            qDebug() << "Tài khoản admin đã tồn tại!";
        }
    }
}

void database::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Đóng kết nối với cơ sở dữ liệu";
    }
}
