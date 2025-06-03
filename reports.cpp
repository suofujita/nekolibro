#include "reports.h"
#include "ui_reports.h"

reports::reports(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::reports)
{
    ui->setupUi(this);
    /* Mở csdl */
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection");
        db.setDatabaseName("nekolibro.db");
    }

    if (!db.isOpen()) {
        if (!db.open()) {
            qDebug() << "Lỗi mở cơ sở dữ liệu:" << db.lastError().text();
        } else {
            qDebug() << "Mở cơ sở dữ liệu thành công!";
        }
    }

    connect(ui->tabWidget, &QTabWidget::currentChanged,this,&reports::onTabChanged);
    connect(ui->png_daily, &QPushButton::clicked,this,&reports::saveChartAsPNG);
    connect(ui->select_time_for_sum, &QComboBox::currentTextChanged,this,&reports::selectDuarationTimeForSum);
    connect(ui->search_bill, &QPushButton::clicked,this, &reports::searchBillsByCustomRange);
    /* cần ánh xạ chuỗi thành Kiểu TimeRange */
    connect(ui->select_time_bill, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        TimeRange range;

        if (text == "Hôm nay")
            range = TimeRange::Today;
        else if (text == "Hôm qua")
            range = TimeRange::Yesterday;
        else if (text == "Tuần này")
            range = TimeRange::ThisWeek;
        else if (text == "Tuần trước")
            range = TimeRange::LastWeek;
        else if (text == "Tháng này")
            range = TimeRange::ThisMonth;
        else if (text == "Tháng trước")
            range = TimeRange::LastMonth;
        else if (text == "3 tháng gần đây")
            range = TimeRange::Recent3Months;
        else if (text == "6 tháng gần đây")
            range = TimeRange::Recent6Months;
        else if (text == "Năm nay")
            range = TimeRange::ThisYear;
        else if (text == "Năm trước")
            range = TimeRange::LastYear;
        else
            return; // không làm gì nếu không khớp

        loadBillsByRange(range);
    });

    /* truyền 1 tham số hàm */
    connect(ui->daily_bills, &QPushButton::clicked, this, [=]() {
        loadBillsByRange(TimeRange::Today);
    });

    /* Hiện thị dữ liệu */
    ui->view_bills->setColumnCount(5);
    ui->view_bills->setHorizontalHeaderLabels({"Ngày lập", "Mã hóa đơn","Số lượng", "Tổng hóa đơn", "Người bán"});
    ui->view_bills->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerExport = ui->view_bills->horizontalHeader();
    headerExport->setSectionResizeMode(1, QHeaderView::Stretch);  // kích thước ô tự động giãn theo nội dung
    headerExport->setSectionResizeMode(0, QHeaderView::Fixed);    // cố định fixed
    headerExport->setSectionResizeMode(2, QHeaderView::Fixed);    // cố định fixed
    headerExport->setSectionResizeMode(3, QHeaderView::Fixed);    // cố định fixed
    headerExport->setSectionResizeMode(4, QHeaderView::Fixed);    // cố định fixed
    ui->view_bills->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->view_bills->setColumnWidth(0, 200); // ngày
    ui->view_bills->setColumnWidth(2, 100); // số lượng
    ui->view_bills->setColumnWidth(3, 200); // tổng hóa đơn
    ui->view_bills->setColumnWidth(4, 300); // người bán
    ui->view_bills->horizontalHeader()->setStretchLastSection(false);

    loadDataForDailyReports();
    loadDataForRetailSum();
    loadDataForRetailBills();
    loadDataForEmployees();
    loadDataForStocks();
    loadBillsByRange(TimeRange::Today);
    ui->start_date_bill->setDate(QDate::currentDate());
    ui->end_date_bill->setDate(QDate::currentDate());
}

reports::~reports()
{
    delete ui;
}

void reports::onTabChanged(int currIndex){
    switch(currIndex){
        case 0:
                loadDataForDailyReports(); break;
        case 1:
            loadDataForRetailSum(); break;
        case 2:
            loadDataForRetailBills(); break;
        case 3:
            loadDataForEmployees(); break;
        case 4:
            loadDataForStocks(); break;
    }
}

void reports::loadDataForDailyReports() {
    QBarSet *barSet = new QBarSet("Biểu đồ doanh thu theo ngày");
    QStringList categories;

    QSqlQuery query;

    // 1. Lấy tổng số đơn và tổng doanh thu trong ngày
    query.prepare("SELECT COUNT(*), SUM(total_bill) FROM RetailInvoices "
                  "WHERE date = date('now')");
    if (query.exec() && query.next()) {
        QLocale locale = QLocale::Vietnamese;
        double sum = query.value(1).toDouble();
        QString formattedSum = locale.toString(sum, 'f', 0);

        ui->total_bills->setText(query.value(0).toString());
        ui->daily_sum->setText("<b>" + formattedSum + "</b>");

        *barSet << sum;
        categories << "TỔNG";
    } else {
        QMessageBox::critical(this, "Lỗi", "Không thể lấy dữ liệu sản phẩm: " + query.lastError().text());
        return;
    }

    // 2. Lấy doanh thu từng nhân viên trong ngày
    query.prepare("SELECT AccountUsers.id, AccountUsers.fullname, "
                  "COALESCE(SUM(RetailInvoices.total_bill), 0), COUNT(RetailInvoices.id) "
                  "FROM AccountUsers "
                  "LEFT JOIN RetailInvoices ON AccountUsers.id = RetailInvoices.user_id "
                  "AND RetailInvoices.date = date('now') "
                  "GROUP BY AccountUsers.id, AccountUsers.fullname "
                  "ORDER BY COALESCE(SUM(RetailInvoices.total_bill), 0) ASC");

    if (!query.exec()) {
        QMessageBox::critical(this, "Lỗi truy vấn", query.lastError().text());
        return;
    }

    while (query.next()) {
        double revenue = query.value(2).toDouble();
        int billCount = query.value(3).toInt();
        QString fullname = query.value(1).toString();

        *barSet << revenue;
        QString label = QString("%1 (%2 đơn)").arg(fullname).arg(billCount);
        categories << label;
    }

    // 3. Vẽ biểu đồ
    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    series->append(barSet);
    series->setLabelsVisible(false); // tắt đi dùng cách khác cách này hiện thị doanh thu xxe+yy???
    series->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Doanh thu theo ngày");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Trục Y: tên nhân viên
    QBarCategoryAxis *axisY = new QBarCategoryAxis();
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Trục X: số tiền
    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelFormat("%.0f");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->view_daily_report->setChart(chart);
    ui->view_daily_report->setRenderHint(QPainter::Antialiasing);
}

void reports::loadDataForRetailSum(){

}

void reports::loadDataForRetailBills(){

}

void reports::loadDataForEmployees(){

}

void reports::loadDataForStocks(){

}

void reports::saveChartAsPNG() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Lưu biểu đồ dưới dạng ảnh"), "",
                                                    tr("PNG Image (*.png);;JPEG Image (*.jpg);;BMP Image (*.bmp)"));

    if (filename.isEmpty())
        return; // Người dùng hủy chọn

    QPixmap pixmap = ui->view_daily_report->grab(); // Chụp ảnh widget chart
    if (!pixmap.save(filename)) {
        QMessageBox::warning(this, tr("Lỗi"), tr("Không thể lưu file ảnh."));
    }
    else {
        QMessageBox::information(this, tr("Thành công"), tr("Lưu biểu đồ thành công!"));
    }
}

void reports::loadBillsByRange(TimeRange range) {
    QString startDate, endDate;
    QDate today = QDate::currentDate();
    /* Khi có range thì sẽ setup được start_date, end_date*/
    switch (range) {
    case TimeRange::Today:
        startDate = endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::Yesterday:
        startDate = endDate = today.addDays(-1).toString("yyyy-MM-dd");
        break;
    case TimeRange::ThisWeek:
        startDate = today.addDays(-today.dayOfWeek() + 1).toString("yyyy-MM-dd");
        endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::LastWeek:
        startDate = today.addDays(-today.dayOfWeek() - 6).toString("yyyy-MM-dd");
        endDate = today.addDays(-today.dayOfWeek()).toString("yyyy-MM-dd");
        break;
    case TimeRange::ThisMonth:
        startDate = QDate(today.year(), today.month(), 1).toString("yyyy-MM-dd");
        endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::LastMonth: {
        QDate firstDayThisMonth(today.year(), today.month(), 1);
        QDate lastMonthDate = firstDayThisMonth.addDays(-1);
        startDate = QDate(lastMonthDate.year(), lastMonthDate.month(), 1).toString("yyyy-MM-dd");
        endDate = lastMonthDate.toString("yyyy-MM-dd");
        break;
    }
    case TimeRange::Recent3Months:
        startDate = today.addMonths(-3).toString("yyyy-MM-dd");
        endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::Recent6Months:
        startDate = today.addMonths(-6).toString("yyyy-MM-dd");
        endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::ThisYear:
        startDate = QDate(today.year(), 1, 1).toString("yyyy-MM-dd");
        endDate = today.toString("yyyy-MM-dd");
        break;
    case TimeRange::LastYear:
        startDate = QDate(today.year() - 1, 1, 1).toString("yyyy-MM-dd");
        endDate = QDate(today.year() - 1, 12, 31).toString("yyyy-MM-dd");
        break;
    }
    insertDataIntoBillsTable(startDate,endDate);
}

void reports::selectDuarationTimeForSum(const QString &text) {
    if (text == "Hôm nay") {
        loadBillsByRange(TimeRange::Today);
    } else if (text == "Hôm qua") {
        loadBillsByRange(TimeRange::Yesterday);
    } else if (text == "Tuần này") {
        loadBillsByRange(TimeRange::ThisWeek);
    } else if (text == "Tuần trước") {
        loadBillsByRange(TimeRange::LastWeek);
    } else if (text == "Tháng này") {
        loadBillsByRange(TimeRange::ThisMonth);
    } else if (text == "Tháng trước") {
        loadBillsByRange(TimeRange::LastMonth);
    } else if (text == "3 tháng gần đây") {
        loadBillsByRange(TimeRange::Recent3Months);
    } else if (text == "6 tháng gần đây") {
        loadBillsByRange(TimeRange::Recent6Months);
    } else if (text == "Năm nay") {
        loadBillsByRange(TimeRange::ThisYear);
    } else if (text == "Năm trước") {
        loadBillsByRange(TimeRange::LastYear);
    }
}

void reports::searchBillsByCustomRange(){
    QString startDate = ui->start_date_bill->date().toString("yyyy-MM-dd");
    QString endDate = ui->end_date_bill->date().toString("yyyy-MM-dd");

    // Kiểm tra nếu start > end thì báo lỗi
    if (QDate::fromString(startDate, "yyyy-MM-dd") > QDate::fromString(endDate, "yyyy-MM-dd")) {
        QMessageBox::warning(this, "Lỗi", "Ngày bắt đầu phải trước hoặc bằng ngày kết thúc.");
        return;
    }

    insertDataIntoBillsTable(startDate,endDate);
}

void reports::insertDataIntoBillsTable(QString startDate, QString endDate){
    /* Lấy dữ liệu từ CSDL chèn vào bảng */
    QSqlQuery query;
    query.prepare("SELECT R.date, R.bill_num, R.total_quanties, R.total_bill, AccountUsers.username "
                  "FROM RetailInvoices R "
                  "JOIN AccountUsers ON AccountUsers.id = R.user_id"
                  " WHERE date BETWEEN :start AND :end");
    query.bindValue(":start", startDate);
    query.bindValue(":end", endDate);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Lỗi truy vấn", query.lastError().text());
        return;
    }

    ui->view_bills->setRowCount(0);
    int row = 0;

    while (query.next()) {
        ui->view_bills->insertRow(row);

        QString date = query.value(0).toString();
        QString billNum = query.value(1).toString();
        int totalQuantity = query.value(2).toInt();
        double totalBill = query.value(3).toDouble();
        QString seller = query.value(4).toString();

        ui->view_bills->setItem(row, 0, new QTableWidgetItem(date));
        ui->view_bills->setItem(row, 1, new QTableWidgetItem(billNum));
        ui->view_bills->setItem(row, 2, new QTableWidgetItem(QString::number(totalQuantity)));
        ui->view_bills->setItem(row, 3, new QTableWidgetItem(QString::number(totalBill)));
        ui->view_bills->setItem(row, 4, new QTableWidgetItem(seller));

        row++;
    }
}

