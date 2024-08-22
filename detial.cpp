#include "detial.h"
#include "ui_detial.h"

#pragma execution_character_set("utf-8")//正常显示汉字、字符

detial::detial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::detial)
{
    ui->setupUi(this);
    ui->table_02->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑表格
    ui->table_03->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑表格
    ui->table_02->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->table_02->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色
    ui->table_02->verticalHeader()->setStyleSheet("QHeaderView::section{background: lightblue;}");
    ui->table_02->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//表格充满布局
    ui->table_03->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头对齐设置
    ui->table_03->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色
    ui->table_03->verticalHeader()->setStyleSheet("QHeaderView::section{background: lightblue;}");
    ui->table_03->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//表格充满布局

    ui->frame->setFrameShape(QFrame::Box);  // 设置边框形状
    ui->frame->setFrameShadow(QFrame::Sunken);  // 设置边框阴影效果
    ui->frame_2->setFrameShape(QFrame::Box);  // 设置边框形状
    ui->frame_2->setFrameShadow(QFrame::Sunken);  // 设置边框阴影效果
    connect(ui->table_02, &QTableWidget::itemSelectionChanged, this, &detial::TableSelectionChanged);
}

detial::~detial()
{
    delete ui;
}

void detial::on_close_clicked()
{
    close();
}

void detial::on_out_clicked()
{
    QString ProductName = ui->a02_order->text();
    exportExcelFile(ui->table_02, ProductName, true);
}

void detial::on_out_2_clicked()
{
    QString ProductName = ui->a02_name->text();
    QString process = ui->a02_order->text();
    QString name = QString(ProductName + "_" +process);
    exportExcelFile(ui->table_03, name, true);
}

void detial::showArguments(QStringList &arguments)
{
    ui->table_02->setColumnCount(1);
    ui->table_02->setRowCount(arguments.count());
    ui->table_02->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList title;
    title << ("参数");
    ui->table_02->setHorizontalHeaderLabels(title);

    int nRow = 0;
    foreach(QString arg, arguments)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(arg);
        ui->table_02->setItem(nRow++, 0, item);
    }
}

bool detial::exportExcelFile(QTableView *v,
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

void detial::setData(int column, const QStringList &rowHeader,const QStringList &text,const QString &colHeader,
                     const QStringList &productIDs, const QString &date,const QString &date2, const int &num)
{
//    for(int i=0;i<chooseID.size();i++)
//    {
//        qDebug()<<i<<"=="<<chooseID.at(i);
//    }
    Ndate01 = date;
    Ndate02 = date2;
    QString processkey;
    num02 = num;
    if(num02 == 1)
        processkey = "processName";
    else if(num02 == 0)
        processkey = "processIndex";
    else
    {
        return;
    }
    QSqlQuery query,query2;
    ui->a02_order->setText(colHeader);
    ui->table_02->setRowCount(rowHeader.size());
    for(int i=0;i<rowHeader.size();i++)
    {
        QString ProductName = rowHeader.at(i);
        QTableWidgetItem *item01 = new QTableWidgetItem(ProductName);
        ui->table_02->setItem(i,0,item01);
        QString entry = text.at(i);
        QStringList parts = entry.split(",");//通过逗号拆分内容
        if (parts.size() == 2)
        {
            QTableWidgetItem *item1 = new QTableWidgetItem(parts.at(0).trimmed());
            QTableWidgetItem *item2 = new QTableWidgetItem(parts.at(1).trimmed());
            ui->table_02->setItem(i,2,item1);
            ui->table_02->setItem(i,3,item2);
        }

//        QString id;
//        if(!chooseID .isEmpty())
//            id = chooseID.at(i);
//        else
//            id = productIDs.at(i);
        QString id = productIDs.at(i);
        QString sql = QString("select processID from cappprocesstab where processID like :id and %1 = :name").arg(processkey);
        query.prepare(sql);
        query.bindValue(":id",id + "%");
        query.bindValue(":name",colHeader);
        if(query.exec())
        {
            QSet<QString> LtextBefore;//存储上一条记录的前八位    数据结构自动去重
            while (query.next())
            {
                QString id001 = query.value(0).toString().left(8);
                QString id002 = query.value(0).toString().right(2);
                query2.prepare("select distinct s0 from planqrtab where s0 like :id and Residuenum!=0 and date(saveTime) between :date and :date2");
                query2.bindValue(":id",id001 + "%" +id002);
                query2.bindValue(":date",Ndate01);
                query2.bindValue(":date2",Ndate02);
                if(query2.exec())
                {
                    while (query2.next())
                    {
                        QString LtextNow = query2.value(0).toString().left(8);
                        LtextBefore.insert(LtextNow);
                    }
                    int a = LtextBefore.size();
                    QString text01 = QString("%1条").arg(a);
                    QTableWidgetItem *item3 = new QTableWidgetItem(text01);
                    ui->table_02->setItem(i,1,item3);
                }
                else
                {
                    qDebug()<<"001"<<query2.lastError().text();
                }
            }
        }
        else
        {
            qDebug()<<"002"<<query.lastError().text();
        }
    }
}

void detial::TableSelectionChanged()
{
    QString processKey;
    if(num02 ==1)
        processKey = "processName";
    else if(num02==0)
        processKey = "processIndex";
    else
    {
        return;
    }

    QStringList capNow,s15,residuenum;
    QString name = ui->table_02->item(ui->table_02->selectedItems().first()->row(), 0)->text();
    ui->a02_name->setText(name);
    QString order = ui->a02_order->text();
    QSqlQuery query,query01;
    query.prepare("select productID from producttab where productName = :name");
    query.bindValue(":name",name);
    if(query.exec() && query.first())
    {
        QString id = query.value(0).toString();
        QString sql = QString("select ProcessID from cappprocesstab where ProcessID like :id and %1 = :order").arg(processKey);
        query.prepare(sql);
        query.bindValue(":id",id + "%");
        query.bindValue(":order",order);
        if(query.exec())
        {
            int queryCount =0;

            while(query.next())
            {
                QString id01 = query.value(0).toString().left(8);
                QString id02 = query.value(0).toString().right(2);
                query01.prepare("select MIN(s0) AS s0,s15,sum(Residuenum) AS TotalResiduenum from planqrtab where s0 like :id and date(saveTime) between :date and :date2 and Residuenum != 0 group by s15");
                query01.bindValue(":id",id01 + "%" + id02);
                query01.bindValue(":date",Ndate01);
                query01.bindValue(":date2",Ndate02);
                if(query01.exec())
                {
                    int queryCount01 = query01.size();
                    while (query01.next())
                    {
                        capNow.append(query01.value(0).toString().mid(6,2));
                        s15.append(query01.value(1).toString());
                        residuenum.append(query01.value(2).toString());
                    }
                    queryCount += queryCount01;
                }
                else
                {
                    qDebug()<<"03"<<query.lastError().text();
                }
                ui->table_03->setRowCount(queryCount);

                for(int i =0;i<queryCount;i++)
                {
                    QTableWidgetItem *item1 = new QTableWidgetItem(capNow.at(i));
                    QTableWidgetItem *item2 = new QTableWidgetItem(s15.at(i));
                    QTableWidgetItem *item3 = new QTableWidgetItem(residuenum.at(i));
                    ui->table_03->setItem(i,0,item1);
                    ui->table_03->setItem(i,1,item2);
                    ui->table_03->setItem(i,2,item3);
                }
            }
        }
        else
        {
            qDebug()<<"02"<<query.lastError().text();
        }
    }
    else
    {
        qDebug()<<"01"<<query.lastError().text();
    }
}
