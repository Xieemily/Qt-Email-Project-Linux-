#include "contactlist.h"
#include "ui_contactlist.h"
#include <QToolButton>
#include <QFile>

ContactList::ContactList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactList)
{
    ui->setupUi(this);

    //stylesheet
    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());

    //account list
    QList<QString> contactAccounts;
    contactAccounts << "a@mail.com" << "b@mail.com" << "c@mail.com";

    //icon list
    QStringList iconList;
    iconList << "checkmark" << "checkmark" << "checkmark";

    QVector<QToolButton *> vToolBtn;


    for(int i = 0; i < contactAccounts.size(); i++)
    {
        QToolButton * btn = new QToolButton;

        //text
        btn->setText(contactAccounts.at(i));

       //icon
        //QString str = QString(":/new/prefix1/picres/%1.png").arg(iconList.at(i));
        QString str = QString(":/new/prefix1/picres/user.png");
        btn->setIcon(QPixmap(str));
        btn->setIconSize(QSize(80,80));

        //set button
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btn->setMaximumSize(QSize(350, 70));
        btn->setMinimumSize(QSize(350, 70));

        //add button
        ui->vLayout->addWidget(btn);

        //save btns to a vector
        vToolBtn.push_back(btn);
    }

    //add slot
    for(int i = 0; i < vToolBtn.size(); i++)
    {
        connect(vToolBtn.at(i), &QToolButton::clicked, [=](){
            emit send_clicked_account(vToolBtn.at(i)->text());
        });
    }

}

ContactList::~ContactList()
{
    delete ui;
}
