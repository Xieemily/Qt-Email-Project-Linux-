#include "mydraft.h"
#include "ui_mydraft.h"
#include "client.h"
#include "write.h"
#include "ui_write.h"
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QFile>


MyDraft::MyDraft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyDraft)
{
    ui->setupUi(this);

    //stylesheet
    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());


    ui->treeWidget_receive->setColumnWidth(0, 400);//content
    ui->treeWidget_receive->setColumnWidth(1, 700);//subject
    ui->treeWidget_receive->setColumnWidth(2, 100);//time
    ui->treeWidget_receive->setColumnWidth(2, 0);//id
}

MyDraft::~MyDraft()
{
    delete ui;
}

void MyDraft::receiveMail()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->treeWidget_receive->clear();
    QString str = QString(":/new/prefix1/picres/message_open.png");
    client * user = new client;


    QString request = QString::number(7) + "|" + userid_global;
    QString mails = user->sendMessage(request);//state|from|subject|time|emailid
    qDebug() << "request" << request << "mails" << mails;

    //
    //QString mails = "1|from|subject|1:1:1|id;2|from2|sub2|2:2:2|id2";
    //

    QStringList items = mails.split(";");
    qDebug() << items;
    for(int i = 0; i < items.size(); i++)
    {
        add(items.at(i));
    }

}

void MyDraft::on_checkBox_stateChanged(int arg1)
{
    QTreeWidgetItemIterator it(ui->treeWidget_receive);
    if(2 == arg1)
    {
        //checked, select all the items in wedgit
        while(*it)
        {
            (*it)->setCheckState(0, Qt::Checked);
            ++it;
        }
    }
    else
    {
        //uncheck
        while(*it)
        {
            (*it)->setCheckState(0, Qt::Unchecked);
            ++it;
        }
    }
}

void MyDraft::on_tbtn_trash_clicked()
{
    QTreeWidgetItemIterator it(ui->treeWidget_receive);
    client * user = new client;

    QStringList checked = GetChecked();
    for(int i = 0; i < checked.size(); i++)
    {
        qDebug() << "deleting" << checked.at(i);
        user->sendMessage("a|" + checked.at(i));
    }

    receiveMail();
}

QStringList MyDraft::GetChecked(){
    QTreeWidgetItemIterator it(ui->treeWidget_receive);
    QStringList checkedItem;
    while(*it)
    {
        if(2 == (*it)->checkState(0))
        {

            checkedItem.push_back((*it)->text(3));
        }
    }
    return checkedItem;
}

void MyDraft::goback(){
    receiveMail();
}

void MyDraft::on_treeWidget_receive_itemDoubleClicked(QTreeWidgetItem *item, int column)
{

    QString clickedEmailID = "r|" + item->text(3);
    client* user = new client;
    QString emailInfo = user->sendMessage(clickedEmailID);//go to read page, title|content|sender|filenum|filename


    qDebug() << "doubleclicked " <<clickedEmailID;
    //QString emailInfo = "sub|content|from|1|a.txt";

    Write * write_page = new Write;

    connect(write_page, &Write::reqfile, [=](QString str){
        QString req_file = "d|" + item->text(3) + "|" + str;

        int file_size = user->sendMessage2(req_file);

        qDebug() << "file_size" << file_size;
        user->recvFile("/home/xmy/test1.cpp",file_size);
    });

    QStringList email_info = emailInfo.split("|", QString::SkipEmptyParts);

    //write_page->setTo("");



    write_page->setSubject(email_info.at(0));
    write_page->setContent(email_info.at(1));
    if(email_info.size() == 5)//has appendix
        write_page->setAppendix(email_info.at(4));

    ui->stackedWidget->insertWidget(1,write_page);
    ui->stackedWidget->setCurrentIndex(1);

    connect(write_page, &Write::sent, this, &MyDraft::goback);

}



void MyDraft::on_tbtn_delete_clicked()
{
    QStringList checked = GetChecked();
    QString request = QString::number(8) + "|" + checked.at(0);
    client * user = new client;
    user->sendMessage(request);
    receiveMail();
}



void MyDraft::on_tbtn_read_clicked()
{
    QStringList checked = GetChecked();
    QString request = QString::number(9) + "|" + checked.at(0);
    client * user = new client;
    user->sendMessage(request);
    receiveMail();
}

void MyDraft::add(QString str)
{
    QStringList received_mail = str.split('|');
    QString state = received_mail.at(0);
    received_mail.removeFirst();
    QTreeWidgetItem * item = new QTreeWidgetItem(received_mail);
    item->setCheckState(0,Qt::Unchecked);
    if(state == "2")//already read
    {
        QString str = QString(":/new/prefix1/picres/message_open.png");
        item->setIcon(0, QPixmap(str));
    }
    ui->treeWidget_receive->addTopLevelItem(item);
}

