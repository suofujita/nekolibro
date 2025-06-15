#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include "nekolibro.h"
namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private:
    Ui::Settings *ui;
    QSqlDatabase db;
    void loadUserInfor();
};

#endif // SETTINGS_H
