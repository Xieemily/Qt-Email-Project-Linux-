#include "read.h"
#include "ui_read.h"
#include <QToolButton>

Read::Read(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Read)
{
    ui->setupUi(this);
}

Read::~Read()
{
    delete ui;
}

void Read::setFrom(QString from)
{
    ui->lineEdit_from->setText(from);
}

void Read::setSubject(QString subject)
{
    ui->lineEdit_subject->setText(subject);
}

void Read::setContent(QString content)
{
    ui->textEdit->append(content);
}

void Read::setAppendix(QString appendix)
{
    QStringList fileNames = appendix.split(";", QString::SkipEmptyParts);
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

        connect(file_btn, &QToolButton::clicked, [=](){
            emit req_file(file_btn->text());
        });

        //add button
        ui->horizontalLayout_file->addWidget(file_btn, 0, Qt::AlignLeft);
        ui->widget_files->show();
    }
}

void Read::on_toolButton_clicked()
{
    this->close();
    emit closeread();
}
