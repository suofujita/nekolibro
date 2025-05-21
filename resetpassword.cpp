#include "resetpassword.h"
#include "ui_resetpassword.h"
#include "login.h"
resetPassword::resetPassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::resetPassword)
{
    ui->setupUi(this);

    connect(ui->send_otp,&QPushButton::clicked,this,&resetPassword::clickedSendOTP);
    connect(ui->verify_otp,&QPushButton::clicked,this,&resetPassword::clickedVerifyOTP);
}

resetPassword::~resetPassword()
{
    delete ui;
}

void resetPassword::clickedSendOTP()
{
    QString email = ui->email_edit->text().trimmed();
    if (email.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng điền đầy đủ thông tin!");
        return;
    }
    otp = generateOTP();
    sendOTP(otp, email);
}

QString resetPassword::generateOTP()
{
    otp.clear();
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    for (int i = 0; i < 6; ++i) {
        otp.append(possibleCharacters.at(QRandomGenerator::global()->bounded(possibleCharacters.length())));

    }
    return otp;
}

void resetPassword::sendOTP(const QString &otp, const QString &email)
{
    SmtpClient smtp("smtp.gmail.com", 465, SmtpClient::SslConnection);

    // Đăng nhập bằng email và app password
    QString gmailUser = "sehunlove934@gmail.com";
    QString gmailPassword = "nuwknwnlqudjbjcf";  // App Password Gmail

    MimeMessage message;
    EmailAddress sender(gmailUser, "Neko Libro App"); // Tạo đối tượng EmailAddress cho người gửi
    message.setSender(sender); // Thiết lập người gửi
    message.addRecipient(EmailAddress(email)); // Thiết lập người nhận
    message.setSubject("Mã OTP để đặt lại mật khẩu");

    MimeText *text = new MimeText;
    text->setText("Mã OTP của bạn là: " + otp);
    message.addPart(text);

    // Kết nối đến máy chủ SMTP
    connect(&smtp, &SmtpClient::error, this, &resetPassword::onError); // Kết nối signal lỗi


   smtp.connectToHost();  // Kết nối máy chủ SMTP

    // Đăng nhập vào máy chủ SMTP (không cần kiểm tra giá trị trả về)
    smtp.login(gmailUser, gmailPassword, SmtpClient::AuthLogin);

    // Gửi email
    smtp.sendMail(message);
    smtp.quit();

    showStatusMessage("Mã OTP đã được gửi đến email của bạn.");
}


void resetPassword::onEmailSend(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        showStatusMessage("Không thể gửi email, vui lòng thử lại.");
    } else {
        showStatusMessage("Mã OTP đã được gửi đến email của bạn.");
    }
    reply->deleteLater();
}

void resetPassword::showStatusMessage(const QString &message)
{
    ui->status->setText(message);
}

void resetPassword::clickedVerifyOTP()
{
    // Lấy OTP người dùng nhập vào
    enterOTP = ui->otp_edit->text().trimmed();

    // Kiểm tra nếu mã OTP người dùng nhập vào đúng
    verifyOTP(enterOTP);
}

void resetPassword::verifyOTP(const QString &enter_OTP)
{
    if (enter_OTP == otp) {
        showStatusMessage("Xác minh mã OTP thành công.");
        // Tiến hành reset mật khẩu hoặc bước tiếp theo
    } else {
        showStatusMessage("Mã OTP không chính xác.");
    }
}

void resetPassword::onError() // Slot xử lý lỗi
{
    showStatusMessage("Đã xảy ra lỗi khi gửi email.");
}
