#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlRecord>
#include <QDebug>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QTableView>
#include <QApplication>
#include <QFileDialog>
#include <QSqlQuery>
#include <QMap>
#include <QThread>

#include "libxl/libxl.h"
#include "detial.h"
#include "product.h"

struct tabledata
{
    QString s0;
    QString s15;
    QDate saveTime;
    int residueNum;

    // 添加一个运算符，用于比较 TableData 是否相等
    bool operator==(const tabledata& other) const
    {
        return s0 == other.s0 && s15 == other.s15;
    }
};//创建结构类型

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void initialpage();
    void initialtable02();//进入程序后的刷新
    void hideEmptyItem01();
    void hideEmptyItem02();
    void showEmptyItem01();
    void showEmptyItem02();
    void Ncount(QDate &date01,QDate &date02);
    void determine();
signals:
    void progressChanged(int progress);

public slots:
    //    void doWork(QDate Ndate);

private slots:
    void on_close_clicked();
    void on_out_clicked();
    void onTableDoubleClicked(int row, int column);
    void SecondInitial();
    void on_refresh_clicked();

    void hideRowState(int state);
    void hideCloumnState(int state);

    void on_product_clicked();
    void onDataSelected(const QList<QStringList> &selectedData);

private:
    Ui::Widget *ui;
    void showArguments(QStringList& arguments);
    bool exportExcelFile(QTableView *v,
                         const QString& sFileName,
                         bool bAddTime/* = true*/);
    QMap<QString, tabledata> DataMap;
    int num=0;
    QStringList chooseProductID;
};

#endif // WIDGET_H
