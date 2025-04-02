#include "createaccount.h"
#include "ui_createaccount.h"

createAccount::createAccount(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::createAccount)
{
    ui->setupUi(this);  /* phải ở trên đầu*/

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db"); // Thay bằng đường dẫn đến file cơ sở dữ liệu
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }

    connect(ui->accept, &QPushButton::clicked,this,&createAccount::clickedAccept);
    connect(ui->cancel,&QPushButton::clicked,this,&createAccount::clickedReject);
}

createAccount::~createAccount()
{
    delete ui;
}

void createAccount::clickedReject(){
    showMinimized();// thu nhỏ không đóng
}

void createAccount::clickedAccept(){
    QString new_username = ui->new_username_edit->text();
    QString new_password = ui->new_password_edit->text();
    QString new_email = ui->new_gmail_edit->text();
    QString new_fullname = ui->new_fullname_edit->text();

    /* Không bỏ trống thông tin */
    if(new_username.isEmpty() || new_password.isEmpty() || new_email.isEmpty() || new_fullname.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng không bỏ trống thông tin!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE username = ?");
    query.addBindValue(new_username);

    /* Kiểm tra tên tài khoản đã tồn tại chưa */
    if(query.exec() && query.next()) {
        QMessageBox::warning(this, "Lỗi", "Tên người dùng đã tồn tại!");
        return;
    }
    /* Tài khoản hợp lệ */
    query.prepare("INSERT INTO Users(username,password,email,fullname)"
                  "VALUES( ? , ? , ?, ?)");
    query.addBindValue(new_username);
    query.addBindValue(new_password);
    query.addBindValue(new_email);
    query.addBindValue(new_fullname);
    /* Thông báo thành công */
    if(query.exec()) {
        QMessageBox::information(this,"Tạo tài khoản mới","Tạo tài khoản mới thành công!");
        close();   // đóng hộp thoại
    }

}
