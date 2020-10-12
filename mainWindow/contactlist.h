#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QWidget>

namespace Ui {
class ContactList;
}

class ContactList : public QWidget
{
    Q_OBJECT

public:
    explicit ContactList(QWidget *parent = nullptr);
    ~ContactList();

signals:
    void send_clicked_account(QString str_account);

private:
    Ui::ContactList *ui;
};

#endif // CONTACTLIST_H
