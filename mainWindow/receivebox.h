#ifndef RECEIVEBOX_H
#define RECEIVEBOX_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>


namespace Ui {
class ReceiveBox;
}

class ReceiveBox : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiveBox(QWidget *parent = nullptr, QString identifer = "");
    ~ReceiveBox();
    QStringList GetChecked();
    void receiveMail();
    void add(QString);
    QString identifer;
    void disable_trash();
    void disable_delete();
    void disable_read();
    void disable_recover();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_tbtn_trash_clicked();

    void on_treeWidget_receive_itemDoubleClicked(QTreeWidgetItem *item, int column);


    void on_tbtn_delete_clicked();



    void goback();

    void on_tbtn_read_clicked();

    void on_tbtn_recover_clicked();

private:
    Ui::ReceiveBox *ui;

};

#endif // RECEIVEBOX_H
