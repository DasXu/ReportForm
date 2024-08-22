#ifndef DETIAL_H
#define DETIAL_H

#include <QWidget>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QTableView>
#include <QApplication>
#include <QFileDialog>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include "libxl/libxl.h"

namespace Ui {
class detial;
}

class detial : public QWidget
{
    Q_OBJECT

public:
    explicit detial(QWidget *parent = nullptr);
    ~detial();

    void setData(int column, const QStringList &rowHeader,const QStringList &text, const QString &colHeader,
                 const QStringList &productIDs, const QString &date,const QString &date2, const int &num);

public slots:
    void TableSelectionChanged();
private slots:
    void on_close_clicked();

    void on_out_clicked();

    void on_out_2_clicked();

private:
    Ui::detial *ui;
    void showArguments(QStringList& arguments);
    bool exportExcelFile(QTableView *v,
                         const QString& sFileName,
                         bool bAddTime/* = true*/);
    QString Ndate01;
    QString Ndate02;
    int num02 = 0;
};

#endif // DETIAL_H
