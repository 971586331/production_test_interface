#ifndef JSONHANDLE_H
#define JSONHANDLE_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class JsonHandle : public QObject
{
    Q_OBJECT
public:
    explicit JsonHandle(QObject *parent = nullptr);

    int find_cmd_list(QJsonArray obj, QString str);
    int find_info_list(QJsonArray obj, QString str);
    void delete_cmd_list(int i);

    QFile *file;                    //配置文件的句柄
    QJsonObject rootObj;            //配置文件的主节点

signals:

public slots:

};

#endif // JSONHANDLE_H
