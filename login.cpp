#include "login.h"
#include "ui_login.h"

#include "createaccount.h"
#include "nekolibro.h"
#include "resetpassword.h"
login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/cat.png"));
    setWindowTitle("Welcome to Neko Libro");

    connect(ui->login_button,&QPushButton::clicked,this,&login::clickedLogin);
    connect(ui->create_account_button, &QPushButton::clicked,this,&login::clickedCreateAccount);
    connect(ui->username_edit,&QLineEdit::returnPressed,this,&login::clickedLogin);
    connect(ui->password_edit,&QLineEdit::returnPressed,this,&login::clickedLogin);
    connect(ui->eye,&QPushButton::clicked,this,&login::clickedTogglePassword);
    connect(ui->reset_password_button,&QPushButton::clicked,this,&login::clickedForgetPassword);
    ui->eye->setIcon(QIcon(":/image/close_eye.png"));
}

login::~login()
{
    delete ui;
}

void login::clickedCreateAccount() {
    QString username = ui->username_edit->text();
    QString plainPassword = ui->password_edit->text();
    QString hashedPassword = NekoLibro::hashPassword(plainPassword);

    QSqlQuery query;
    query.prepare("SELECT role FROM AccountUsers WHERE username = ? AND password_hash = ?");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);

    if (query.exec() && query.next()) {
        QString role = query.value("role").toString();

        if (role == "admin") {
            if (!pCreateWindow) {
                pCreateWindow = new createAccount(this);
            }
            pCreateWindow->show();
        } else {
            QMessageBox::warning(this, "Lỗi", "Bạn không có quyền tạo tài khoản!");
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Thông tin tài khoản không chính xác!");
    }
}

/* Ẩn/Hiện mật khẩu */
void login::clickedTogglePassword()
{
    passwordVisible = !passwordVisible;

    if (passwordVisible) {
        ui->password_edit->setEchoMode(QLineEdit::Normal);
        ui->eye->setIcon(QIcon(":/image/eye.png"));
    } else {
        ui->password_edit->setEchoMode(QLineEdit::Password);
        ui->eye->setIcon(QIcon(":/image/close_eye.png"));
    }
}

void login::clickedLogin()
{

    QString username = ui->username_edit->text();
    QString plainPassword = ui->password_edit->text();
    QString hashedPassword = NekoLibro::hashPassword(plainPassword);

    if (username.isEmpty() || plainPassword.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng điền đầy đủ thông tin!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM AccountUsers WHERE username = ? AND password_hash = ? AND active = 1");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);

    if (query.exec() && query.next()) {

        ui->status_label->setText("Đăng nhập thành công.....");
        ui->status_label->setStyleSheet("color: green;");


        // Đợi 1.5 giây rồi mở màn hình chính
        QTimer::singleShot(1500, this, [this]() {
            pNekoLibroWindow = new NekoLibro();
            QString _user = ui->username_edit->text();
            pNekoLibroWindow->setCurrentUser(_user);
            pNekoLibroWindow->showUserName();
            pNekoLibroWindow->showFullName();
            pNekoLibroWindow->show();
            close(); // đóng cửa sổ đăng nhập
        });

    } else {

        // Kiểm tra xem tài khoản có tồn tại nhưng bị khóa không
        QSqlQuery checkQuery;
        checkQuery.prepare(R"(
            SELECT active FROM AccountUsers
            WHERE username = ? AND password_hash = ?
        )");
        checkQuery.addBindValue(username);
        checkQuery.addBindValue(hashedPassword);
        if (checkQuery.exec() && checkQuery.next()) {
            int active = checkQuery.value(0).toInt();
            if (active == 0) {
                QMessageBox::warning(this, "Đăng nhập", "Tài khoản của bạn đã bị khóa!");
                return;
            }
        }

        QMessageBox::warning(this, "Đăng nhập", "Tên người dùng hoặc mật khẩu không đúng!");
    }
}

void login::clickedForgetPassword(){
    if(!pResetPassWindow) {
        pResetPassWindow = new resetPassword();
    }
    pResetPassWindow->show();
}


