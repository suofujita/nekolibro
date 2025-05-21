#include "nekolibro.h"
#include "flashscreen.h"
#include "login.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FlashScreen screen;
    login log;
    screen.show();

    database db;
    if (db.connectDatabase()) {
        db.createUserProfiles();
        db.createAdminAccount();  // Tạo tài khoản admin mặc định nếu chưa tồn tại
    }

    NekoLibro w;
    // w.show();
    QObject::connect(&screen,&FlashScreen::ScreenFinished,[&](){
        log.show();
    });
    return a.exec();
}


