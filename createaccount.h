#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include <QDialog>
#include <Qstring>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include "nekolibro.h"
namespace Ui {
class createAccount;
}

class createAccount : public QDialog
{
    Q_OBJECT

public:
    explicit createAccount(QWidget *parent = nullptr);
    ~createAccount();

private slots:
    void clickedAccept();
    void clickedReject();
private:
    Ui::createAccount *ui;
};

#endif // CREATEACCOUNT_H
