#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    /* Kết nối Database */
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

    loadUserInfor();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::loadUserInfor() {
    QString currentUser = NekoLibro::currentUser;
    ui->username->setText(currentUser);

    QSqlQuery query;
    query.prepare("SELECT fullname, email, date_of_birth, hometown, phone, role "
                  "FROM AccountUsers WHERE username = ?");
    query.addBindValue(currentUser);

    if (!query.exec()) {
        QMessageBox::warning(this, "Lỗi", "Không thể truy vấn thông tin người dùng!");
        return;
    }

    if (query.next()) {
        ui->fullname->setText(query.value(0).toString());
        ui->email->setText(query.value(1).toString());
        ui->dob->setText(query.value(2).toString());
        ui->hometown->setText(query.value(3).toString());
        ui->phone->setText(query.value(4).toString());
        ui->role->setText(query.value(5).toString());
    }
}
