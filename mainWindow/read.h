#ifndef READ_H
#define READ_H

#include <QWidget>

namespace Ui {
class Read;
}

class Read : public QWidget
{
    Q_OBJECT

public:
    explicit Read(QWidget *parent = nullptr);
    ~Read();
    void setFrom(QString);
    void setSubject(QString);
    void setContent(QString);
    void setAppendix(QString);

private slots:
    void on_toolButton_clicked();

signals:
    void closeread();
    void req_file(QString);

private:
    Ui::Read *ui;
};

#endif // READ_H
