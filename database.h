#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class database
{
public:
    database();
    bool connectDatabase();
    void createUsersTable();
    void closeDatabase();
    void createAdminAccount();
};

#endif // DATABASE_H
