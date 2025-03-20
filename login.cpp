#include "login.h"
#include "ui_login.h"

#include "createaccount.h"
#include "nekolibro.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/cat.png"));
    setWindowTitle("Welcome to Neko Libro");

    connect(ui->login_button,&QPushButton::clicked,this,&login::clickedLogin);
    connect(ui->create_account_button, &QPushButton::clicked,this,&login::clickedCreateAccount);
}

login::~login()
{
    delete ui;
}

void login::clickedLogin()
{
    QString username = ui->username_edit->text();
    QString password = ui->password_edit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng điền đầy đủ thông tin!");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {

        ui->status_label->setText("Đăng nhập thành công.....");
        ui->status_label->setStyleSheet("color: green;");

        // Đợi 1.5 giây rồi mở màn hình chính
        QTimer::singleShot(1500, this, [this]() {
            pNekoLibroWindow = new NekoLibro();
            pNekoLibroWindow->show();
            close(); // đóng cửa sổ đăng nhập
        });

    } else {
        QMessageBox::warning(this, "Đăng nhập", "Tên người dùng hoặc mật khẩu không đúng!");
    }
}

void login::clickedCreateAccount() {
    if (!pCreateWindow) { // Kiểm tra xem cửa sổ đã tồn tại chưa
        pCreateWindow = new createAccount(this);
    }

    QString username = ui->username_edit->text();
    QString password = ui->password_edit->text();

    QSqlQuery query;
    query.prepare("SELECT isAdmin FROM Users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        int isAdmin = query.value("isAdmin").toInt();
        if (isAdmin == 1) {  // Người dùng là admin
            pCreateWindow = new createAccount(this);
            pCreateWindow->show();
        } else {
            QMessageBox::warning(this, "Lỗi", "Bạn không có quyền tạo tài khoản!");    // người dùng có tài khoản nhưng không có quyền truy cập
        }
    } else {
        QMessageBox::warning(this, "Lỗi", "Đăng nhập không hợp lệ!");    // người dùng chưa có tài khoản
    }
}




