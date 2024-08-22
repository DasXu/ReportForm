#include "widget.h"
#include <QApplication>
#pragma execution_character_set("utf-8")//正常显示汉字、字符

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 构建配置文件的绝对路径
    QString configFilePath = QCoreApplication::applicationDirPath() + "/server.ini";

    // 使用绝对路径读取配置文件
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(0, "提示", "无法打开配置文件。");
        return false;
    }

    QTextStream in(&configFile);
    QString dbName = in.readLine().trimmed();
    QString hostName = in.readLine().trimmed();
    int port = in.readLine().toInt();
    QString userName = in.readLine().trimmed();;
    QString password = in.readLine().trimmed();;

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dbName);
    db.setHostName(hostName);
    db.setPort(port);
    db.setUserName(userName);
    db.setPassword(password);

//    QStringList arguments = qApp->arguments();
//    if(arguments.size() != 6)
//    {
//        QMessageBox::information(0, "提示", "参数错误。");
//        return false;
//    }
//    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
//    db.setDatabaseName(arguments.at(1));
//    db.setHostName(arguments.at(2));
//    db.setPort(arguments.at(3).toInt());
//    db.setUserName(arguments.at(4));
//    db.setPassword(arguments.at(5));
    if(!db.open())
    {
        QMessageBox::information(NULL,"提示",db.lastError().text());
        qDebug()<<"数据库1 = "<<db.lastError().text();
    }
    Widget w;
    w.show();

    return a.exec();
}
