#ifndef REPORTS_H
#define REPORTS_H

#include <QWidget>
#include "nekolibro.h"
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QFileDialog>
#include <xlsxdocument.h>
#include <xlsxformat.h>
#include <xlsxworksheet.h>
#include "viewinvoicesdetails.h"

using namespace QXlsx;

enum class TimeRange {
    Today,
    Yesterday,
    ThisWeek,
    LastWeek,
    ThisMonth,
    LastMonth,
    Recent3Months,
    Recent6Months,
    ThisYear,
    LastYear
};

enum class RevenueRange {
    Last7Days,
    Last14Days,
    ThisMonth,
    LastMonth,
    Last3Months,
    Last6Months,
    ThisYear,
    LastYear
};


namespace Ui {
class reports;
}

class reports : public QWidget
{
    Q_OBJECT

public:
    explicit reports(QWidget *parent = nullptr);
    ~reports();
private slots:
    void onTabChanged(int currIndex);
    void saveChartAsPNG();
    void exportBillsToExcel();
    /* hóa đơn bán lẻ*/
    void loadBillsByRange(TimeRange range);
    void searchBillsByCustomRange();
    void clickedRetailBillNum(int row, int col);
    /* doanh thu tổng hợp*/
    void createRevenueChart(RevenueRange range);
    void RevenueChartByCustomRange();
private:
    Ui::reports *ui;
    QSqlDatabase db;
    ViewInvoicesDetails *pViewInvoicesDetails = nullptr;


    void loadDataForDailyReports();
    void loadDataForEmployees();
    void loadDataForStocks();

    void RetailInvoiceDetails(int row);  // xem chi tiết đơn hàng
    void insertDataIntoBillsTable(QString startDate, QString endDate, QString seller);
};

#endif // REPORTS_H
