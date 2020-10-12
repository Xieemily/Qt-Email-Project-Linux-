#include "receivebox.h"
#include "ui_receivebox.h"
#include "client.h"
#include "read.h"
#include "ui_read.h"
#include <QStandardItemModel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStackedWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QFile>

ReceiveBox::ReceiveBox(QWidget *parent, QString _identifer) :
    QWidget(parent),identifer(_identifer),
    ui(new Ui::ReceiveBox)
{
    ui->setupUi(this);

    //stylesheet
    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());



    ui->treeWidget_receive->setColumnWidth(0, 400);//content
    ui->treeWidget_receive->setColumnWidth(1, 500);//subject
    ui->treeWidget_receive->setColumnWidth(2, 300);//time
    ui->treeWidget_receive->setColumnWidth(3, 0);//id
    ui->treeWidget_receive->hideColumn(3);

    //QStringList * strlst = new QStringList()
    //add info from server to treewidget

    QString receive_str1 = "aa|bb|cc|dd";
    QString receive_str2 = "xxx|yyy|zzz";
    QString receive_str3 = "t1|t2|t3";
    QString receive_id = "id1|id2|id3";



    //receiveMail();
}

ReceiveBox::~ReceiveBox()
{
    delete ui;
}

void ReceiveBox::add(QString str)
{
    QStringList received_mail = str.split('|',QString::SkipEmptyParts);
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

void ReceiveBox::disable_read(){
    ui->tbtn_read->hide();
}
void ReceiveBox::disable_trash(){
    ui->tbtn_trash->hide();
}
void ReceiveBox::disable_delete(){
    ui->tbtn_delete->hide();
}

void ReceiveBox::disable_recover(){
    ui->tbtn_recover->hide();
}


void ReceiveBox::receiveMail()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->treeWidget_receive->clear();
    QString str = QString(":/new/prefix1/picres/message_open.png");
    client * user = new client;


    QString request = identifer + userid_global;
    QString mails = user->sendMessage(request);//state|from|subject|time|emailid
    qDebug() << "request" << request;
    qDebug() << "mails" << mails;

    //
    //QString mails = "1|from|subject|1:1:1|id;2|from2|sub2|2:2:2|id2";
    //
    if(mails == "no message")mails = "";
    QStringList items = mails.split(";");
    qDebug() << items;
    for(int i = 0; i < items.size(); i++)
    {
        if(items.at(i).isEmpty())continue;
        add(items.at(i));
    }

}

void ReceiveBox::on_checkBox_stateChanged(int arg1)
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

void ReceiveBox::on_tbtn_trash_clicked()
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

QStringList ReceiveBox::GetChecked(){
    QTreeWidgetItemIterator it(ui->treeWidget_receive);
    QStringList checkedItem;
    while(*it)
    {
        qDebug() << "in while" << (*it)->text(0);
        if(2 == (*it)->checkState(0))
        {
            qDebug() << (*it)->text(3);
            checkedItem.push_back((*it)->text(3));
        }
        it++;
    }
    qDebug() << "getchecked item:" << checkedItem;
    return checkedItem;
}

void ReceiveBox::goback(){
    receiveMail();
}

void ReceiveBox::on_treeWidget_receive_itemDoubleClicked(QTreeWidgetItem *item, int column)
{

    QString clickedEmailID = "r|" + item->text(3);
    client* user = new client;
    QString emailInfo = user->sendMessage(clickedEmailID);//go to read page, title|content|sender|filenum|filename


    //
    //QString emailInfo = "sub|content|from|1|a.txt";
    //
    Read * read_page = new Read;


    QStringList email_info = emailInfo.split("|", QString::SkipEmptyParts);

    read_page->setFrom(email_info.at(2));
    read_page->setSubject(email_info.at(0));
    read_page->setContent(email_info.at(1));
    if(email_info.size() == 5)//has appendix
        read_page->setAppendix(email_info.at(4));

    ui->stackedWidget->insertWidget(1,read_page);
    ui->stackedWidget->setCurrentIndex(1);

    connect(read_page, &Read::req_file, [=](QString file){
        QString req_file = "d|" + item->text(3) + "|" + file;

        int file_size = user->sendMessage2(req_file);

        qDebug() << "file_size" << file_size;

        user->recvFile("/home/xmy/"+file,file_size);

    });
    connect(read_page, &Read::closeread, this, &ReceiveBox::goback);

}



void ReceiveBox::on_tbtn_delete_clicked()
{
    client * user = new client;
    QStringList checked = GetChecked();
    qDebug() << "checked" << checked;
    for(int i = 0; i < checked.size(); i++)
    {
        QString request = QString::number(8) + "|" + checked.at(i);
        user->sendMessage(request);
    }
    receiveMail();
}



void ReceiveBox::on_tbtn_read_clicked()
{
    QStringList checked = GetChecked();
    for(int i = 0; i < checked.size(); i++)
    {
        QString request = QString::number(9) + "|" + checked.at(i);
        client * user = new client;
        user->sendMessage(request);
    }
    receiveMail();
}

void ReceiveBox::on_tbtn_recover_clicked()
{
    QStringList checked = GetChecked();
    QString request = "f|" + checked.at(0);
    client * user = new client;
    user->sendMessage(request);
    receiveMail();
}
