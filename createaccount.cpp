#include "createaccount.h"
#include "ui_createaccount.h"

createAccount::createAccount(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::createAccount)
{
    ui->setupUi(this);  /* phải ở trên đầu*/

    connect(ui->buttonBox, &QDialogButtonBox::accepted,this,&createAccount::clickedAccept);
    connect(ui->buttonBox,&QDialogButtonBox::rejected,this,&createAccount::clickedReject);
}

createAccount::~createAccount()
{
    delete ui;
}

void createAccount::clickedReject(){
    close();
}

void createAccount::clickedAccept(){
    QString new_username = ui->new_username_edit->text();
    QString new_password = ui->new_password_edit->text();
    QString new_email = ui->new_gmail_edit->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE username = ?");
    query.addBindValue(new_username);
    if(query.exec() && !query.next()) {
        query.prepare("INSERT INTO Users(username,password,email)"
                      "VALUES( ? , ? , ?)");
        query.addBindValue(new_username);
        query.addBindValue(new_password);
        query.addBindValue(new_email);

        if(query.exec()) {
            QMessageBox::information(this,"Tạo tài khoản mới","Tạo tài khoản mới thành công!");
        }
    }

}
