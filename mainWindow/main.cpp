#include "mainpage.h"
#include "login.h"
#include "signup.h"
#include "receivebox.h"
#include "contactlist.h"
#include "write.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;
    w.show();
    return a.exec();
}
