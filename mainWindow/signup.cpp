#include "signup.h"
#include "ui_signup.h"
#include "login.h"
#include "ui_mainwindow2.h"
#include "client.h"
#include<QDialog>
#include<QDebug>
#include<QMessageBox>
#include<QPixmap>
#include<QFile>


Signup::Signup(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Signup)
{
    ui->setupUi(this);

    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());

    loadMarks();

    ui->lineEdit_username->setFocus();

}


Signup::~Signup()
{
    delete ui;
}

void Signup::loadMarks(){
    //load tick mark image
    QImage image;
    image.load(":/new/prefix1/picres/checkmark.png");
    QPixmap pixmap_tick = QPixmap::fromImage(image);
    fit_pixmap_tick = pixmap_tick.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    image.load(":/new/prefix1/picres/cross.png");
    QPixmap pixmap_cross = QPixmap::fromImage(image);
    fit_pixmap_cross = pixmap_cross.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


bool Signup::checkText(QLineEdit * line_edit, QLabel * warning_label, int max_len, int min_len){
    QString username = line_edit->text();
    if(username.length() > max_len){
        warning_label->setText("too long");
        warning_label->setStyleSheet("color:red;");
    }
    else if(username.length() < min_len){
        warning_label->setText("too short");
        warning_label->setStyleSheet("color:red;");
    }
    else
    {
        warning_label->setPixmap(fit_pixmap_tick);
        return true;
    }
    return false;
}


void Signup::on_pushButton_signup_clicked()
{

    QString username = ui->lineEdit_username->text();
    QString account = ui->lineEdit_account->text();
    QString password = ui->lineEdit_password->text();
    QString confirm = ui->lineEdit_confirm->text();

    if(checkText(ui->lineEdit_username, ui->label_username_check, 15, 6)
            &&checkText(ui->lineEdit_account, ui->label_account_check, 15, 6)
            &&checkText(ui->lineEdit_password, ui->label_password_check, 20, 8)
            &&ui->lineEdit_password->text().compare(ui->lineEdit_confirm->text()) == 0
            )
    {
        //user info length checked, connect to database and check repeat account
        QString signup_req = QString::number(0) + '|' + username + '|' + account + '|' + password;
        qDebug() << signup_req;

        client * clt = new client();

        if(clt->connectToServer())
        {
            //connetion established;
            QString isValidUser = clt->sendMessage(signup_req);
            if(isValidUser[0] == '1')
            {
                //QMessageBox::
                toLogin();
            }
            else
            {//repeat id
                QMessageBox::warning(this, "ERROR","Account already exists!");
                return;
            }
        }
        else
        {
            //connection failed
            QMessageBox::warning(this, "ERROR","failed connecting to server");
            return;
        }

    }
    else
    {
        //user input invalid
        QMessageBox::warning(this, "invalid info", "please recheck");
        return;
    }



}


void Signup::on_pushButton_back_clicked()
{
    this->close();
    Login * loginPage = new Login;
    loginPage->show();
}

void Signup::toLogin(){
    this->close();
    Login * loginPage = new Login;
    loginPage->show();
}



void Signup::on_lineEdit_username_editingFinished()
{
    checkText(ui->lineEdit_username, ui->label_username_check, 15, 6);
}


void Signup::on_lineEdit_account_editingFinished()
{
    checkText(ui->lineEdit_account, ui->label_account_check, 15, 6);
}

void Signup::on_lineEdit_password_editingFinished()
{
    checkText(ui->lineEdit_password, ui->label_password_check, 20, 8);
}



void Signup::on_lineEdit_confirm_editingFinished()
{
    checkText(ui->lineEdit_confirm, ui->label_confirm_check, 20, 8);
    if(ui->lineEdit_password->text().compare(ui->lineEdit_confirm->text()) != 0)
        ui->label_confirm_check->setPixmap(fit_pixmap_cross);
}

