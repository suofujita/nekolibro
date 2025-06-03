#ifndef NEKOLIBRO_H
#define NEKOLIBRO_H

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QString>
#include <QTimer>
#include "database.h"
#include "login.h"
#include <QStringList>
#include <QStringListModel>
#include <QCompleter>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QStandardItem>
#include <QMenu>
#include <QToolButton>
#include <QAction>
#include <QDate>
#include <QLabel>
#include <QTimer>
#include <QScreen>
#include <QSpinBox>
#include <QCryptographicHash>
#include <QtCharts>
QT_BEGIN_NAMESPACE

class CategoriesWindow;
class SalesWindow;
class ImExport;
class reports;
class Settings;
class AccountsWindow;
class EmployeesWindow;

namespace Ui {
class NekoLibro;
}
QT_END_NAMESPACE

class NekoLibro : public QMainWindow
{
    Q_OBJECT

public:
    NekoLibro(QWidget *parent = nullptr);
    ~NekoLibro();
    QStringList getCategoriesList();
    static QString currentUser, currentFullName;
    void setCurrentUser(QString username);
    void showUserName();
    void showFullName();
    void showTime();
    QString getCurrentUser();
    static QString hashPassword(const QString &password);
    static int getAuthorId(const QString &authorName);
    static int getCategoryId(const QString &categoryName);
private slots:
    void clickedLogOut();

    void openSalesWindow();
    void openEmployeesWindow();
    void openCategoriesWindow();
    void openImExportWindow();
    void openReportWindow();
    void openSettingsWindow();
    void openAccountsWindow();

    void gotoAddBook();
    void gotoImportInvoice();
    void gotoExportInvoice();
    void gotoImportLogs();
    void gotoExportLogs();

private:
    Ui::NekoLibro *ui;
    login *pLogin = nullptr;

    SalesWindow *pSaleWindow = nullptr;
    EmployeesWindow *pEmployeesWindow = nullptr;
    CategoriesWindow *pCategoriesWindow = nullptr;
    ImExport *pImExportWindow = nullptr;
    reports *pReportWindow = nullptr;
    Settings *pSettingWindow = nullptr;
    AccountsWindow *pAccountsWindow = nullptr;
    QList<QWidget*> childWindows;  // lưu tất cả các cửa sổ con của trang chủ để thuận lợi
                                   // cho việc giải phóng vùng nhớ đăng xuất
    QSqlDatabase db;
    QMenu *menuCategories = nullptr;
    QMenu *menuImExport = nullptr;
    QTimer *time;

    void updateStock();
    void clearChildWindows();  // Xóa các cửa sổ con

};
#endif // NEKOLIBRO_H
