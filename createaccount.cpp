#include "createaccount.h"
#include "ui_createaccount.h"

createAccount::createAccount(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::createAccount)
{
    ui->setupUi(this);  /* phải ở trên đầu*/

    setWindowTitle("Tạo tài khoản - Neko Libro");
    setWindowIcon(QIcon(":/image/cat.png"));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("nekolibro.db"); // Thay bằng đường dẫn đến file cơ sở dữ liệu
    if (!db.open()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    QSqlQuery enableFK;
    enableFK.exec("PRAGMA foreign_keys = ON");

    connect(ui->accept, &QPushButton::clicked,this,&createAccount::clickedAccept);
    connect(ui->cancel,&QPushButton::clicked,this,&createAccount::clickedReject);
    connect(ui->eye,&QPushButton::clicked,this,&createAccount::clickedTogglePass);

    ui->eye->setIcon(QIcon(":/image/close_eye.png"));
}

createAccount::~createAccount()
{
    delete ui;
}

void createAccount::clickedReject(){
    showMinimized();// thu nhỏ không đóng
}


void createAccount::clickedAccept() {
    QString new_username = ui->new_username_edit->text();
    QString plain_new_password = ui->new_password_edit->text();
    QString new_email = ui->new_email_edit->text();
    QString hashed_new_password = NekoLibro::hashPassword(plain_new_password);
    QString selected_role = ui->role_comboBox->currentText();

    // Kiểm tra thông tin không được bỏ trống
    if (new_username.isEmpty() || plain_new_password.isEmpty() || new_email.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng không bỏ trống thông tin!");
        return;
    }

    QSqlQuery query;

    // Kiểm tra tên tài khoản đã tồn tại chưa
    query.prepare("SELECT * FROM AccountUsers WHERE username = ?");
    query.addBindValue(new_username);
    if (query.exec() && query.next()) {
        QMessageBox::warning(this, "Lỗi", "Tên người dùng đã tồn tại!");
        return;
    }

    if (!new_email.contains("@")) {
        QMessageBox::warning(this, "Lỗi", "Email không hợp lệ!");
        return;
    }

    // Kiểm tra email đã tồn tại chưa
    query.prepare("SELECT * FROM AccountUsers WHERE email = ?");
    query.addBindValue(new_email);
    if (query.exec() && query.next()) {
        QMessageBox::warning(this, "Lỗi", "Email đã được sử dụng đăng ký!");
        return;
    }

    query.prepare("INSERT INTO AccountUsers (username, password_hash, role, email) "
                  "VALUES (?, ?, ?, ?)");
    query.addBindValue(new_username);
    query.addBindValue(hashed_new_password);
    query.addBindValue(selected_role);
    query.addBindValue(new_email);

    // Thêm thông tin người dùng vào bảng UserProfiles
    if (query.exec()) {
            QMessageBox::information(this, "Tạo tài khoản mới", "Tạo tài khoản mới thành công!");
            close();
        }
        else {
            QMessageBox::warning(this, "Lỗi", "Không thể tạo tài khoản mới!");
        }
}

void createAccount::clickedTogglePass(){
    passwordVisible = !passwordVisible;
    if (passwordVisible) {
        ui->new_password_edit->setEchoMode(QLineEdit::Normal);
        ui->eye->setIcon(QIcon(":/image/eye.png"));
    } else {
        ui->new_password_edit->setEchoMode(QLineEdit::Password);
        ui->eye->setIcon(QIcon(":/image/close_eye.png"));
    }
}

