#ifndef WRITE_H
#define WRITE_H

#include <QWidget>
#include <QToolButton>
#include <QFileDialog>

#include "client.h"

namespace Ui {
class Write;
}

class Write : public QWidget
{
    Q_OBJECT

public:
    explicit Write(QWidget *parent = nullptr);
    ~Write();
    void setTo(QString);
    void setSubject(QString);
    void setContent(QString);
    void setAppendix(QString);

private slots:
    void receive_account(QString str_account);

    void on_toolButton_appendix_clicked();

    void on_pushButton_send_clicked();

    void on_lineEdit_to_textChanged(const QString &arg1);

    void on_lineEdit_subject_textChanged(const QString &arg1);

    void on_toolButton_draft_clicked();

signals:
    void sent();
    void reqfile(QString);

private:
    Ui::Write *ui;
    QList<QString> send_to;
    QString subject;
    QList<QString> paths;
    void addFileBtn(QFileInfo);
    QVector<QToolButton *> fileToolBtn;
    int numFile;
    QString content;
    QStringList fileNames;
};

#endif // WRITE_H
