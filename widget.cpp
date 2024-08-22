#include "widget.h"
#include "ui_widget.h"

#pragma execution_character_set("utf-8")//正常显示汉字、字符

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("在制数详情");
    QDate date01 = QDate::currentDate();
    int currentYear = date01.year();
    ui->year_1->setDate(QDate(currentYear,1,1));
    ui->year_2->setDate(date01);

    ui->table_01->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑表格
    ui->progressBar->hide();
    determine();//判断并赋予num值
    initialpage();

    connect(ui->table_01, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onTableDoubleClicked(int, int)));//关联单元格
    connect(ui->hide_row, SIGNAL(stateChanged(int)), this, SLOT(hideRowState(int)));//隐藏空白行
    connect(ui->hide_cloumn, SIGNAL(stateChanged(int)), this, SLOT(hideCloumnState(int)));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_close_clicked()
{
    close();
}

void Widget::on_out_clicked()
{
    exportExcelFile(ui->table_01, ("产品在制数"), true);
}

void Widget::on_refresh_clicked()
{
    SecondInitial();
}

void Widget::on_product_clicked()
{
    product *pro = new product();
    pro->setWindowTitle("产品选择");
    connect(pro, &product::dataSelected, this, &Widget::onDataSelected);
    pro->show();
}

void Widget::initialpage()
{
    QSqlQuery query;
    QString keyword;
    if(num == 1)
        keyword = "processName";
    else if(num == 0)
        keyword = "processIndex";
    else
    {
        return;
    }
    QString sql = QString ("select distinct %1 from cappprocesstab").arg(keyword);
    query.prepare(sql);
    if(query.exec())
    {
        QStringList headerLabels;  // 用于存储列的标签
        while (query.next())
        {
            headerLabels << query.value(0).toString();  // 将内容添加到列的标签列表中
        }
        ui->table_01->insertColumn(0);  // 在第一列之前插入一个空白列
        ui->table_01->setColumnCount(headerLabels.count() + 1);  // 设置列数为标签的数量
        ui->table_01->setHorizontalHeaderItem(0, new QTableWidgetItem("产品"));
        for (int i = 1; i <= headerLabels.count(); ++i)
        {
            QTableWidgetItem *item = new QTableWidgetItem(headerLabels.at(i - 1));
            ui->table_01->setHorizontalHeaderItem(i, item);
        }
    }
    ui->table_01->setColumnWidth(0,180);
    ui->table_01->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->table_01->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色
    ui->table_01->verticalHeader()->setStyleSheet("QHeaderView::section{background: lightblue;}");
}

void Widget::showArguments(QStringList &arguments)
{
    ui->table_01->setColumnCount(1);
    ui->table_01->setRowCount(arguments.count());
    ui->table_01->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList title;
    title << ("参数");
    ui->table_01->setHorizontalHeaderLabels(title);

    int nRow = 0;
    foreach(QString arg, arguments)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(arg);
        ui->table_01->setItem(nRow++, 0, item);
    }
}

bool Widget::exportExcelFile(QTableView *v,
                             const QString& sFileName,
                             bool bAddTime/* = true*/)
{
    QString s;
    QDateTime dt;
    double fScale = 6;
    int nCol, nRow, nTotalRow, nTotalCol, nColExcel;

    // 1 判断表格中未显示任何记录
    nTotalRow = v->model()->rowCount();
    nTotalCol = v->model()->columnCount();
    if (nTotalRow < 1)
    {
        QMessageBox::information(0, ("提示"), ("没有可导出的数据！"));
        return false;
    }

    // 2 打开book
    libxl::Book* book = xlCreateBook(); // use xlCreateXMLBook() for working with xlsx files
    book->setKey( L"GCCG", L"windows-282123090cc0e6036db16b60a1o3p0h9" );
    libxl::Sheet* sheet = book->addSheet(L"导出报表");

    libxl::Format *format= book->addFormat();
    format->setAlignH(libxl::ALIGNH_LEFT);
    format->setAlignV(libxl::ALIGNV_CENTER);
    format->setBorderTop(libxl::BORDERSTYLE_THIN);
    format->setBorderLeft(libxl::BORDERSTYLE_THIN);
    format->setBorderBottom(libxl::BORDERSTYLE_THIN);
    format->setBorderRight(libxl::BORDERSTYLE_THIN);

    // 3 写表头
    nColExcel = 0;
    for (nCol = 0; nCol < nTotalCol; nCol++)
    {
        if(v->isColumnHidden(nCol)) continue;

        s = v->model()->headerData(nCol, Qt::Horizontal).toString();
        sheet->setCol(nColExcel, nColExcel+1, v->columnWidth(nCol)/fScale);
        sheet->writeStr(0, nColExcel,
                        reinterpret_cast<const wchar_t *>(s.utf16()), format);
        nColExcel++;
    }
    nTotalCol = nCol;

    // 4 写每一行
    nColExcel = 0;
    for(nCol = 0; nCol < nTotalCol; nCol++)
    {
        if(v->isColumnHidden(nCol)) continue;
        for(nRow = 0; nRow < nTotalRow; nRow++)
        {
            s = v->model()->data(v->model()->index(nRow, nCol)).toString();
            sheet->writeStr(nRow+1, nColExcel,
                            reinterpret_cast<const wchar_t *>(s.utf16()), format);
        }
        nColExcel++;
    }

    // 5 保存文件
    if (bAddTime)
    {
        dt = QDateTime::currentDateTime();
        s = QString("%1 [ %2 ]").arg(sFileName)
                .arg( QDateTime::currentDateTime()
                      .toString(("yyyy-MM-dd hh：mm：ss"))); //!
    }
    else s = sFileName;

    s = QFileDialog::getSaveFileName(0, ("导出"), s, ("Excel文件(*.xls)"));
    if(s.isEmpty())
    {
        book->release();
        return true;
    }
    if( !book->save(reinterpret_cast<const wchar_t *>(s.utf16())) )
    {
        book->release();
        QMessageBox::information(0, ("提示"), ("保存Excel文件失败！"));
        return false;
    }
    book->release();

    // 6 打开Excel文件
    int res = QMessageBox::question(0, ("提示"), ("导出成功。请问是否打开文件?"),
                                    QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes)
    {

        QDesktopServices::openUrl( QUrl::fromLocalFile(s) );
    }

    return true;
}

//更改时间后的计算
void Widget::initialtable02()
{
    QDate Ndate01 = ui->year_1->date();//获取开始日期
    QDate Ndate02 = ui->year_2->date();//获取结束日期
    QDate Ndate03 = QDate::currentDate();
    if(Ndate01 > Ndate03 || Ndate02 > Ndate03)
    {
        QMessageBox::warning(NULL,"提示","不能超过当前日期!");
        return;
    }
    else
    {
        if(Ndate01 >= Ndate02)
        {
            QMessageBox::information(NULL,"提示","开始时间晚于结束时间,已将开始时间与结束时间对调");
            Ncount(Ndate02,Ndate01);
        }
        else
        {
            Ncount(Ndate01,Ndate02);
        }
    }
}

void Widget::onTableDoubleClicked(int row, int column)
{
    QString date = ui->year_1->date().toString("yyyy-MM-dd");
    QString date2= ui->year_2->date().toString("yyyy-MM-dd");

    QString name = ui->table_01->item(row,0)->text();
    QStringList rowHeader,text01,text02;
    if(column == 0)
    {

    }
    else
    {
        for (int row = 0; row < ui->table_01->rowCount(); ++row)
        {
            QTableWidgetItem *itemInColumn = ui->table_01->item(row, column);
            if (itemInColumn && !itemInColumn->text().isEmpty())
            {
                rowHeader.append(ui->table_01->item(row, 0)->text());
                text01.append(ui->table_01->item(row, column)->text());
                text02.append(ui->table_01->item(row,0)->data(Qt::UserRole).toString());
            }
        }
        QString colHeader = ui->table_01->horizontalHeaderItem(column)->text();
        int num01 = num;
        // 创建子窗口并传递信息
        detial *de = new detial();
        de->setWindowTitle("工序——产品在制详情");
        de->raise();
        de->setData(column, rowHeader,text01,colHeader,text02,date,date2,num01);
        de->show();
    }
}

void Widget::hideEmptyItem01()    //隐藏空白行
{
    for(int row = 0; row<ui->table_01->rowCount(); ++row)
    {
        bool isEmptyRow = true;
        for(int col = 1; col < ui->table_01->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->table_01->item(row, col);

            if(item && !item->text().isEmpty())
            {
                isEmptyRow = false;
                break;
            }
        }
        ui->table_01->setRowHidden(row, isEmptyRow);
    }
}

void Widget::hideEmptyItem02()    //隐藏空白列
{
    for(int col = 1; col < ui->table_01->columnCount(); ++col)
    {
        bool isEmptyCol = true;
        for(int row = 0; row<ui->table_01->rowCount(); ++row)
        {
            QTableWidgetItem *item = ui->table_01->item(row, col);

            if(item && !item->text().isEmpty())
            {
                isEmptyCol = false;
                break;
            }
        }
        ui->table_01->setColumnHidden(col, isEmptyCol);
    }
}

void Widget::showEmptyItem01()    //显示空白行
{
    for (int row = 0; row < ui->table_01->rowCount(); ++row)
    {
        ui->table_01->setRowHidden(row, false);
    }
}

void Widget::showEmptyItem02()    //显示空白列
{
    for (int col = 0; col < ui->table_01->columnCount(); ++col)
    {
        ui->table_01->setColumnHidden(col, false);
    }
}

void Widget::Ncount(QDate &date01,QDate &date02)      //通用计算函数
{
    QString processKey;
    if(num == 1)
        processKey = "processName";
    else if(num == 0)
        processKey = "processIndex";
    else
    {
        return;
    }

    ui->progressBar->show();
    int currentQueryCount = 0;  // 初始化查询计数器

    int columnCount01 = ui->table_01->columnCount();//获取表格列数
    QSqlQuery query;
    QSqlQuery query2;
    QSqlQuery query3;
    query.prepare("select productID,productName from producttab");
    if(query.exec())
    {
        int a = 0;
        int b = 0;
        int totalQueryCount = query.size();
        while (query.next())
        {
            ++currentQueryCount;
            // 计算进度并更新进度条
            int progress = static_cast<int>((static_cast<double>(currentQueryCount) / totalQueryCount) * 100);
            ui->progressBar->setValue(progress);
            qApp->processEvents();  // 实时更新UI

            QString text0 = query.value(0).toString();
            ui->table_01->insertRow(b);
            QTableWidgetItem *item = new QTableWidgetItem(query.value(1).toString());
            if (item)
            {
                item->setBackground(QBrush(QColor(173, 216, 230)));  // 设置背景颜色为蓝色
                item->setData(Qt::UserRole,text0);
            }
            ui->table_01->setItem(b,0,item);
            b++;//将id号和零件名对应

            for(int i=1;i<columnCount01;i++)
            {
                QString processIndex = ui->table_01->horizontalHeaderItem(i)->text();
                QString ID = text0;
                QString sql =QString("select processID from cappprocesstab where %1 = :Index and processID like :id").arg(processKey);
                query2.prepare(sql);
                query2.bindValue(":Index",processIndex);
                query2.bindValue(":id",ID + "%");
                if(query2.exec())
                {
                    int sumCount = 0;
                    int sumNumber = 0;
                    while(query2.next())
                    {
                        QString ID01 = query2.value(0).toString().left(8);
                        QString ID02 = query2.value(0).toString().right(2);
                        query3.prepare("select s15, SUM(Residuenum) AS ResiduenumSum from planqrtab where s0 like :id and date(saveTime) between :date and :date2 and Residuenum != 0  group by s15");
                        query3.bindValue(":id",ID01 + "%" + ID02);
                        query3.bindValue(":date",date01);
                        query3.bindValue(":date2",date02);
                        if(query3.exec())
                        {
                            int totalCount = query3.size();
                            sumCount += totalCount;
                            while (query3.next())
                            {
                                int c = query3.value(1).toInt();
                                sumNumber += c;
                            }
                            if(sumCount != 0 || sumNumber != 0)
                            {
                                QString text3 = QString("%1批,%2个").arg(sumCount).arg(sumNumber);
                                QTableWidgetItem *item = new QTableWidgetItem(text3);
                                ui->table_01->setItem(a,i,item);
                            }
                        }
                    }
                }
                else
                {
                    qDebug()<<"02:"<<query2.lastError().text();
                }
            }
            a++;
        }
        ui->progressBar->hide();
        ui->hide_row->setChecked(true);
        ui->hide_cloumn->setChecked(true);
        hideRowState(Qt::Checked);
        hideCloumnState(Qt::Checked);
    }
    else
    {
        qDebug()<<"01:"<<query.lastError().text();
    }
}



void Widget::SecondInitial()
{
    ui->table_01->setRowCount(0);  // 清空表格的行
    ui->table_01->clearContents(); // 清空表格的内容
    initialpage();
    initialtable02();
}

void Widget::hideRowState(int state) //复选框隐藏功能实现
{
    if (state == Qt::Checked)
    {
        hideEmptyItem01();// 勾选隐藏空白行时执行的函数
    }
    else
    {
        showEmptyItem01();// 取消勾选隐藏空白行时执行的函数
    }
}

void Widget::hideCloumnState(int state)
{
    if (state == Qt::Checked)
    {
        hideEmptyItem02();
    }
    else
    {
        showEmptyItem02();
    }
}

void Widget::determine()//查看公司数据库的aa_systemset的F02值
{
    QSqlQuery query;
    query.prepare("select SetValue from aa_systemset where SetID = 'F02'");
    if(query.exec() && query.next())
    {
        num = query.value(0).toInt();
    }
}

void Widget::onDataSelected(const QList<QStringList> &selectedData)//选中产品后更新
{
    QString processKey;
    if(num == 1)
        processKey = "processName";
    else if(num == 0)
        processKey = "processIndex";
    else
    {
        // 处理未知的类型
        return;
    }
    ui->table_01->setRowCount(0);  // 清空表格的行
    ui->table_01->clearContents(); // 清空表格的内容
    initialpage();
    QDate date01 = ui->year_1->date();//获取开始日期
    QDate date02 = ui->year_2->date();//获取结束日期

    int columnCount01 = ui->table_01->columnCount();//获取表格列数
    QSqlQuery query2;
    QSqlQuery query3;
    //遍历传入的信息，并填充行表头
    ui->progressBar->show();
    int TotalCount = selectedData.size();  // 初始化查询计数器
    for (const QStringList& rowData : selectedData)
    {
        // 获取第一列和第二列信息
        QString column1 = rowData.at(0); // 第一列信息  产品id
        QString column2 = rowData.at(1); // 第二列信息  产品名
        int row = ui->table_01->rowCount(); // 获取当前行数
        ui->table_01->insertRow(row); // 插入新行
        QTableWidgetItem *item = new QTableWidgetItem(column2);
        item->setBackground(QBrush(QColor(173, 216, 230)));  // 设置背景颜色为蓝色
        item->setData(Qt::UserRole,column1);
        ui->table_01->setItem(row, 0, item); // 填充第一列信息

        int progress = static_cast<int>((static_cast<double>(row+1) / TotalCount) * 100);
        ui->progressBar->setValue(progress);
        qApp->processEvents();  // 实时更新UI

        //逐列计算并填充
        for(int i=1;i<columnCount01;i++)
        {
            QString processIndex = ui->table_01->horizontalHeaderItem(i)->text();
            QString sql = QString("select processID from cappprocesstab where %1 = :Index and processID like :id").arg(processKey);
            query2.prepare(sql);
            query2.bindValue(":Index",processIndex);
            query2.bindValue(":id",column1 + "%");
            if(query2.exec())
            {
                int sumCount = 0;
                int sumNumber = 0;
                while(query2.next())
                {
                    QString ID01 = query2.value(0).toString().left(8);
                    QString ID02 = query2.value(0).toString().right(2);

                    query3.prepare("select s15, SUM(Residuenum) AS ResiduenumSum from planqrtab where s0 like :id and date(saveTime) between :date and :date2 and Residuenum != 0  group by s15");
                    query3.bindValue(":id",ID01 + "%" + ID02);
                    query3.bindValue(":date",date01);
                    query3.bindValue(":date2",date02);
                    if(query3.exec())
                    {
                        int totalCount = query3.size();
                        sumCount += totalCount;
                        while (query3.next())
                        {
                            int c = query3.value(1).toInt();
                            sumNumber += c;
                        }
                        if(sumCount != 0 || sumNumber != 0)
                        {
                            QString text3 = QString("%1批,%2个").arg(sumCount).arg(sumNumber);
                            QTableWidgetItem *item = new QTableWidgetItem(text3);
                            ui->table_01->setItem(row,i,item);
                        }
                    }
                }
            }
            else
            {
                qDebug()<<"02:"<<query2.lastError().text();
            }
        }
        chooseProductID.append(column1);//存入字符串列表，用于传送至下一界面
    }
    ui->progressBar->hide();
    ui->hide_row->setChecked(true);
    ui->hide_cloumn->setChecked(true);
    hideCloumnState(Qt::Checked);
}
