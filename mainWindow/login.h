#ifndef Login_H
#define Login_H

#include <QMainWindow>

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_pushButton_signup_clicked();

    void on_pushButton_login_clicked();

private:
    Ui::Login *ui;
};

#endif // Login_H
