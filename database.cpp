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

void database::createUserProfiles(){
    QSqlQuery query;
    if(!query.exec(R"(
    CREATE TABLE IF NOT EXISTS UserProfiles (
        user_id INTEGER PRIMARY KEY,
        full_name TEXT,
        email TEXT NOT NULL,
        date_of_birth TEXT,
        hometown TEXT,
        phone TEXT,
        FOREIGN KEY(user_id) REFERENCES AccountUsers(id)
    )
)")) {
        qDebug() << "Lỗi tạo bảng UserProfiles:" << query.lastError().text();
    }
}

void database::createAdminAccount() {
    if (db.isOpen()) {
        QString defaultPassword = "123";
        QString hashedDefaultPass = NekoLibro::hashPassword(defaultPassword);
        QSqlQuery query;

        // Kiểm tra xem tài khoản admin đã tồn tại chưa
        query.prepare("SELECT * FROM AccountUsers WHERE username = 'admin'");
        if (query.exec() && !query.next()) {
            // Nếu chưa có tài khoản admin, tạo tài khoản admin mới
            query.prepare("INSERT INTO AccountUsers (username, password_hash, role) "
                          "VALUES ('admin', ?, 'admin')");
            query.addBindValue(hashedDefaultPass);

            if (query.exec()) {
                qDebug() << "Tài khoản admin đã được tạo thành công!";
                qint64 user_id = query.lastInsertId().toLongLong();
                query.prepare("INSERT INTO UserProfiles (user_id, email) "
                              "VALUES (?,'admin123@email.com')");
                query.addBindValue(user_id);
                if(query.exec()) {
                    qDebug() << "Đã thên thông tin vào bảng UserProfiles";
                }
                else {
                    qDebug() << "Lỗi khi thêm dữ liệu admin";
                }
            } else {
                qDebug() << "Lỗi khi tạo tài khoản admin:" << query.lastError().text();
            }
        } else {
            qDebug() << "Tài khoản admin đã tồn tại!";
        }
    } else {
        qDebug() << "Cơ sở dữ liệu chưa mở!";
    }
}

void database::closeDatabase() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Đóng kết nối với cơ sở dữ liệu";
    }
}
