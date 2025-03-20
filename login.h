#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QSql>
#include <QSqlQuery>
#include <QTimer>

class createAccount;
class NekoLibro;
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
private slots:
    void clickedLogin();
    void clickedCreateAccount();
private:
    Ui::login *ui;
    createAccount *pCreateWindow = nullptr;
    NekoLibro *pNekoLibroWindow = nullptr;
};

#endif // LOGIN_H
