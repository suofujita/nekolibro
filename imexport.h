#ifndef IMEXPORT_H
#define IMEXPORT_H

#include <QWidget>
#include "nekolibro.h"

namespace Ui {
class ImExport;
}

class ImExport : public QWidget
{
    Q_OBJECT

public:
    explicit ImExport(QWidget *parent = nullptr);
    ~ImExport();

private slots:
    void toMainImExport();       // main
    void toExportInfor();        // xem xác nhận thông tin trước khi ok
    //void toExportDetail();      // xem lại chi tiết xuất hàng
    void toImportInfor();        // xem xác nhận thông tin trước khi ok
    //void toImportDetail();       // xem lại Chi tiết nhập hàng
    void toImportHistories();    // lịch sử nhập hàng
    void toExportHistories();   // lịch sử xuất hàng


private:
    Ui::ImExport *ui;
    QSqlDatabase db;
};

#endif // IMEXPORT_H
