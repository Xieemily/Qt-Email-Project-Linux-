#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>
#include <QString>
#include "mydraft.h"
#include "receivebox.h"
#include "write.h"
#include "ui_write.h"
#include "client.h"

namespace Ui {
class MainPage;
}

class MainPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();
    void uncheckAll(QPushButton *);
    Write * write_page;
    ReceiveBox * receive_page;
    MyDraft * draft_page;
    ReceiveBox * sent_page;
    ReceiveBox * deleted_page;

private slots:
    void on_write_btn_clicked();

    void on_receivebox_btn_clicked();

    void on_draft_btn_clicked();

    void on_sent_btn_clicked();

    void on_deleted_btn_clicked();



private:
    Ui::MainPage *ui;
    //QPushButton write,deleted,contact,receivebox,sent,draft;
    //QStackedWidget mystackedwidget;
    //QWidget receivebox_2,deleted_2,contact_2,sent_2,draft_2;
};



#endif // MAINPAGE_H
