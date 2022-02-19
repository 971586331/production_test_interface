#include "jsonhandle.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

JsonHandle::JsonHandle(QObject *parent) : QObject(parent)
{
    //打开配置文件
    file = new QFile(QCoreApplication::applicationDirPath() + "/config.json");
    if(!file->open(QIODevice::ReadOnly))
    {
        qDebug() << "配置文件打开失败！";
        QMessageBox::information(NULL, "错误", "打开配置文件错误！", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    //读出json文档
    QJsonDocument jdc(QJsonDocument::fromJson(file->readAll()));
    file->close();
    rootObj = jdc.object();
    if( rootObj.isEmpty() == true )
    {
        qDebug() << "配置文件主节点为空！";
        QMessageBox::information(NULL, "错误", "配置文件主节点为空！", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    //获得test_flow中的测试流程
    QJsonArray test_flow_Array = rootObj.value("test_flow").toArray();
    if( test_flow_Array.isEmpty() == true )
    {
        qDebug() << "配置文件中测试流程为空！";
        QMessageBox::information(NULL, "警告", "配置文件中测试流程为空！", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }


    //获得cmd_list中的所有指令
    QJsonArray cmd_list_Array = rootObj.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        qDebug() << "配置文件中指令列表为空！";
        QMessageBox::information(NULL, "警告", "配置文件中指令列表为空！", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }

    qDebug() << "test_flow_Array = " << test_flow_Array.size();
    qDebug() << "cmd_list_Array = " << cmd_list_Array.size();
}

/**
 * @brief MainWindow::find_cmd_list 根据指令从cmd_list中查找相应的项
 * @param obj   cmd_list
 * @param str   指令字符串
 * @return      指令在cmd_list中的位置
 */
int JsonHandle::find_cmd_list(QJsonArray obj, QString str)
{
    //遍历obj
    int i = 0;

    if( obj.isEmpty() == true )
        return -1;
    for(i=0; i<obj.size(); i++)
    {
        if( obj[i].toObject()["指令"].toString() == str )
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief MainWindow::find_cmd_list 根据需求信息从cmd_list中查找相应的项
 * @param obj   cmd_list
 * @param str   指令字符串
 * @return      指令在cmd_list中的位置
 */
int JsonHandle::find_info_list(QJsonArray obj, QString str)
{
    //遍历obj
    int i = 0;

    if( obj.isEmpty() == true )
        return -1;
    for(i=0; i<obj.size(); i++)
    {
        if( obj[i].toObject()["需求信息"].toString() == str )
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief JsonHandle::delete_cmd_list   删除cmd_list中的第i项
 * @param i
 */
void JsonHandle::delete_cmd_list(int i)
{
    rootObj.value("cmd_list").toArray().removeAt(i);
}
