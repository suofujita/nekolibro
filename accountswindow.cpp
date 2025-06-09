#include "accountswindow.h"
#include "ui_accountswindow.h"

AccountsWindow::AccountsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AccountsWindow)
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

    setWindowTitle("Quản lý tài khoản - Neko Libro");
    setWindowIcon(QIcon(":/image/cat.png"));

    /* signal slots*/
    connect(ui->save_infor, &QPushButton::clicked,this,&AccountsWindow::updateUserProfiles);
    connect(ui->cancel_infor, &QPushButton::clicked,this,&AccountsWindow::tabMinimized);
    connect(ui->save_pass, &QPushButton::clicked,this,&AccountsWindow::changePassword);
    connect(ui->cancel_pass, &QPushButton::clicked,this,&AccountsWindow::tabMinimized);
    connect(ui->vision_old, &QPushButton::clicked,this,&AccountsWindow::visibleOldPassword);
    connect(ui->vision_new,&QPushButton::clicked,this,&AccountsWindow::visibleNewPassword);
    connect(ui->vision_again,&QPushButton::clicked,this,&AccountsWindow::visibleVeryfiedNewPassword);

    /* design */
    ui->update_logo->setFixedSize(200,200);
    ui->name_tag->setFixedSize(50,50);
    ui->phone_tag->setFixedSize(50,50);
    ui->dob_tag->setFixedSize(50,50);
    ui->hometown_tag->setFixedSize(50,50);
    ui->email_tag->setFixedSize(50,50);

    fillEmail();
}

AccountsWindow::~AccountsWindow()
{
    delete ui;
}

void AccountsWindow::tabChangePassword(){
    ui->stackedWidget->setCurrentIndex(0);
}

void AccountsWindow::tabUpdateUserProfile(){
    ui->stackedWidget->setCurrentIndex(1);
    ui->dob->setDate(QDate::currentDate());
    loadHometownList();
}

void AccountsWindow::tabMinimized(){
    showMinimized();
}

void AccountsWindow::updateUserProfiles(){
    /* Chèn dữ liệu vào bảng AccountUsers */
    QString currentUser = NekoLibro::currentUser;
    QString fullname = ui->fullname->text().trimmed();
    QString email = ui->email->text().trimmed();
    QString dob = ui->dob->date().toString("yyyy-MM-dd");
    QString hometown = ui->hometown->currentText();
    QString phone = ui->phone->text().trimmed();
    QSqlQuery query;

    query.prepare("UPDATE AccountUsers SET fullname = ?, email = ?, date_of_birth = ?, hometown = ?, phone = ? "
                  "WHERE username = ?");
    query.addBindValue(fullname);
    query.addBindValue(email);
    query.addBindValue(dob);
    query.addBindValue(hometown);
    query.addBindValue(phone);
    query.addBindValue(currentUser);

    if(!query.exec()){
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    else {
         QMessageBox::information(this, "Thành công", "Đã cập nhật thông tin thành công!");
        this->close();
    }
}

void AccountsWindow::changePassword(){
    /* Kiểm tra mật khẩu cũ đúng không?*/
    /* Kiểm tra mật khẩu mới có trùng với mật khẩu cũ */
    /* Kiểm tra nhập lại mật khẩu mới đúng không*/
    QString currentUser = NekoLibro::currentUser;
    QString plainPassword = ui->old_password->text().trimmed();
    QString hashedPassword = NekoLibro::hashPassword(plainPassword);
    QString hashedOldPassword;

    QSqlQuery query;
    query.prepare("SELECT password_hash FROM AccountUsers WHERE username = ?");
    query.addBindValue(currentUser);

    if(!query.exec()){
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }

    if(!query.next()){
        QMessageBox::critical(this, "Lỗi", "Tài khoản không tồn tại!");
        return;
    }

    hashedOldPassword = query.value(0).toString();

    if (hashedPassword != hashedOldPassword)  {
        QMessageBox::critical(this, "Lỗi","Mật khẩu không đúng!");
        return;
    }

    QString newPassword = ui->new_password->text().trimmed();
    QString hashedNewPassword = NekoLibro::hashPassword(newPassword);
    QString typeNewPasswordAgain = ui->type_again->text().trimmed();
    QString hashedNewPasswordAgain = NekoLibro::hashPassword(typeNewPasswordAgain);

    if(hashedNewPassword == hashedOldPassword){
        QMessageBox::critical(this, "Lỗi","Mật khẩu không thay đổi!");
        return;
    }

    if(hashedNewPassword != hashedNewPasswordAgain) {
        QMessageBox::critical(this, "Lỗi","Mật khẩu nhập lại không trùng với mật khẩu mới!");
        return;
    }

    query.prepare("UPDATE AccountUsers SET password_hash = ? WHERE username = ?");
    query.addBindValue(hashedNewPassword);
    query.addBindValue(currentUser);

    if(!query.exec()){
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }
    else {
        QMessageBox::information(this, "Thành công", "Đã cập nhật mật khẩu mới!");
        ui->fullname->clear();
        ui->phone->clear();
        this->close();
    }
}

/* Sử dụng chatGPT sinh code phần này */
void AccountsWindow::loadHometownList(){
    QStringList hometownList = {
        "An Giang", "Bà Rịa - Vũng Tàu", "Bắc Giang", "Bắc Kạn", "Bạc Liêu",
        "Bắc Ninh", "Bến Tre", "Bình Định", "Bình Dương", "Bình Phước",
        "Bình Thuận", "Cà Mau", "Cần Thơ", "Cao Bằng", "Đà Nẵng",
        "Đắk Lắk", "Đắk Nông", "Điện Biên", "Đồng Nai", "Đồng Tháp",
        "Gia Lai", "Hà Giang", "Hà Nam", "Hà Nội", "Hà Tĩnh",
        "Hải Dương", "Hải Phòng", "Hậu Giang", "Hòa Bình", "Hưng Yên",
        "Khánh Hòa", "Kiên Giang", "Kon Tum", "Lai Châu", "Lâm Đồng",
        "Lạng Sơn", "Lào Cai", "Long An", "Nam Định", "Nghệ An",
        "Ninh Bình", "Ninh Thuận", "Phú Thọ", "Phú Yên", "Quảng Bình",
        "Quảng Nam", "Quảng Ngãi", "Quảng Ninh", "Quảng Trị", "Sóc Trăng",
        "Sơn La", "Tây Ninh", "Thái Bình", "Thái Nguyên", "Thanh Hóa",
        "Thừa Thiên Huế", "Tiền Giang", "TP. Hồ Chí Minh", "Trà Vinh", "Tuyên Quang",
        "Vĩnh Long", "Vĩnh Phúc", "Yên Bái"
    };

    ui->hometown->clear(); // Xóa dữ liệu cũ nếu có
    ui->hometown->addItems(hometownList);
}

void AccountsWindow::visibleOldPassword(){
    passwordVisible = !passwordVisible;

    if (passwordVisible) {
        ui->old_password->setEchoMode(QLineEdit::Normal);
        ui->vision_old->setIcon(QIcon(":/image/eye.png"));
    } else {
        ui->old_password->setEchoMode(QLineEdit::Password);
        ui->vision_old->setIcon(QIcon(":/image/close_eye.png"));
    }
}
void AccountsWindow::visibleNewPassword(){
    passwordVisible = !passwordVisible;

    if (passwordVisible) {
        ui->new_password->setEchoMode(QLineEdit::Normal);
        ui->vision_new->setIcon(QIcon(":/image/eye.png"));
    } else {
        ui->new_password->setEchoMode(QLineEdit::Password);
        ui->vision_new->setIcon(QIcon(":/image/close_eye.png"));
    }
}
void AccountsWindow::visibleVeryfiedNewPassword(){
    passwordVisible = !passwordVisible;

    if (passwordVisible) {
        ui->type_again->setEchoMode(QLineEdit::Normal);
        ui->vision_again->setIcon(QIcon(":/image/eye.png"));
    } else {
        ui->type_again->setEchoMode(QLineEdit::Password);
        ui->vision_again->setIcon(QIcon(":/image/close_eye.png"));
    }
}

void AccountsWindow::fillEmail(){
    QSqlQuery query;
    /* điền trước email cho người dùng đỡ phải điền nếu không thay đổi */
    query.prepare("SELECT email FROM AccountUsers WHERE username = ?");
    query.addBindValue(NekoLibro::currentUser);

    if(!query.exec()) {
        QMessageBox::critical(this, "Lỗi", "Không thể mở cơ sở dữ liệu!");
        return;
    }

    if(!query.next()){
        QMessageBox::critical(this, "Lỗi", "Tài khoản không tồn tại!");
        return;
    }

    QString currentEmail = query.value(0).toString();
    ui->email->setText(currentEmail);
}
