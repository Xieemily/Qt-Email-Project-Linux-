#include "write.h"
#include "ui_write.h"
#include "contactlist.h"
#include "ui_contactlist.h"
#include "client.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QWidget>
#include <QDateTime>

void Write::receive_account(QString str_account){
    //show chosen account
    ui->lineEdit_to->insert(str_account+",");
}


//click bar to add target contacts
Write::Write(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Write)
{
    ui->setupUi(this);

    //stylesheet
    QFile stylesheet(":/new/prefix1/picres/xxx.qss");
    stylesheet.open(QIODevice::ReadOnly);
    this->setStyleSheet(stylesheet.readAll());

    numFile = 0;
    Write::paths.clear();
    Write::send_to.clear();
    Write::subject.clear();

    ui->widget_files->hide();



    //choose account from contact list
    connect(ui->widget_contacts, &ContactList::send_clicked_account, this, &Write::receive_account);

    //font
    connect(ui->fontComboBox, &QFontComboBox::currentFontChanged, this, [=](const QFont &font){
        ui->textEdit->setCurrentFont(font);
        ui->textEdit->setFocus();
    });


    //size
    void (QComboBox:: * sig_size)(const QString &text) = &QComboBox::currentIndexChanged;
    connect(ui->comboBox, sig_size, [=](const QString text){
        ui->textEdit->setFontPointSize(text.toDouble());
        ui->textEdit->setFocus();
    });

    //bold
    connect(ui->toolButton_Bold, &QToolButton::clicked, this, [=](bool checked){
        if(checked)
        {
            ui->textEdit->setFontWeight(QFont::Bold);
        }
        else
        {
            ui->textEdit->setFontWeight(QFont::Normal);
        }
    });

    //italic
    connect(ui->toolButton_italic, &QToolButton::clicked, this, [=](bool checked){
        if(checked)
        {
            ui->textEdit->setFontItalic(true);
        }
        else
        {
            ui->textEdit->setFontItalic(false);
        }
    });

    //underline
    connect(ui->toolButton_underline,&QToolButton::clicked, this, [=](bool checked){
        if(checked)
        {
            ui->textEdit->setFontUnderline(true);
        }
        else
        {
            ui->textEdit->setFontUnderline(false);
        }
    });

    //clear
    connect(ui->toolButton_clear, &QToolButton::clicked, this, [=](){
        ui->textEdit->clear();
    });

    //color
    QColorDialog * colordlg = new QColorDialog(this);
    connect(colordlg, &QColorDialog::currentColorChanged, [=](QColor color){
        ui->textEdit->setTextColor(color);
    });

    connect(colordlg, &QColorDialog::colorSelected, [=](QColor color){
        ui->textEdit->setTextColor(color);
    });

    connect(ui->toolButton_color, &QToolButton::clicked, [=](){
        colordlg->show();
    });

}



Write::~Write()
{
    delete ui;
}


void Write::on_lineEdit_to_textChanged(const QString &arg1)
{
    send_to = arg1.split(',', QString::SkipEmptyParts);

    for(int i = 0; i < send_to.size(); i++)
    {
        qDebug() << i << "to:" << send_to.at(i);
    }
}

void Write::on_lineEdit_subject_textChanged(const QString &arg1)
{
    subject = arg1;
    qDebug() << subject;
}


void Write::on_toolButton_appendix_clicked()
{
    if(numFile > 5)
    {
        QMessageBox::warning(this, "warning", "the number of files exceeds the limit!");
        return;
    }

    //choose file
    QString path = QFileDialog::getOpenFileName(this, "choose file",":");
    if(path.isEmpty())return;
    numFile++;

    Write::paths.append(path);

    //file info
    QFileInfo fileInfo = QFileInfo(path);
    fileNames.append(fileInfo.fileName());

    addFileBtn(fileInfo);

//    for(int i = 0; i < paths.size(); i++)
//    {
//        qDebug() << paths.at(i);
//    }



}

void Write::addFileBtn(QFileInfo fileInfo){
    QToolButton * file_btn = new QToolButton;

    //text
    file_btn->setText(fileInfo.fileName());

   //icon
    QString str = QString(":/new/prefix1/picres/file_new.png");
    file_btn->setIcon(QPixmap(str));
    file_btn->setIconSize(QSize(40,40));

    //set button
    file_btn->setAutoRaise(true);
    file_btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    file_btn->setMaximumSize(QSize(120, 80));
    file_btn->setMinimumSize(QSize(120, 80));

    //add button
    ui->horizontalLayout_file->addWidget(file_btn, 0, Qt::AlignLeft);
    ui->widget_files->show();

    //click to cancel file
    connect(file_btn, &QToolButton::clicked, [=](){
        file_btn->deleteLater();
        ui->horizontalLayout_file->removeWidget(file_btn);
        paths.removeAll(fileInfo.filePath());
        fileToolBtn.removeOne(file_btn);
        numFile--;

    });

    fileToolBtn.push_back(file_btn);
}






void Write::on_pushButton_send_clicked()
{
    client * user = new client();
    user->connectToServer();
    content = ui->textEdit->toPlainText();
    qDebug() << content;
    //email request format:2|from|to,to...|subject|time|bool(appendix)|number of appendix
    QString email_request = QString::number(2) + "|" + userid_global + "|";
    qDebug() << email_request;

    for(int i = 0; i < send_to.size(); i++)
    {
        QString email_request = QString::number(2) + "|" + userid_global + "|";
        qDebug() << email_request;

        qDebug() << send_to.size();
        email_request += send_to.at(i);

        qDebug() << "numfile" << numFile;
        email_request += "|" + subject + "|" + QTime().currentTime().toString("hh:mm:ss");
        email_request += "|" + QString::number(numFile);

        email_request +=  "|" + content;

        qDebug() << "email rea:" << email_request;

        //send request
        QString mail_id = user->sendMessage(email_request);
        qDebug() << "mail_id" << mail_id;

        //user->sendMessage(content);



        //send appendix
        qDebug() << "path_size" <<paths.size();
        qDebug() << "file_num" << numFile;

        for(int i = 0; i < paths.size(); i++)
        {
            user->sendMessage(fileNames.at(i));
            qDebug() << paths.at(i);

            user->sendFile(paths.at(i));
        }
    }
}



void Write::on_toolButton_draft_clicked()
{
    client * user = new client();
    user->connectToServer();
    content = ui->textEdit->toPlainText();
    qDebug() << content;
    //email request format:2|from|to,to...|subject|time|bool(appendix)|number of appendix
    QString email_request = QString::number(3) + "|" + userid_global + "|";
    qDebug() << email_request;

    for(int i = 0; i < send_to.size()-1; i++)
    {
        qDebug() << send_to.size();
        email_request += send_to.at(i);
        email_request += ',';
    }
    email_request += send_to.at(send_to.size()-1);


    qDebug() << "numfile" << numFile;
    email_request += "|" + subject + "|" + QTime().currentTime().toString("hh:mm:ss");
    email_request += "|" + QString::number(numFile);

    email_request +=  "|" + content;

    qDebug() << "email rea:" << email_request;

    //send request
    QString mail_id = user->sendMessage(email_request);
    qDebug() << "mail_id" << mail_id;

    //user->sendMessage(content);



    //send appendix
    qDebug() << "path_size" <<paths.size();
    qDebug() << "file_num" << numFile;

    for(int i = 0; i < paths.size(); i++)
    {
        user->sendMessage(fileNames.at(i));
        qDebug() << paths.at(i);
        user->sendFile(paths.at(i));
    }
}

void Write::setTo(QString str){
    ui->lineEdit_to->setText(str);
}

void Write::setSubject(QString str){
    ui->lineEdit_subject->setText(str);
}

void Write::setContent(QString str){
    ui->textEdit->append(str);
}

void Write::setAppendix(QString appendix){
    QStringList fileNames = appendix.split(",",QString::SkipEmptyParts);
    for(int i = 0; i < fileNames.size(); i++)
    {


        QToolButton * file_btn = new QToolButton;

        //text
        file_btn->setText(fileNames.at(i));

       //icon
        QString str = QString(":/new/prefix1/picres/file_new.png");
        file_btn->setIcon(QPixmap(str));
        file_btn->setIconSize(QSize(40,40));

        //set button
        file_btn->setAutoRaise(true);
        file_btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        file_btn->setMaximumSize(QSize(120, 80));
        file_btn->setMinimumSize(QSize(120, 80));

        //signal
        connect(file_btn, &QToolButton::clicked, [=](){
            emit reqfile(file_btn->text());
        });

        //add button
        ui->horizontalLayout_file->addWidget(file_btn, 0, Qt::AlignLeft);
        ui->widget_files->show();
    }
}
