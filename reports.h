#ifndef REPORTS_H
#define REPORTS_H

#include <QWidget>
#include "nekolibro.h"
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QFileDialog>

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
    void selectDuarationTimeForSum(const QString &text);
    void loadBillsByRange(TimeRange range);
    void searchBillsByCustomRange();
private:
    Ui::reports *ui;
    QSqlDatabase db;

    void loadDataForDailyReports();
    void loadDataForRetailSum();
    void loadDataForRetailBills();
    void loadDataForEmployees();
    void loadDataForStocks();

    void insertDataIntoBillsTable(QString startDate, QString endDate);

};

#endif // REPORTS_H
