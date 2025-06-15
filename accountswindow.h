#ifndef ACCOUNTSWINDOW_H
#define ACCOUNTSWINDOW_H

#include <QWidget>
#include "nekolibro.h"
namespace Ui {
class AccountsWindow;
}

class AccountsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsWindow(QWidget *parent = nullptr);
    ~AccountsWindow();

public slots:
    void tabChangePassword();
    void tabUpdateUserProfile();

private slots:
    void visibleOldPassword();
    void visibleNewPassword();
    void visibleVeryfiedNewPassword();
    void changePassword();
    void updateUserProfiles();
    void tabMinimized();
private:
    Ui::AccountsWindow *ui;
    QSqlDatabase db;
    bool passwordVisible = false;
    void loadHometownList();
    void fillInfor();

};

#endif // ACCOUNTSWINDOW_H
