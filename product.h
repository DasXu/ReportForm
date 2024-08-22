#ifndef PRODUCT_H
#define PRODUCT_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QEvent>//监视窗口变化

namespace Ui {
class product;
}

class product : public QWidget
{
    Q_OBJECT

public:
    explicit product(QWidget *parent = nullptr);
    ~product();

    void tableshow();
    void dataBase01();

    void typeshow();

    void fliter();
signals:
    // 定义信号，用于发送选中行的信息
    void dataSelected(const QList<QStringList> &selectedData);

private slots:
    void on_ok_clicked();

    void on_cancel_clicked();

    void on_product_fliter_clicked();

    void on_product_show_clicked();

private:
    Ui::product *ui;
};

#endif // PRODUCT_H
