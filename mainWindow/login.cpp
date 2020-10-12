#include "login.h"
#include "signup.h"
#include "ui_login.h"
#include "signup.h"
#include "user.h"
#include "mainpage.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());

}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_signup_clicked()
{
    this->hide();
    Signup * md2 = new Signup(this);
    md2->show();
}

void Login::on_pushButton_login_clicked()
{
    QString userid = ui->lineEdit_login_username->text();
    QString password = ui->lineEdit_login_password->text();

    //qDebug() << userid << password;

    if(userid.isEmpty() || password.isEmpty()){
        QMessageBox::warning(this, "warning", "username or password is empty");
        return;
    }

    QString loginreq = QString::number(1) + "|" +userid + "|" + password;

    client * user = new client;
    user->connectToServer();
    QString isValid = user->sendMessage(loginreq);
    //user->receiveFromServer();
    qDebug() << "receive:" << isValid;

    if(isValid[0] == '1')//success
    {
        //go to mainpage
        userid_global = userid;
        this->close();
        MainPage *mainpage = new MainPage();
        QString userid_global = userid;
        mainpage->show();
    }
    else
    {
        QMessageBox::warning(this, "login failed", "invalid username or password");
        ui->lineEdit_login_password->clear();
    }



}
