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

    ui->tabWidget->setCurrentIndex(0);

    connect(ui->tabWidget, &QTabWidget::currentChanged,this,&reports::onTabChanged);
    connect(ui->png_daily, &QPushButton::clicked,this,&reports::saveChartAsPNG);
    connect(ui->search_bill, &QPushButton::clicked,this, &reports::searchBillsByCustomRange);  // tìm hóa đơn thời gian tùy chỉnh
    connect(ui->search_sum, &QPushButton::clicked,this,&reports::RevenueChartByCustomRange);  // vẽ biểu đồ thời gian tùy chỉnh
    connect(ui->excel_bill,&QPushButton::clicked,this,&reports::exportBillsToExcel);
    connect(ui->view_bills,&QTableWidget::cellPressed, this, &reports::clickedRetailBillNum);
    connect(ui->png_sum,&QPushButton::clicked,this,&reports::saveChartAsPNG);
    connect(ui->all_stocks, &QPushButton::clicked,this,&reports::showAllProducts);
    connect(ui->sort, &QPushButton::clicked, this, &reports::sortByStock);
    connect(ui->search_employee,&QPushButton::clicked,this, &reports::loadDataForEmployees);

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


    /* cần ánh xạ chuỗi thành Kiểu RevenueRange */
    connect(ui->select_time_for_sum, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        RevenueRange range;

        if (text == "7 ngày qua")
            range = RevenueRange::Last7Days;
        else if (text == "14 ngày qua")
            range = RevenueRange::Last14Days;
        else if (text == "Tháng này")
            range = RevenueRange::ThisMonth;
        else if (text == "Tháng trước")
            range = RevenueRange::LastMonth;
        else if (text == "3 tháng gần đây")
            range = RevenueRange::Last3Months;
        else if (text == "6 tháng gần đây")
            range = RevenueRange::Last6Months;
        else if (text == "Năm nay")
            range = RevenueRange::ThisYear;
        else if (text == "Năm trước")
            range = RevenueRange::LastYear;
        else
            return; // không làm gì nếu không khớp

        createRevenueChart(range);
    });

    convertToProductReportType();

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
    stockUI();
    loadDataForDailyReports();
    createRevenueChart(RevenueRange::Last7Days);
    loadBillsByRange(TimeRange::Today);
    ui->start_date_bill->setDate(QDate::currentDate());
    ui->end_date_bill->setDate(QDate::currentDate());
    ui->start_date_sum->setDate(QDate::currentDate());
    ui->end_date_sum->setDate(QDate::currentDate());

    loadDataForStocks(ProductReportType::Top5BestSellers); // cho combobox
    loadSellerCombox();
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
            createRevenueChart(RevenueRange::Last7Days); break;
        case 2:
            loadBillsByRange(TimeRange::Today); break;
        case 3:
            loadDataForEmployees(); break;
        case 4:
            loadDataForStocks(ProductReportType::Top5BestSellers); break;
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

void reports::loadDataForStocks(ProductReportType type){

    switch(type) {
    case ProductReportType::Top5BestSellers:
        loadtop5bestseller(); break;
    case ProductReportType::UpcomingSoldOut:
        loadUpcomingSoldout(); break;
    case ProductReportType::Overstocked:
        loadOverStocked(); break;
    }
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

    QString seller = ui->seller_1->currentText();

    insertDataIntoBillsTable(startDate,endDate,seller);
}

void reports::searchBillsByCustomRange(){
    QString startDate = ui->start_date_bill->date().toString("yyyy-MM-dd");
    QString endDate = ui->end_date_bill->date().toString("yyyy-MM-dd");

    // Kiểm tra nếu start > end thì báo lỗi
    if (QDate::fromString(startDate, "yyyy-MM-dd") > QDate::fromString(endDate, "yyyy-MM-dd")) {
        QMessageBox::warning(this, "Lỗi", "Ngày bắt đầu phải trước hoặc bằng ngày kết thúc.");
        return;
    }

    QString seller = ui->seller_2->currentText();

    insertDataIntoBillsTable(startDate,endDate,seller);
}

void reports::insertDataIntoBillsTable(QString startDate, QString endDate, QString seller){
    QSqlQuery query;
    /* Lấy dữ liệu từ CSDL chèn vào bảng */
    if (seller == "Tất cả")
    {
        query.prepare("SELECT R.date, R.bill_num, R.total_quanties, R.total_bill, AccountUsers.username "
                      "FROM RetailInvoices R "
                      "JOIN AccountUsers ON AccountUsers.id = R.user_id "
                      "WHERE date BETWEEN :start AND :end");
        query.bindValue(":start", startDate);
        query.bindValue(":end", endDate);
    }
    else
    {
        query.prepare("SELECT R.date, R.bill_num, R.total_quanties, R.total_bill, AccountUsers.username "
                      "FROM RetailInvoices R "
                      "JOIN AccountUsers ON AccountUsers.id = R.user_id "
                      "WHERE date BETWEEN :start AND :end AND AccountUsers.username = :seller");
        query.bindValue(":start", startDate);
        query.bindValue(":end", endDate);
        query.bindValue(":seller",seller);
    }

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

void reports::exportBillsToExcel() {
    QString startDate = ui->start_date_bill->date().toString("yyyy-MM-dd");
    QString endDate = ui->end_date_bill->date().toString("yyyy-MM-dd");
    QString seller = ui->seller_2->currentText();

    QSqlQuery query;
    /* Lấy dữ liệu từ CSDL chèn vào bảng */
    if (seller == "Tất cả")
    {
        query.prepare("SELECT R.date, R.bill_num, P.title, P.selling_price, I.quantity, (P.selling_price*I.quantity), AccountUsers.username "
                      "FROM RetailInvoices R "
                      "JOIN AccountUsers ON AccountUsers.id = R.user_id "
                      "JOIN RetailInvoicesItems I ON I.invoice_id = R.id "
                      "JOIN Products P ON P.id = I.product_id "
                      "WHERE date BETWEEN :start AND :end");
        query.bindValue(":start", startDate);
        query.bindValue(":end", endDate);
    }
    else
    {
        query.prepare("SELECT R.date, R.bill_num, P.title, P.selling_price, I.quantity, (P.selling_price*I.quantity), AccountUsers.username "
                      "FROM RetailInvoices R "
                      "JOIN AccountUsers ON AccountUsers.id = R.user_id "
                      "JOIN RetailInvoicesItems I ON I.invoice_id = R.id "
                      "JOIN Products P ON P.id = I.product_id "
                      "WHERE date BETWEEN :start AND :end AND AccountUsers.username = :seller");
        query.bindValue(":start", startDate);
        query.bindValue(":end", endDate);
        query.bindValue(":seller",seller);
    }

    if ( query.exec() && !query.next()) {
        QMessageBox::warning(this, "Lỗi", "Không có sản phẩm nào trong khoảng thời gian này để xuất.");
        return;
    }

    QXlsx::Document xlsx;
     /* hàng 1 là hàng tiêu đề */
    // Tiêu đề cột
    QStringList headers = { "Ngày", "Mã hóa đơn", "Tên sản phẩm", "Giá bán", "Số lượng", "Thành tiền", "Người bán"};
    for (int col = 0; col < headers.size(); ++col) {
        xlsx.write(1, col + 1, headers[col]);
    }
    /* chèn bắt đầu từ hàng 2 */
    int row = 2;
    do {
        xlsx.write(row, 1, query.value(0).toString());
        xlsx.write(row, 2, query.value(1).toString());
        xlsx.write(row, 3, query.value(2).toString());
        xlsx.write(row, 4, query.value(3).toDouble());
        xlsx.write(row, 5, query.value(4).toInt());
        xlsx.write(row, 6, query.value(5).toDouble());
        xlsx.write(row, 7, query.value(6).toString());
        row++;
    } while (query.next());

    QString filePath = QFileDialog::getSaveFileName(this, "Lưu file Excel", "", "Excel Files (*.xlsx)");
    if (!filePath.isEmpty()) {
        if (xlsx.saveAs(filePath)) {
            QMessageBox::information(this, "Thành công", "Đã xuất dữ liệu thành công.");
        } else {
            QMessageBox::critical(this, "Thất bại", "Không thể lưu file Excel.");
        }
    }
}

void reports::clickedRetailBillNum(int row, int col){
    if(col == 1) {
        RetailInvoiceDetails(row);
    }
}

void reports::RetailInvoiceDetails(int row)
{
    QString invoiceId ;
    QString billNum = ui->view_bills->item(row,1)->text();
    QSqlQuery query;
    query.prepare("SELECT id FROM RetailInvoices WHERE bill_num = ?");
    query.addBindValue(billNum);
    query.exec();
    if(query.next()){
        invoiceId = query.value(0).toString();
    }
    pViewInvoicesDetails = new ViewInvoicesDetails(invoiceId, InvoiceType::Retail, this);
    pViewInvoicesDetails->exec();
    delete pViewInvoicesDetails;
    pViewInvoicesDetails = nullptr;
}

void reports::createRevenueChart(RevenueRange range) {
    QDate today = QDate::currentDate();
    QDate startDate, endDate = today;
    QString title;

    /* Chia trường hợp lấy range ánh xạ từ combox */
    switch (range) {
    case RevenueRange::Last7Days:
        startDate = today.addDays(-6);
        title = "Doanh thu 7 ngày qua";
        break;
    case RevenueRange::Last14Days:
        startDate = today.addDays(-13);
        title = "Doanh thu 14 ngày qua";
        break;
    case RevenueRange::ThisMonth:
        startDate = QDate(today.year(), today.month(), 1);
        title = "Doanh thu tháng này";
        break;
    case RevenueRange::LastMonth: {
        QDate firstThisMonth(today.year(), today.month(), 1);
        QDate lastMonthDate = firstThisMonth.addMonths(-1);
        startDate = QDate(lastMonthDate.year(), lastMonthDate.month(), 1);
        endDate = firstThisMonth.addDays(-1);
        title = "Doanh thu tháng trước";
        break;
    }
    case RevenueRange::Last3Months:
        startDate = today.addMonths(-2);  // bao gồm tháng này
        startDate = QDate(startDate.year(), startDate.month(), 1);
        title = "Doanh thu 3 tháng gần đây";
        break;
    case RevenueRange::Last6Months:
        startDate = today.addMonths(-5);
        startDate = QDate(startDate.year(), startDate.month(), 1);
        title = "Doanh thu 6 tháng gần đây";
        break;
    case RevenueRange::ThisYear:
        startDate = QDate(today.year(), 1, 1);
        title = "Doanh thu năm nay";
        break;
    case RevenueRange::LastYear:
        startDate = QDate(today.year() - 1, 1, 1);
        endDate = QDate(today.year() - 1, 12, 31);
        title = "Doanh thu năm trước";
        break;
    }

    // Xác định group theo ngày hoặc theo tháng
    QString groupFormat;
    if (range == RevenueRange::Last7Days || range == RevenueRange::Last14Days)
        groupFormat = "%Y-%m-%d";
    else
        groupFormat = "%Y-%m";

    QMap<QString, double> revenues;

    double totalRevenue = 0;
    QSqlQuery query;
    query.prepare(QString(R"(
        SELECT strftime('%1', date) AS time_group, SUM(total_bill)
        FROM RetailInvoices
        WHERE date BETWEEN ? AND ?
        GROUP BY time_group
        ORDER BY time_group
    )").arg(groupFormat));

    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Lỗi truy vấn doanh thu:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QString time = query.value(0).toString();
        double sum = query.value(1).toDouble();
        revenues[time] = sum;
        totalRevenue += sum;
    }

    // Chuẩn bị biểu đồ
    QBarSet *set = new QBarSet("Doanh thu");
    QStringList labels;

    if (groupFormat == "%Y-%m-%d") {
        for (QDate d = startDate; d <= endDate; d = d.addDays(1)) {
            QString key = d.toString("yyyy-MM-dd");
            labels << d.toString("dd/MM");
            set->append(revenues.value(key, 0));
        }
    } else {
        QDate d = startDate;
        while (d <= endDate) {
            QString key = d.toString("yyyy-MM");
            labels << d.toString("MM/yyyy");
            set->append(revenues.value(key, 0));
            d = d.addMonths(1);
        }
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(labels);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->view_sum_report->setChart(chart);
    ui->view_sum_report->setRenderHint(QPainter::Antialiasing);
    ui->custom_sum->setText("Tổng doanh thu: " + QLocale().toString(totalRevenue, 'f', 0) + " đ");
}

void reports::RevenueChartByCustomRange() {
    QDate startDate = ui->start_date_sum->date(); // so sánh đc nếu thuộc kiểu QDate
    QDate endDate = ui->end_date_sum->date();

    // Kiểm tra nếu ngày bắt đầu > ngày kết thúc
    if (startDate > endDate) {
        QMessageBox::warning(this, "Lỗi", "Ngày bắt đầu phải trước hoặc bằng ngày kết thúc.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT date, SUM(total_bill) FROM RetailInvoices "
                  "WHERE date BETWEEN :start AND :end "
                  "GROUP BY date "
                  "ORDER BY date");
    query.bindValue(":start", startDate);
    query.bindValue(":end", endDate);

    if (!query.exec()) {
        QMessageBox::warning(this, "Lỗi", "Không truy vấn được dữ liệu:\n" + query.lastError().text());
        return;
    }

    QBarSet *barSet = new QBarSet("Doanh thu");
    QStringList categories; // ngày dưới dạng chuỗi
    double totalRevenue = 0;

    while (query.next()) {
        QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        double revenue = query.value(1).toDouble();

        *barSet << revenue;
        categories << date.toString("dd/MM");
        totalRevenue += revenue;
    }

    if (barSet->count() == 0) {
        QMessageBox::information(this, "Thông báo", "Không có dữ liệu trong khoảng thời gian đã chọn.");
        return;
    }

    QBarSeries *series = new QBarSeries;
    series->append(barSet);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle("Doanh thu từ " + startDate.toString("dd/MM/yyyy") + " đến " + endDate.toString("dd/MM/yyyy"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // Trục X - Ngày
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    axisX->setTitleText("Ngày");

    // Trục Y - Doanh thu
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Doanh thu (VNĐ)");
    axisY->applyNiceNumbers();

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    ui->view_sum_report->setChart(chart);
    ui->custom_sum->setText("Tổng doanh thu: " + QLocale().toString(totalRevenue, 'f', 0) + " đ");
}

void reports::loadSellerCombox(){
    QSqlQuery query;
    query.prepare("SELECT username FROM AccountUsers");

    if (!query.exec()) {
        QMessageBox::warning(this, "Lỗi", "Không thể tải danh sách người bán.");
        return;
    }

    ui->seller_1->clear();
    ui->seller_2->clear();
    ui->select_employee->clear();

    ui->seller_1->addItem("Tất cả");
    ui->seller_2->addItem("Tất cả");
    ui->select_employee->addItem("Tất cả");

    while (query.next()) {
        QString username = query.value(0).toString();
            ui->seller_1->addItem(username);
            ui->seller_2->addItem(username);
            ui->select_employee->addItem(username);
    }
}

void reports::showAllProducts(){
    QSqlQuery query;
    query.prepare(R"(
        SELECT  P.id,
                P.title,
                A.name,
                P.stock,
                COALESCE(SUM(R.quantity), 0) AS total_sold
        FROM Products P
        JOIN Authors A ON P.author_id = A.id
        LEFT JOIN RetailInvoicesItems R ON R.product_id = P.id
        GROUP BY P.id, P.title, A.name, P.stock
        ORDER BY P.stock DESC)");
    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError();
        return;
    }

    ui->view_stocks->setRowCount(0); // Xóa dữ liệu cũ nếu có
    int row = 0;
    /* sử dụng vòng lặp while mới lấy được hết các dòng */
    while (query.next()) {
        ui->view_stocks->insertRow(row); // Thêm dòng mới

        for (int col = 0; col < 5; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
            ui->view_stocks->setItem(row, col, item);
        }

        row++;
    }
}

void reports::sortByStock(){
    isIncreasing = !isIncreasing;

    if(isIncreasing) {
        QSqlQuery query;
        query.prepare(R"(
        SELECT  P.id,
                P.title,
                A.name,
                P.stock,
                COALESCE(SUM(R.quantity), 0) AS total_sold
        FROM Products P
        JOIN Authors A ON P.author_id = A.id
        LEFT JOIN RetailInvoicesItems R ON R.product_id = P.id
        GROUP BY P.id, P.title, A.name, P.stock
        ORDER BY P.stock ASC)");

        if (!query.exec()) {
            qDebug() << "Query failed:" << query.lastError();
            return;
        }

        ui->view_stocks->setRowCount(0); // Xóa dữ liệu cũ nếu có
        int row = 0;
        /* sử dụng vòng lặp while mới lấy được hết các dòng */
        while (query.next()) {
            ui->view_stocks->insertRow(row); // Thêm dòng mới

            for (int col = 0; col < 5; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                ui->view_stocks->setItem(row, col, item);
            }

            row++;
        }
    }
    else {
        showAllProducts();
    }
}

void reports::stockUI(){
    ui->view_stocks->setColumnCount(5);
    ui->view_stocks->setHorizontalHeaderLabels({"ID", "Tên sách","Tên tác giả", "Tồn kho","Đã bán"});
    ui->view_stocks->horizontalHeader()->setStretchLastSection(true);   // truy cập phần tiêu đề, cột cuối cùng tự động giãn để lấp đầy khoảng trống
    QHeaderView *headerInfor = ui->view_stocks->horizontalHeader();
    for(int col=0; col< 5 ;col++){
        headerInfor->setSectionResizeMode(col, QHeaderView::ResizeToContents);  // kích thước ô tự động giãn theo nội dung
    }
    ui->view_stocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->view_stocks->horizontalHeader()->setStretchLastSection(false);
}

void reports::convertToProductReportType(){
    connect(ui->select_stock, &QComboBox::currentTextChanged, this, [=](const QString &text) {
        ProductReportType type;

        if (text == "Top 5 sản phẩm đang bán chạy")
            type = ProductReportType::Top5BestSellers;
        else if (text == "Sản phẩm sắp hết hàng")
            type = ProductReportType::UpcomingSoldOut;
        else if (text == "Sản phẩm tồn kho nhiều")
             type = ProductReportType::Overstocked;
        else
            return; // không làm gì nếu không khớp

        loadDataForStocks(type);
    });
}

void reports::loadtop5bestseller() {
    /*  Truy vấn:
        - Cộng dồn số lượng bán (R.quantity) theo từng cuốn sách
        - Sắp xếp giảm dần theo tổng số đã bán
        - Giới hạn 5 kết quả đầu tiên
    */
    QSqlQuery query;
    query.prepare(R"(
        SELECT  P.id,
                P.title,
                A.name,
                P.stock,
                COALESCE(SUM(R.quantity), 0) AS total_sold
        FROM Products P
        JOIN Authors A ON P.author_id = A.id
        LEFT JOIN RetailInvoicesItems R ON R.product_id = P.id
        GROUP BY P.id, P.title, A.name, P.stock
        ORDER BY total_sold DESC
        LIMIT 5
    )");

    if (!query.exec()) {
        qDebug() << "Best-seller query failed:" << query.lastError();
        return;
    }

    ui->view_stocks->setRowCount(0);          // xoá dữ liệu cũ
    int row = 0;
    while (query.next()) {
        ui->view_stocks->insertRow(row);

        for (int col = 0; col < 5; ++col) {
            auto *item = new QTableWidgetItem(query.value(col).toString());
            ui->view_stocks->setItem(row, col, item);
        }
        ++row;
    }
}
void reports::loadUpcomingSoldout() {
     /* stock < 5 */
    QSqlQuery query;

     query.prepare(R"(
        SELECT  P.id,
                P.title,
                A.name,
                P.stock,
                COALESCE(SUM(R.quantity), 0) AS total_sold
        FROM Products P
        JOIN Authors A ON P.author_id = A.id
        LEFT JOIN RetailInvoicesItems R ON R.product_id = P.id
        WHERE P.stock < 5
        GROUP BY P.id, P.title, A.name, P.stock
        ORDER BY P.stock DESC
    )");
     if (!query.exec()) {
         qDebug() << "Best-seller query failed:" << query.lastError();
         return;
     }

    ui->view_stocks->setRowCount(0);          // xoá dữ liệu cũ
    int row = 0;
    while (query.next()) {
         ui->view_stocks->insertRow(row);

         for (int col = 0; col < 5; ++col) {
             auto *item = new QTableWidgetItem(query.value(col).toString());
             ui->view_stocks->setItem(row, col, item);
         }
         ++row;
     }
}
void reports::loadOverStocked() {
     /* stock > 20 */
    QSqlQuery query;
     query.prepare(R"(
        SELECT  P.id,
                P.title,
                A.name,
                P.stock,
                COALESCE(SUM(R.quantity), 0) AS total_sold
        FROM Products P
        JOIN Authors A ON P.author_id = A.id
        LEFT JOIN RetailInvoicesItems R ON R.product_id = P.id
        WHERE P.stock > 20
        GROUP BY P.id, P.title, A.name, P.stock
        ORDER BY P.stock DESC
    )");
    if (!query.exec()) {
        qDebug() << "Best-seller query failed:" << query.lastError();
        return;
    }

        ui->view_stocks->setRowCount(0);          // xoá dữ liệu cũ
        int row = 0;
        while (query.next()) {
            ui->view_stocks->insertRow(row);

            for (int col = 0; col < 5; ++col) {
                auto *item = new QTableWidgetItem(query.value(col).toString());
                ui->view_stocks->setItem(row, col, item);
            }
            ++row;
        }
}

void reports::loadDataForEmployees() {
    // Xác định loại báo cáo
    ReportRange type;
    QString text = ui->range_time->currentText();

    if (text == "Theo ngày")
        type = ReportRange::Daily;
    else if (text == "Theo tháng")
        type = ReportRange::Monthly;
    else if (text == "Theo quý")
        type = ReportRange::Quarterly;
    else if (text == "Theo năm")
        type = ReportRange::Yearly;
    else
        return;

    QString seller = ui->select_employee->currentText();

    // Tính khoảng thời gian
    QDate startDate, endDate;
    QDate current = QDate::currentDate();

    switch (type) {
    case ReportRange::Daily:
        startDate = current;
        endDate = current;
        break;
    case ReportRange::Monthly:
        startDate = QDate(current.year(), current.month(), 1);
        endDate = startDate.addMonths(1).addDays(-1);
        break;
    case ReportRange::Quarterly: {
        int quarter = (current.month() - 1) / 3 + 1;
        startDate = QDate(current.year(), (quarter - 1) * 3 + 1, 1);
        endDate = startDate.addMonths(3).addDays(-1);
        break;
    }
    case ReportRange::Yearly:
        startDate = QDate(current.year(), 1, 1);
        endDate = QDate(current.year(), 12, 31);
        break;
    }

    // Viết truy vấn SQL linh hoạt
    QString sql = R"(
    SELECT A.username, A.fullname, COALESCE(SUM(I.total_bill), 0) AS total
    FROM AccountUsers A
    LEFT JOIN RetailInvoices I ON I.user_id = A.id
    WHERE A.role IN ('admin', 'user')
      AND I.date BETWEEN ? AND ?
    )";

    if (seller != "Tất cả") {
        sql += " AND A.username = ?";
    }

    sql += " GROUP BY A.username, A.fullname ORDER BY total DESC";

    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(startDate.toString("yyyy-MM-dd"));
    query.addBindValue(endDate.toString("yyyy-MM-dd"));
    if (seller != "Tất cả") {
        query.addBindValue(seller);
    }


    if (!query.exec()) {
        QMessageBox::warning(this, "Lỗi", "Không thể truy vấn dữ liệu:\n" + query.lastError().text());
        return;
    }

    // Cập nhật bảng
    ui->view_employee->clearContents();
    ui->view_employee->setRowCount(0);
    ui->view_employee->setColumnCount(3);
    ui->view_employee->setHorizontalHeaderLabels({"Tên người dùng", "Họ và tên", "Doanh thu"});

    int row = 0;
    while (query.next()) {
        QString username = query.value(0).toString();
        QString fullName = query.value(1).toString();
        double revenue = query.value(2).toDouble();

        ui->view_employee->insertRow(row);
        ui->view_employee->setItem(row, 0, new QTableWidgetItem(username));
        ui->view_employee->setItem(row, 1, new QTableWidgetItem(fullName));
        ui->view_employee->setItem(row, 2, new QTableWidgetItem(QLocale().toString(revenue, 'f', 0)));

        ++row;
    }

    ui->view_employee->resizeColumnsToContents();
}
