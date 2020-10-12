#ifndef MYDRAFT_H
#define MYDRAFT_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace Ui {
class MyDraft;
}

class MyDraft : public QWidget
{
    Q_OBJECT

public:
    explicit MyDraft(QWidget *parent = nullptr);
    ~MyDraft();
    QStringList GetChecked();
    void receiveMail();
    void add(QString);
    QString identifer;
    void disable_trash();
    void disable_delete();
    void disable_read();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_tbtn_trash_clicked();

    void on_treeWidget_receive_itemDoubleClicked(QTreeWidgetItem *item, int column);


    void on_tbtn_delete_clicked();



    void goback();

    void on_tbtn_read_clicked();


private:
    Ui::MyDraft *ui;
};

#endif // MYDRAFT_H
