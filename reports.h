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

enum class ProductReportType {
    Top5BestSellers,      // 5 sản phẩm bán chạy nhất
    UpcomingSoldOut,      // Sắp hết hàng
    Overstocked           // Tồn kho quá nhiều
};

enum class ReportRange {
    Daily,
    Monthly,
    Quarterly,
    Yearly
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

    void showAllProducts();
    void sortByStock();
    void loadDataForStocks(ProductReportType type);

    void exportInventoryToExcel();
private:
    Ui::reports *ui;
    QSqlDatabase db;
    ViewInvoicesDetails *pViewInvoicesDetails = nullptr;
    bool isIncreasing = false;

    void loadDataForDailyReports();
    void loadDataForEmployees();

    void RetailInvoiceDetails(int row);  // xem chi tiết đơn hàng
    void insertDataIntoBillsTable(QString startDate, QString endDate, QString seller);

    void loadSellerCombox();
    void stockUI();
    void convertToProductReportType();

    void loadtop5bestseller();
    void loadUpcomingSoldout();
    void loadOverStocked();

};

#endif // REPORTS_H
