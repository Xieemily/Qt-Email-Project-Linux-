#include "mainpage.h"
#include "ui_mainpage.h"
#include <QWidget>
#include "read.h"
#include "write.h"
#include "user.h"
#include "receivebox.h"
#include "mydraft.h"

MainPage::MainPage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);

    qDebug() << userid_global;



    //stylesheet
    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());

    ui->label->setText(userid_global);



    Write * write_page = new Write;
    receive_page = new ReceiveBox(this, "4|");
    draft_page = new MyDraft;
    sent_page = new ReceiveBox(this, "5|");
    deleted_page = new ReceiveBox(this, "6|");

    receive_page->disable_recover();
    sent_page->disable_read();
    sent_page->disable_recover();
    deleted_page->disable_trash();
    deleted_page->disable_read();


    ui->mystackedwidget->insertWidget(0, write_page);
    ui->mystackedwidget->insertWidget(1, receive_page);
    ui->mystackedwidget->insertWidget(2, draft_page);
    ui->mystackedwidget->insertWidget(3, sent_page);
    ui->mystackedwidget->insertWidget(4, deleted_page);

    receive_page->receiveMail();
    ui->mystackedwidget->setCurrentIndex(1);




}

MainPage::~MainPage()
{
    delete ui;
}



void MainPage::on_write_btn_clicked()
{
    ui->mystackedwidget->setCurrentIndex(0);
}

void MainPage::on_receivebox_btn_clicked()
{
    ui->mystackedwidget->setCurrentIndex(1);
    receive_page->receiveMail();

}

void MainPage::on_draft_btn_clicked()
{
    ui->mystackedwidget->setCurrentIndex(2);
    draft_page->receiveMail();
}

void MainPage::on_sent_btn_clicked()
{
    ui->mystackedwidget->setCurrentIndex(3);
    sent_page->receiveMail();

}

void MainPage::on_deleted_btn_clicked()
{
    ui->mystackedwidget->setCurrentIndex(4);
    deleted_page->receiveMail();
}


