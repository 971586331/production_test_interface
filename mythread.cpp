#include "mythread.h"
#include <qDebug>
#include <Python.h>
#include <QCoreApplication>

MyThread::MyThread()
{

}

void MyThread::run()
{
    //如果没有在环境变量中添加python，这里要写这一句
    Py_SetPythonHome("C:/Users/wb-sy649554/.conda/envs/alibaba_32bit");
    //初始化python模块
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("import os");
    PyRun_SimpleString("sys.path.append('./')");
    //PyRun_SimpleString("print os.getcwd()");

    //获得exe文件的路径
    QString exe_path = QCoreApplication::applicationDirPath();
    QString python_path = "os.chdir(\"" + exe_path + "\")";
    char*  ch;
    QByteArray ba = python_path.toLatin1();
    ch=ba.data();
    //修改python的工作目录
    PyRun_SimpleString(ch);
    //PyRun_SimpleString("print os.getcwd()");

    if ( !Py_IsInitialized() )
    {
        qDebug("Py_IsInitialized error!\n");
        emit signal_python_state(INIT_ERROR);
        this->quit();
    }
    //导入qt_py.py模块
    PyObject* pModule = PyImport_ImportModule("speaker_mic_test_server");
    if (!pModule)
    {
        qDebug("PyImport_ImportModule error!\n");
        emit signal_python_state(IMPORT_ERROR);
        this->quit();
    }
    //获取qt_py模块中的hello函数
    PyObject* pFunhello= PyObject_GetAttrString(pModule,"main");
    if(!pFunhello)
    {
        qDebug("PyObject_GetAttrString error\n");
        emit signal_python_state(GETFUN_ERROR);
        this->quit();
    }
    //调用hello函数
    PyObject * ret = PyObject_CallFunction(pFunhello, NULL);
    if(!ret)
    {
        qDebug("PyObject_CallFunction error\n");
        emit signal_python_state(CALL_ERROR);
        this->quit();
    }
    qDebug("python run ok!\n");
    emit signal_python_state(RUN_NORMAL);
}
