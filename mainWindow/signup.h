#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class Signup; }
QT_END_NAMESPACE

class Signup : public QMainWindow
{
    Q_OBJECT

public:
    Signup(QWidget *parent = nullptr);
    ~Signup();

private slots:

    void on_lineEdit_username_editingFinished();


    void on_lineEdit_account_editingFinished();

    void on_lineEdit_password_editingFinished();

    void on_lineEdit_confirm_editingFinished();

    void on_pushButton_signup_clicked();


    void on_pushButton_back_clicked();

private:
    Ui::Signup *ui;

    QPixmap fit_pixmap_tick;
    QPixmap fit_pixmap_cross;

    void toLogin();
    bool checkText(QLineEdit * line_edit, QLabel * warning_label, int max_len, int min_len);
    void loadMarks();
};
#endif // MAINWINDOW_H
