#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "nekolibro.h"
class database
{
public:
    database();
    bool connectDatabase();
    void closeDatabase();
    void createUserProfiles();
    void createAdminAccount();

};

#endif // DATABASE_H
