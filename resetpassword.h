#ifndef RESETPASSWORD_H
#define RESETPASSWORD_H

#include <QDialog>
#include "login.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRandomGenerator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSslSocket>
#include "mimemessage.h"
#include "mimemultipart.h"
#include "mimetext.h"
#include "smtpclient.h"
#include "emailaddress.h"

namespace Ui {
class resetPassword;
}

class resetPassword : public QDialog
{
    Q_OBJECT

public:
    explicit resetPassword(QWidget *parent = nullptr);
    ~resetPassword();
private slots:
    void clickedSendOTP();
    void clickedVerifyOTP();
    void onError();
private:
    Ui::resetPassword *ui;
    QString otp;
    QString enterOTP;
    QString generateOTP();
    void sendOTP(const QString &otp, const QString &email);
    void showStatusMessage(const QString &message);
    void verifyOTP(const QString &enter_OTP);
    void onEmailSend(QNetworkReply *reply);
};

#endif // RESETPASSWORD_H
