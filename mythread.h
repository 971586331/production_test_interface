#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>


enum e_python_state
{
    INIT_ERROR,      //初始化错误
    IMPORT_ERROR,    //导入错误
    GETFUN_ERROR,   //获得方法错误
    CALL_ERROR,     //调用错误
    RUN_NORMAL,     //运行正常
};

class MyThread : public QThread
{
    Q_OBJECT

public:
    MyThread();

signals:
    void signal_python_state(e_python_state);

protected:
    virtual void run();

};

#endif // MYTHREAD_H
