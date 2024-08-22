#include "product.h"
#include "ui_product.h"

#include <QWindowStateChangeEvent>

#pragma execution_character_set("utf-8")//正常显示汉字、字符

product::product(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::product)
{
    ui->setupUi(this);
    ui->frame->setFrameShape(QFrame::Box);  // 设置边框形状
    ui->frame->setFrameShadow(QFrame::Sunken);  // 设置边框阴影效果
    ui->productTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//表格充满布局

    tableshow();//初始化产品信息
    typeshow();//初始化控件内容
}

product::~product()
{
    delete ui;
}

void product::on_ok_clicked()
{
    // 获取选中的行
    QList<QTableWidgetItem*> selectedItems = ui->productTab->selectedItems();

    // 创建用于存储选中数据的列表
    QList<QStringList> selectedData;

    // 用于存储已添加的行索引
    QSet<int> addedRows;

    // 遍历选中的项，将数据添加到列表中
    for (QTableWidgetItem* item : selectedItems)
    {
        int row = item->row();

        // 检查该行是否已经添加过
        if (!addedRows.contains(row))
        {
            // 获取该行的所有数据
            QStringList rowData;
            for (int c = 0; c < ui->productTab->columnCount(); ++c)
            {
                rowData << ui->productTab->item(row, c)->text();
            }

            // 将数据添加到列表中
            selectedData << rowData;

            // 将行索引添加到已添加的集合中
            addedRows.insert(row);
        }
    }
    close();
    emit dataSelected(selectedData);
}

void product::on_cancel_clicked()
{
    close();
}

void product::on_product_fliter_clicked()
{
    fliter();
}

void product::on_product_show_clicked()
{
    tableshow();
}

void product::tableshow()
{
    ui->productTab->horizontalHeader()->setStyleSheet("QHeaderView::section{background:lightblue;}"); //skyblue设置表头背景色
    ui->productTab->verticalHeader()->setStyleSheet("QHeaderView::section{background: lightblue;}");
    QSqlQuery query;
    query.prepare("select productID,productName,ClientName,clientProductID,productState,stockcount from a10_productview");
    if(!query.exec())
    {
        qDebug()<<query.lastError().text();
    }
    else
    {
        int rowCount = query.size();
        if (rowCount > 0)
        {
            // 设置 QTableWidget 的行数
            ui->productTab->setRowCount(rowCount);

            // 处理查询结果
            int rowNumber = 1;  // 初始序号
            while (query.next()) {
                // 在第一列添加序号
                // 处理其他列的数据
                QString productID = query.value(0).toString();
                QString productName = query.value(1).toString();
                QString clientName = query.value(2).toString();
                QString clientProductID = query.value(3).toString();
                QString productState = query.value(4).toString();
                int stockCount = query.value(5).toInt();

                // 设置其他列的数据
                ui->productTab->setItem(rowNumber - 1, 0, new QTableWidgetItem(productID));
                ui->productTab->setItem(rowNumber - 1, 1, new QTableWidgetItem(productName));
                ui->productTab->setItem(rowNumber - 1, 2, new QTableWidgetItem(clientName));
                ui->productTab->setItem(rowNumber - 1, 3, new QTableWidgetItem(clientProductID));
                ui->productTab->setItem(rowNumber - 1, 4, new QTableWidgetItem(productState));
                ui->productTab->setItem(rowNumber - 1, 5, new QTableWidgetItem(QString::number(stockCount)));

                // 增加序号
                ++rowNumber;
            }
        }
    }
}

void product::typeshow()
{
    QHeaderView *header = ui->productTab->horizontalHeader();

    // 获取表头的列数
    int columnCount = header->count();

    // 遍历表头项，将其文本添加到 QComboBox 中
    for (int i = 0; i < columnCount; ++i)
    {
        QString columnHeaderText = header->model()->headerData(i, Qt::Horizontal).toString();
        ui->product_type->addItem(columnHeaderText);
    }

    QStringList list;
    list<<"包含"<<"等于";
    ui->product_include->addItems(list);
}

void product::fliter()
{
    ui->productTab->setRowCount(0);
    QString text1 = ui->product_type->currentText();
    QString text2 = ui->product_include->currentText();
    QString text3 = ui->product_text->text();
    QSqlQuery query;
    QString columnName;
    // 判断选择的类型，并设置对应的列名
    if (text1 == "产品编号")
        columnName = "productID";
    else if (text1 == "产品名称")
        columnName = "productName";
    else if (text1 == "客户名称")
        columnName = "ClientName";
    else if (text1 == "客户产品编号")
        columnName = "clientProductID";
    else if (text1 == "产品状态")
        columnName = "productState";
    else if (text1 == "库存数量")
        columnName = "stockcount";
    else
    {
        // 处理未知的类型
        return;
    }

    if (text2 == "包含")
    {
        QString sql = QString("SELECT productID, productName, ClientName, clientProductID, productState, stockcount FROM a10_productview WHERE %1 like :id").arg(columnName);
        query.prepare(sql);
        query.bindValue(":id", '%' + text3 + '%');
        if(query.exec())
        {
            int rowCount = query.size();
            if (rowCount > 0)
            {
                ui->productTab->setRowCount(rowCount);
                int rowNumber = 1;  // 初始序号
                while (query.next())
                {
                    QString productID = query.value(0).toString();
                    QString productName = query.value(1).toString();
                    QString clientName = query.value(2).toString();
                    QString clientProductID = query.value(3).toString();
                    QString productState = query.value(4).toString();
                    int stockCount = query.value(5).toInt();
                    ui->productTab->setItem(rowNumber - 1, 0, new QTableWidgetItem(productID));
                    ui->productTab->setItem(rowNumber - 1, 1, new QTableWidgetItem(productName));
                    ui->productTab->setItem(rowNumber - 1, 2, new QTableWidgetItem(clientName));
                    ui->productTab->setItem(rowNumber - 1, 3, new QTableWidgetItem(clientProductID));
                    ui->productTab->setItem(rowNumber - 1, 4, new QTableWidgetItem(productState));
                    ui->productTab->setItem(rowNumber - 1, 5, new QTableWidgetItem(QString::number(stockCount)));
                    ++rowNumber;
                }
            }
        }
    }
    else if (text2 == "等于")
    {
        QString sql = QString("SELECT productID, productName, ClientName, clientProductID, productState, stockcount FROM a10_productview WHERE %1 = :id").arg(columnName);
        query.prepare(sql);
        query.bindValue(":id",text3);
        if(query.exec())
        {
            int rowCount = query.size();
            if (rowCount > 0)
            {
                ui->productTab->setRowCount(rowCount);
                int rowNumber = 1;  // 初始序号
                while (query.next())
                {
                    QString productID = query.value(0).toString();
                    QString productName = query.value(1).toString();
                    QString clientName = query.value(2).toString();
                    QString clientProductID = query.value(3).toString();
                    QString productState = query.value(4).toString();
                    int stockCount = query.value(5).toInt();
                    ui->productTab->setItem(rowNumber - 1, 0, new QTableWidgetItem(productID));
                    ui->productTab->setItem(rowNumber - 1, 1, new QTableWidgetItem(productName));
                    ui->productTab->setItem(rowNumber - 1, 2, new QTableWidgetItem(clientName));
                    ui->productTab->setItem(rowNumber - 1, 3, new QTableWidgetItem(clientProductID));
                    ui->productTab->setItem(rowNumber - 1, 4, new QTableWidgetItem(productState));
                    ui->productTab->setItem(rowNumber - 1, 5, new QTableWidgetItem(QString::number(stockCount)));
                    ++rowNumber;
                }
            }
        }
    }
}
