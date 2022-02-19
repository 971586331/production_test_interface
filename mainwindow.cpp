#include "mainwindow.h"
#include "main.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <Python.h>
#include "mythread.h"
#include <QMenuBar>
#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QTreeWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QCoreApplication>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioRecorder>
#include <qcustomplot.h>
#include "fftw-3.3.5-dll32/fftw3.h"

#define BOARD_SAMPLE_RATE  (16000)
#define BOARD_BIT_RATE     (16)
#define BOARD_CHANNEL       (2)
#define BOARD_BYTE_RATE    (2)

#define PC_SAMPLE_RATE  (48000)
#define PC_BIT_RATE     (16)
#define PC_CHANNEL       (2)
#define PC_BYTE_RATE    (2)

/**
 * @brief MainWindow::MainWindow   构造函数
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //初始化ui
    ui_init();

    //线程用于执行python
    thread1 = new MyThread();
    connect(thread1,SIGNAL(signal_python_state(e_python_state)),this,SLOT(signal_python_state_slots(e_python_state)));
    thread1->start();

    //创建一个定时器，用来驱动状态机
    fsm_timer = new QTimer;
    fsm_timer->setSingleShot(false);    //设置定时器是否为单次触发。默认为 false 多次触发
    connect(fsm_timer, SIGNAL(timeout()), this, SLOT(slot_fsm_timer_timeout()));

    //指令发送成功信号槽
    connect(this, SIGNAL(signal_cmd_execute_successfully(struct com_struct)), this, SLOT(slot_cmd_execute_successfully(struct com_struct)));

    //判断系统中是否存在麦克风
    check_microphone();

    //录音初始化
    audioRecorder = new QAudioRecorder;
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/pcm");
    audioSettings.setSampleRate(PC_SAMPLE_RATE);
    audioSettings.setChannelCount(PC_CHANNEL);
    audioSettings.setBitRate(PC_BIT_RATE);
    //audioSettings.setQuality(QMultimedia::LowQuality);
    //audioSettings.setEncodingMode(QMultimedia::ConstantQualityEncoding);
    audioRecorder->setEncodingSettings(audioSettings);

    //设置textBrowser的进度条在最下方
    //ui->textBrowser_SerialDebug->moveCursor(QTextCursor::End);

    mic_pc_wave_plot.time_waveform = mic_pc_pcm_polt;
    mic_pc_wave_plot.amplitude_waveform = mic_pc_amplitude_polt;
    mic_pc_wave_plot.phase_waveform = mic_pc_phase_polt;

    mic_board_wave_plot.time_waveform = mic_board_pcm_polt;
    mic_board_wave_plot.amplitude_waveform = mic_board_amplitude_polt;
    mic_board_wave_plot.phase_waveform = mic_board_phase_polt;
}

/**
 * @brief MainWindow::~MainWindow   析构函数
 */
MainWindow::~MainWindow()
{
    qDebug() << "ui exit";

}

void MainWindow::ui_init(void)
{   
    //***菜单栏 start***/
    QMenuBar *main_menu_bar = new QMenuBar();
    //在菜单栏中创建一个菜单
    QMenu *app_config = new QMenu("配置");
    QAction *edit_config_file = app_config->addAction("编辑配置文件");
    connect(edit_config_file, SIGNAL(triggered(bool)), this, SLOT(slot_edit_config_file_triggered(bool)));
    QAction *reset_config_file = app_config->addAction("恢复默认配置");
    connect(reset_config_file, SIGNAL(triggered(bool)), this, SLOT(slot_reset_config_file_triggered(bool)));
    main_menu_bar->addMenu(app_config);
    this->setMenuBar(main_menu_bar);
    //***菜单栏 end***/

    //***状态栏 start***/
    QStatusBar *statusbar = new QStatusBar();
    statusbar_label = new QLabel("状态：");
    statusbar->addWidget(statusbar_label);
    this->setStatusBar(statusbar);
    //***状态栏 end***/

    //创建一个顶层的widget
    QWidget *widget = new QWidget();
    this->setCentralWidget(widget);

    //***端口设置 start***/
    QLabel * com_num_label = new QLabel("端口号:");
    com_num_combobox = new QComboBox();
    //初始化串口号下拉列表
    foreach (const QSerialPortInfo &qspinfo, QSerialPortInfo::availablePorts())
    {
       com_num_combobox->addItem(qspinfo.portName());
    }
    QPushButton *refreshcom_button = new QPushButton("刷新端口");
    connect(refreshcom_button, SIGNAL(clicked()), this, SLOT(slots_refreshcom_button_clicked()));
    QLabel * com_baud_label = new QLabel("波特率:");
    //创建波特率输入框，并初始化默认波特率
    cmd_baud_lineedit = new QLineEdit();
    cmd_baud_lineedit->setMaximumWidth(100);
    cmd_baud_lineedit->setText(json_obj->rootObj.value("baud_rate").toString());
    //创建一个指示灯，并初始化为黑色
    led_label = new QLabel();
    led_label->setMinimumSize(20, 20);
    led_label->setMaximumSize(20, 20);
    led_label->setStyleSheet("background-color:black;");
    open_com_button = new QPushButton("打开串口");
    connect(open_com_button, SIGNAL(clicked()), this, SLOT(slots_open_com_button_clicked()));
    QHBoxLayout *com_hboxlayout = new QHBoxLayout();
    com_hboxlayout->addWidget(com_num_label);
    com_hboxlayout->addWidget(com_num_combobox);
    com_hboxlayout->addWidget(refreshcom_button);
    com_hboxlayout->addWidget(com_baud_label);
    com_hboxlayout->addWidget(cmd_baud_lineedit);
    com_hboxlayout->addWidget(led_label);
    com_hboxlayout->addWidget(open_com_button);
    com_hboxlayout->addStretch();
    QGroupBox *com_groupbox = new QGroupBox("端口设置");
    com_groupbox->setLayout(com_hboxlayout);
    //***端口设置 end***/

    //***测试流程 start***/
    check_all_button = new QPushButton("选择全部");
    connect(open_com_button, SIGNAL(clicked()), this, SLOT(slots_check_all_button_clicked()));
    QLabel * reset_num_label = new QLabel("重试次数:");
    reset_num_lineedit = new QLineEdit();
    reset_num_lineedit->setText("3");
    reset_num_lineedit->setMaximumWidth(100);
    QPushButton *start_test_button = new QPushButton("开始测试");
    connect(start_test_button, SIGNAL(clicked()), this, SLOT(slots_start_test_button_clicked()));
    QHBoxLayout *test_hboxlayout = new QHBoxLayout();
    test_hboxlayout->addWidget(check_all_button);
    test_hboxlayout->addWidget(reset_num_label);
    test_hboxlayout->addWidget(reset_num_lineedit);
    test_hboxlayout->addWidget(start_test_button);
    test_hboxlayout->addStretch();
    test_treewidget = new QTreeWidget();
    //将测试流程加载到树中
    init_treewidget(test_treewidget, &json_obj->rootObj);
    QVBoxLayout *test_vboxlayout = new QVBoxLayout();
    test_vboxlayout->addLayout(test_hboxlayout);
    test_vboxlayout->addWidget(test_treewidget);
    QGroupBox *test_groupbox = new QGroupBox("测试流程");
    test_groupbox->setLayout(test_vboxlayout);
    //***测试流程 end***/

    //***音频信号分析 start***/
    QTabWidget *waveform_tabwidget = new QTabWidget();
    //麦克风测试顶层widget
    QWidget *mic_wave_top_widget = new QWidget();
    //麦克风测试顶层网格布局
    QGridLayout *mic_wave_gridlayout = new QGridLayout();

    //电脑麦克风的时域波形
    QWidget *mic_pc_pcm_widget = new QWidget();
    mic_pc_pcm_polt = new QCustomPlot(mic_pc_pcm_widget);
    mic_pc_pcm_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_pc_pcm_title = new QCPTextElement(mic_pc_pcm_polt, "PC音频的时域波形", QFont("sans", 10, QFont::Bold));
    mic_pc_pcm_polt->plotLayout()->addElement(0, 0, mic_pc_pcm_title);
    mic_pc_pcm_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_pc_pcm_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_pc_pcm_polt->xAxis->setLabel("时间(ms)");
    mic_pc_pcm_polt->yAxis->setLabel("音频采样值");
    mic_pc_pcm_polt->xAxis->setRange(0, 10000);
    mic_pc_pcm_polt->yAxis->setRange(-1, 1);
    mic_pc_pcm_polt->addGraph();
    mic_pc_pcm_polt->graph(0)->setPen(QPen(Qt::red));
    mic_pc_pcm_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_pc_pcm_polt, 0, 0);

    //电脑麦克风的频域幅度
    QWidget *mic_pc_amplitude_widget = new QWidget();
    mic_pc_amplitude_polt = new QCustomPlot(mic_pc_amplitude_widget);
    mic_pc_amplitude_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_pc_amplitude_title = new QCPTextElement(mic_pc_amplitude_polt, "PC音频的频域幅度", QFont("sans", 10, QFont::Bold));
    mic_pc_amplitude_polt->plotLayout()->addElement(0, 0, mic_pc_amplitude_title);
    mic_pc_amplitude_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_pc_amplitude_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_pc_amplitude_polt->xAxis->setLabel("频率(Hz)");
    mic_pc_amplitude_polt->yAxis->setLabel("幅度(V)");
    mic_pc_amplitude_polt->addGraph();
    mic_pc_amplitude_polt->graph(0)->setPen(QPen(Qt::red));
    mic_pc_amplitude_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_pc_amplitude_polt, 0, 1);

    //电脑麦克风的频域上的相位
    QWidget *mic_pc_phase_widget = new QWidget();
    mic_pc_phase_polt = new QCustomPlot(mic_pc_phase_widget);
    mic_pc_phase_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_pc_phase_title = new QCPTextElement(mic_pc_phase_polt, "PC音频的频域相位", QFont("sans", 10, QFont::Bold));
    mic_pc_phase_polt->plotLayout()->addElement(0, 0, mic_pc_phase_title);
    mic_pc_phase_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_pc_phase_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_pc_phase_polt->xAxis->setLabel("频率(Hz)");
    mic_pc_phase_polt->yAxis->setLabel("相位(度)");
    mic_pc_phase_polt->addGraph();
    mic_pc_phase_polt->graph(0)->setPen(QPen(Qt::red));
    mic_pc_phase_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_pc_phase_polt, 0, 2);

    //测试板麦克风的时域波形
    QWidget *mic_board_pcm_widget = new QWidget();
    mic_board_pcm_polt = new QCustomPlot(mic_board_pcm_widget);
    mic_board_pcm_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_board_pcm_title = new QCPTextElement(mic_board_pcm_polt, "产品音频的时域波形", QFont("sans", 10, QFont::Bold));
    mic_board_pcm_polt->plotLayout()->addElement(0, 0, mic_board_pcm_title);
    mic_board_pcm_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_board_pcm_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_board_pcm_polt->xAxis->setLabel("时间(ms)");
    mic_board_pcm_polt->yAxis->setLabel("音频采样值");
    mic_board_pcm_polt->xAxis->setRange(0, 10000);
    mic_board_pcm_polt->yAxis->setRange(-1, 1);
    mic_board_pcm_polt->addGraph();
    mic_board_pcm_polt->graph(0)->setPen(QPen(Qt::red));
    mic_board_pcm_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_board_pcm_polt, 1, 0);

    //输出的频域幅度
    QWidget *mic_board_amplitude_widget = new QWidget();
    mic_board_amplitude_polt = new QCustomPlot(mic_board_amplitude_widget);
    mic_board_amplitude_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_board_amplitude_title = new QCPTextElement(mic_board_amplitude_polt, "产品音频的频域幅度", QFont("sans", 10, QFont::Bold));
    mic_board_amplitude_polt->plotLayout()->addElement(0, 0, mic_board_amplitude_title);
    mic_board_amplitude_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_board_amplitude_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_board_amplitude_polt->xAxis->setLabel("频率(Hz)");
    mic_board_amplitude_polt->yAxis->setLabel("幅度(V)");
    mic_board_amplitude_polt->addGraph();
    mic_board_amplitude_polt->graph(0)->setPen(QPen(Qt::red));
    mic_board_amplitude_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_board_amplitude_polt, 1, 1);

    //输出的频域相位
    QWidget *mic_board_phase_widget = new QWidget();
    mic_board_phase_polt = new QCustomPlot(mic_board_phase_widget);
    mic_board_phase_polt->plotLayout()->insertRow(0);
    QCPTextElement *mic_board_phase_title = new QCPTextElement(mic_board_phase_polt, "产品音频的频域相位", QFont("sans", 10, QFont::Bold));
    mic_board_phase_polt->plotLayout()->addElement(0, 0, mic_board_phase_title);
    mic_board_phase_polt->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    mic_board_phase_polt->axisRect()->setRangeZoom(Qt::Horizontal);
    mic_board_phase_polt->xAxis->setLabel("频率(Hz)");
    mic_board_phase_polt->yAxis->setLabel("相位(度)");
    mic_board_phase_polt->addGraph();
    mic_board_phase_polt->graph(0)->setPen(QPen(Qt::red));
    mic_board_phase_polt->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    mic_wave_gridlayout->addWidget(mic_board_phase_polt, 1, 2);

    mic_wave_top_widget->setLayout(mic_wave_gridlayout);
    waveform_tabwidget->addTab(mic_wave_top_widget, "麦克风测试");
    //***音频信号分析 end***/

    //***顶层的网格布局 start***/
    QGridLayout *top_gridlayout = new QGridLayout();
    top_gridlayout->setColumnStretch(0, 1);
    top_gridlayout->setColumnStretch(1, 2);
    top_gridlayout->addWidget(com_groupbox, 0, 0);
    top_gridlayout->addWidget(test_groupbox, 1, 0);
    top_gridlayout->addWidget(waveform_tabwidget, 0, 1, 2, 1);
    widget->setLayout(top_gridlayout);
    //***顶层的网格布局 start***/
}

void MainWindow::check_microphone()
{
    QAudioFormat format;
    // Set up the desired format, for example:
    format.setSampleRate(PC_SAMPLE_RATE);
    format.setChannelCount(PC_CHANNEL);
    format.setSampleSize(PC_BIT_RATE);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
    {
        QMessageBox::about(NULL, "错误", "未检测到电脑麦克风输入或麦克风格式不支持！");
    }
}

/**
 * @brief MainWindow::Init_treeWidget
 * @param jsonObj
 */
void MainWindow::init_treewidget(QTreeWidget *treeWidget, QJsonObject * jsonObj)
{
    QStringList headers;
    headers << "测试项目" << "指令参数" << "测试结果";
    //设置表头
    treeWidget->setHeaderLabels(headers);
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //先清空这个树
    treeWidget->clear();

    //获得test_flow中的测试流程
    QJsonArray test_flow_Array = jsonObj->value("test_flow").toArray();
    //获得cmd_list中的所有指令
    QJsonArray cmd_list_Array = jsonObj->value("cmd_list").toArray();

    for(int i=0; i<test_flow_Array.size(); i++)
    {
        //遍历treeWidget的父节点
        QJsonObject test_Obj = test_flow_Array[i].toObject();
        QTreeWidgetItem *root = new QTreeWidgetItem( treeWidget, QStringList() << test_Obj["测试项目"].toString());
        root->setCheckState(0, Qt::Checked);
        root->setExpanded(true);    //将项目展开

        //在子项的没测试结果上添加label
        QLabel *label = new QLabel(this);
        treeWidget->setItemWidget(root, 2, label);

        QJsonArray test_cmd_Array = test_Obj.value("测试指令列表").toArray();

        //遍历测试流程中的子流程
        for(int j=0; j<test_cmd_Array.size(); j++)
        {
            //根据测试指令列表中的指令，从cmd_list中查找对应的指令
            int index = json_obj->find_cmd_list(cmd_list_Array, test_cmd_Array[j].toString());
            if( index != -1 )
            {
                QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
                QTreeWidgetItem *child = new QTreeWidgetItem(root, QStringList() << cmd_Obj["需求信息"].toString());
                //在子项的没测试结果上添加label
                QLabel *label = new QLabel(this);
                treeWidget->setItemWidget(child, 2, label);

                for(QJsonObject::Iterator it=cmd_Obj.begin(); it!=cmd_Obj.end(); it++)
                {
                    QTreeWidgetItem *child_2 = new QTreeWidgetItem( child, QStringList() << it.key() << it.value().toString() );
                    //child_2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
                    //ui->treeWidget->openPersistentEditor(child_2, 1);
                }
            }
        }
    }

}

/**
 * @brief MainWindow::slots_refreshcom_button_clicked   刷新串口号
 */
void MainWindow::slots_refreshcom_button_clicked()
{
    com_num_combobox->clear();
    foreach (const QSerialPortInfo &qspinfo, QSerialPortInfo::availablePorts())
    {
       com_num_combobox->addItem(qspinfo.portName());
    }
}

/**
 * @brief MainWindow::slots_open_com_button_clicked 打开串口槽函数
 */
void MainWindow::slots_open_com_button_clicked()
{
    if(open_com_button->text() == "打开串口")
    {
        gSerial_Info.my_serialport = new QSerialPort(this);

        //设置串口号
        gSerial_Info.my_serialport->setPortName(com_num_combobox->currentText());
        //以读写方式打开串口
        if(gSerial_Info.my_serialport->open(QIODevice::ReadWrite))
        {
            //设置波特率
            gSerial_Info.my_serialport->setBaudRate(cmd_baud_lineedit->text().toInt());
            //设置数据位
            gSerial_Info.my_serialport->setDataBits(QSerialPort::Data8);
            //设置校验位
            gSerial_Info.my_serialport->setParity(QSerialPort::NoParity);
            //设置流控制
            gSerial_Info.my_serialport->setFlowControl(QSerialPort::NoFlowControl);
            //设置停止位
            gSerial_Info.my_serialport->setStopBits(QSerialPort::OneStop);

            connect(gSerial_Info.my_serialport,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

            open_com_button->setText("关闭串口");
            led_label->setStyleSheet("background-color:green;");
            Serial_status = true;
        }
        else
        {
            QMessageBox::about(NULL, "提示", "串口没有打开！");
            return;
        }
    }
    else
    {
        gSerial_Info.my_serialport->close();
        open_com_button->setText("打开串口");
        led_label->setStyleSheet("background-color:black");
        Serial_status = false;
    }
}

/**
 * @brief MainWindow::receiveInfo   串口接收数据槽
 */
void MainWindow::receiveInfo()
{
    gSerial_Info.serial_data = gSerial_Info.my_serialport->readAll();
    qDebug() << "recv = " << gSerial_Info.serial_data;

    QDateTime time = QDateTime::currentDateTime();      //获取系统现在的时间
    QString time_str = time.toString("hh:mm:ss zzz");   //设置显示格式
    //写入调试数据
    //ui->textBrowser_SerialDebug->insertPlainText(time_str + " 接收：" + gSerial_Info.serial_data + "\r\n");
}

/**
 * @brief MainWindow::slots_check_all_button_clicked 选择全部按钮单击的槽
 */
void MainWindow::slots_check_all_button_clicked()
{
    if(check_all_button->text() == "选择全部")
    {
        int top_num = test_treewidget->topLevelItemCount();

        for(int i=0; i<top_num; i++)
        {
            test_treewidget->topLevelItem(i)->setCheckState(0, Qt::Checked);
        }
        check_all_button->setText("全部取消");
    }
    else
    {
        int top_num = test_treewidget->topLevelItemCount();

        for(int i=0; i<top_num; i++)
        {
            test_treewidget->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
        }
        check_all_button->setText("选择全部");
    }
}

/**
 * @brief MainWindow::slot_edit_config_file_triggered   菜单栏的编辑指令
 */
void MainWindow::slot_edit_config_file_triggered(bool)
{
    bool ok = false;
    QString string = QInputDialog::getText(this, tr("身份验证"),tr("请输入管理员密码："), QLineEdit::Password, "", &ok);
    if(ok && ( string == json_obj->rootObj["passwd"].toString() ))
    {
        ui_EditCmdWindow = new EditCmdWindow(this);
        connect(ui_EditCmdWindow,SIGNAL(signal_window_exit(QJsonObject *)),this,SLOT(signal_window_exit_slots(QJsonObject *)));
        ui_EditCmdWindow->show();
    }
    else
    {
        QMessageBox::about(NULL, "错误", "密码错误！");
    }
}

void MainWindow::signal_window_exit_slots(QJsonObject * jsonobj)
{
    init_treewidget(test_treewidget, jsonobj);
}

/**
 * @brief MainWindow::Get_TestFlow_CmdList  根据测试流程的名称查找测试流程
 * @param flow_name
 * @return
 */
/*
struct flow_struct MainWindow::Get_TestFlow_CmdList(QString flow_name)
{
    struct flow_struct test_flow_struct = NULL;
    int top_num =ui->treeWidget->topLevelItemCount();
    for(int i=0; i<top_num; i++)
    {
        QTreeWidgetItem *top_item =  ui->treeWidget->topLevelItem(i);
        //找到要查找的流程
        if( top_item->text(0) == flow_name )
        {
            //保存流程的名称
            test_flow_struct.flow_name = top_item->text(0);
            //保存流程的重试次数
            test_flow_struct.flow_retry = ui->lineEdit_FlowRetry->text().toInt();
            //保存顶层的label
            test_flow_struct.label = qobject_cast<QLabel *>(ui->treeWidget->itemWidget(top_item, 2));
            test_flow_struct.label->setText(" ");
            test_flow_struct.label->setStyleSheet("background-color:white");
            for(int j=0; j<child_num; j++)
            {
                struct com_struct temp_com_struct;
                QTreeWidgetItem *child_item = top_item->child(j);
                int index = json_obj->find_info_list(cmd_list_Array, child_item->text(0));
                if( index != -1 )
                {
                    temp_com_struct.label = qobject_cast<QLabel *>(ui->treeWidget->itemWidget(child_item, 2));
                    temp_com_struct.label->setText(" ");
                    temp_com_struct.label->setStyleSheet("background-color:white");
                    QJsonObject tem_obj = cmd_list_Array[index].toObject();
                    temp_com_struct.Info = tem_obj.value("需求信息").toString();
                    temp_com_struct.ComStr = tem_obj.value("指令").toString();
                    temp_com_struct.Param = tem_obj.value("指令参数").toString();
                    temp_com_struct.Retry = tem_obj.value("重发次数").toString().toInt();
                    temp_com_struct.TimeOut = tem_obj.value("指令超时").toString().toInt();
                    temp_com_struct.Interval = tem_obj.value("发送间隔").toString().toInt();
                    temp_com_struct.IsSuccess = tem_obj.value("成功判定").toString();
                    temp_com_struct.IsFailure = tem_obj.value("失败判定").toString();

                    test_flow_struct.com_list.enqueue(temp_com_struct);
                }
            }
        }
    }
    return temp_com_struct;
}
*/

/**
 * @brief MainWindow::slots_start_test_button_clicked   开始测试按钮槽
 */
void MainWindow::slots_start_test_button_clicked()
{
    qDebug("run slots_start_test_button_clicked()");
    //得先确认串口和HTPP服务器已经打开
    if(Serial_status != true)
    {
        QMessageBox::about(NULL, "错误", "串口没有打开，不能开始测试！");
        return;
    }
    if(Serial_status != true)
    {
        QMessageBox::about(NULL, "错误", "串口没有打开，不能开始测试！");
        return;
    }

    //获得cmd_list中的所有指令
    QJsonArray cmd_list_Array = json_obj->rootObj.value("cmd_list").toArray();
    //先清空测试流程list
    test_flow_list.clear();
    //先遍历tree，将所有的指令入队列
    int top_num = test_treewidget->topLevelItemCount();
    for(int i=0; i<top_num; i++)
    {
        QTreeWidgetItem *top_item = test_treewidget->topLevelItem(i);
        //得到流程下的指令
        int child_num = top_item->childCount();
        //判断测试流程是否使能
        if( (top_item->checkState(0) == Qt::Checked) && (child_num > 0) )
        {
            struct flow_struct test_flow_struct;
            //保存流程的名称
            test_flow_struct.flow_name = top_item->text(0);
            //保存流程的重试次数
            test_flow_struct.flow_retry = reset_num_lineedit->text().toInt();
            //保存顶层的label
            test_flow_struct.label = qobject_cast<QLabel *>(test_treewidget->itemWidget(top_item, 2));
            test_flow_struct.label->setText(" ");
            test_flow_struct.label->setStyleSheet("background-color:white");
            for(int j=0; j<child_num; j++)
            {
                struct com_struct temp_com_struct;
                QTreeWidgetItem *child_item = top_item->child(j);
                int index = json_obj->find_info_list(cmd_list_Array, child_item->text(0));
                if( index != -1 )
                {
                    temp_com_struct.label = qobject_cast<QLabel *>(test_treewidget->itemWidget(child_item, 2));
                    temp_com_struct.label->setText(" ");
                    temp_com_struct.label->setStyleSheet("background-color:white");
                    QJsonObject tem_obj = cmd_list_Array[index].toObject();
                    temp_com_struct.Info = tem_obj.value("需求信息").toString();
                    temp_com_struct.ComStr = tem_obj.value("指令").toString();
                    temp_com_struct.Param = tem_obj.value("指令参数").toString();
                    temp_com_struct.Retry = tem_obj.value("重发次数").toString().toInt();
                    temp_com_struct.TimeOut = tem_obj.value("指令超时").toString().toInt();
                    temp_com_struct.Interval = tem_obj.value("发送间隔").toString().toInt();
                    temp_com_struct.IsSuccess = tem_obj.value("成功判定").toString();
                    temp_com_struct.IsFailure = tem_obj.value("失败判定").toString();

                    //test_flow_struct.com_list.append(temp_com_struct);
                    test_flow_struct.com_list.enqueue(temp_com_struct);
                }
            }
            test_flow_list.enqueue(test_flow_struct);
        }
    }
    //开启定时器，开始驱动状态机
    fsm_timer->start(1);    //启动或重启定时器, 并设置定时器时间：毫秒
    flow_state = INIT;

    for(int i=0; i<test_flow_list.size(); i++)
    {
        //qDebug() << "flow name = " << test_flow_list[i].flow_name;
        for(int j=0; j<test_flow_list[i].com_list.size(); j++ )
        {
            //qDebug() << "cmd name = " << test_flow_list[i].com_list[j].Info;
        }
    }
}

void MainWindow::clean_flow_label(struct flow_struct * node)
{
    node->label->setText(" ");
    node->label->setStyleSheet("background-color:white");
    int com_num = node->com_list.size();
    qDebug() << "run clean_flow_label com_num = " << com_num;
    for(int i = 0; i < com_num; i++)
    {
        node->com_list[i].label->setText(" ");
        node->com_list[i].label->setStyleSheet("background-color:white");
    }
}

/**
 * @brief MainWindow::slot_fsm_timer_timeout    定时器超时槽
 */
void MainWindow::slot_fsm_timer_timeout()
{
    static struct flow_struct flow_node;
    static struct flow_struct flow_node_back;
    static struct com_struct com_node;
    static int timeout_count = 0;   //超时计数

    //qDebug() << "flow_state = " << flow_state;

    //状态机
    switch(flow_state)
    {
        case INIT:              //00
        {
            flow_node = test_flow_list.dequeue();      //返回第一个流程
            flow_node_back = flow_node;                 //把出队的顶层节点备份
            com_node = flow_node.com_list.dequeue();   //返回流程中的命令
            flow_state = SEND_CMD;                  //将状态切换为读指令状态
        }
        break;
        case READ_TEXT_FLOW:    //01
        {
            //判断是否还要重发
            if( flow_node.flow_retry > 0 )
            {
                qDebug() << "flow_node.flow_retry = " << flow_node.flow_retry;
                flow_node.flow_retry --;     //将重发次数减1
                flow_node.com_list = flow_node_back.com_list;   //重新填充指令
                clean_flow_label(&flow_node);       //将当前流程的所有label清空
                com_node = flow_node.com_list.dequeue();   //返回流程中的命令

                qDebug() << "flow_node.flow_name = " << flow_node.flow_name;
                flow_state = SEND_CMD;                  //将状态切换为读指令状态
            }
            else
            {
                //如果队列不为空
                if( test_flow_list.isEmpty() != true )
                {
                    flow_node = test_flow_list.dequeue();   //返回下一个流程
                    flow_node_back = flow_node;             //把出队的顶层节点备份
                    if( flow_node.com_list.isEmpty() != true )
                    {
                        com_node = flow_node.com_list.dequeue();   //返回流程中的命令
                        flow_state = SEND_CMD;       //将状态置为发送状态
                    }
                }
                else
                {
                    //如果下一个元素为空，表示所有的测试流程都结束了
                    flow_state = FLOW_END;
                }
            }
        }
        break;
        case READ_TEXT_CMD:     //02
        {
            //
            if( flow_node.com_list.isEmpty() != true )
            {
                com_node = flow_node.com_list.dequeue();   //返回流程中的命令
                flow_state = SEND_CMD;       //将状态置为发送状态
            }
            else
            {
                //如果下一个元素为空，表示这个测试流程成功结束了
                timeout_count = 0;
                flow_state = FLOW_SUCCEED;
            }
        }
        break;
        case SEND_CMD:          //03
        {
            //发送指令，发送指令前将接收清除
            gSerial_Info.serial_data.clear();
            QString str = com_node.ComStr + " " + com_node.Param + "\r\n";
            QByteArray ba = str.toLatin1();
            qDebug() << "send str = " << ba;
            const char *c_str = ba.data();
            int ret = gSerial_Info.my_serialport->write(c_str);
            if( ret != -1 )
            {
                QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                QString time_str = time.toString("hh:mm:ss zzz"); //设置显示格式
                //写入调试数据
                //ui->textBrowser_SerialDebug->insertPlainText(time_str + " 发送：" + str);

                //在状态栏显示当前发送的指令
                statusbar_label->setText(flow_node.flow_name + "：" + com_node.Info);
            }
            timeout_count = 0;
            flow_state = WAIT_ACK;   //状态置为发送状态
        }
        break;
        case WAIT_ACK:          //04
        {
            if( gSerial_Info.serial_data.length() != 0 )
            {
                //判断是不是成功
                if( gSerial_Info.serial_data.indexOf(com_node.IsSuccess) != -1 )
                {
                    //如果成功了，更新测试结果，进入成功状态
                    com_node.label->setText("成功");
                    com_node.label->setStyleSheet("background-color:green");
                    timeout_count = 0;
                    flow_state = SUCCEED;
                }
                else if( gSerial_Info.serial_data.indexOf(com_node.IsFailure) != -1 )
                {
                    qDebug() << "recv IsFailure";
                    //如果失败了，转为失败状态
                    com_node.label->setText("失败");
                    com_node.label->setStyleSheet("background-color:red");
                    flow_state = FAILURE;
                }
                else
                {
                    timeout_count ++;
                }
            }
            else
            {
                timeout_count ++;
            }
            //判断等待下位机响应是否超时
            if( timeout_count >= com_node.TimeOut )
            {
                //如果超时了，转为超状态
                flow_state = TIMEOUT;
            }
        }
        break;
        case WAIT_INTERVAL:     //05
        {
            timeout_count ++;
            //如果间隔时间到了，发送下一个指令
            if( timeout_count >= com_node.Interval )
            {
                flow_state = READ_TEXT_CMD;
            }
        }
        break;
        case TIMEOUT:           //06
        case FAILURE:           //07
        {
            //判断当前指令是否还要重发
            if( com_node.Retry >= 0 )
            {
                com_node.Retry --;     //将重发次数减1
                flow_state =SEND_CMD;
            }
            else
            {
                //子项测试失败了
                com_node.label->setText("失败");
                com_node.label->setStyleSheet("background-color:red");
                //这个指令发送失败了，这个流程也失败了
                flow_state = FLOW_FAILURE;
            }
        }
        break;
        case SUCCEED:           //08
        {
            flow_state = WAIT_INTERVAL;
            //发送一个信号，表示当前指令执行成功
            emit signal_cmd_execute_successfully(com_node);
        }
        break;
        case FLOW_FAILURE:      //09
        {
            //如果这个流程失败了，更新流程的显示，进行下一个流程
            flow_node.label->setText("测试失败");
            flow_node.label->setStyleSheet("background-color:red");
            flow_state = READ_TEXT_FLOW;
        }
        break;
        case FLOW_SUCCEED:      //10
        {
            timeout_count ++;
            //如果间隔时间到了，对测试结果进行分析
            if( timeout_count >= 1000 )
            {
                flow_state = RESULT_ANALYSIS;
            }
        }
        break;
        case RESULT_ANALYSIS:   //11
        {
            if( flow_node.flow_name == "麦克风测试" )
            {
                //显示录音文件的波形，并分析是否通过测试
                QString pc_fileName = QCoreApplication::applicationDirPath() + "/pc_mic_record-48KHz-16bit-Stereo.pcm";
                QString board_fileName = QCoreApplication::applicationDirPath() + "/record-16KHz-16bit-Stereo.pcm";
                fft_analyze(pc_fileName, board_fileName, &mic_pc_wave_plot, &mic_board_wave_plot);
            }

            if( flow_node.flow_name == "扬声器测试" )
            {
                //显示录音文件的波形，并分析是否通过测试

            }

            //如果这个流程成功了，更新流程的显示，进行下一个流程
            flow_node.label->setText("测试成功");
            flow_node.label->setStyleSheet("background-color:green");
            flow_node.flow_retry = 0;   //成功就不需要重试了
            flow_state = READ_TEXT_FLOW;
        }
        break;
        case FLOW_END:          //12
        {
            //所有的流程都已经走完了
            statusbar_label->setText("测试完成！");
            QMessageBox::about(NULL, "提示", "测试完成！");
            fsm_timer->stop();
        }
        break;
        default:break;
    }

}

/**
 * @brief wav2pcm   将wav格式转为pcm格式
 * @param in_file
 * @param out_file
 * @return
 */
int wav2pcm(char *in_file, char *out_file)
{
    size_t result;
    char  *buf;
    FILE *fp1=fopen(in_file, "rb");//wav文件打开，打开读权限
    FILE *fp2=fopen(out_file, "wb");//pcm文件创建，给予写权限
    fseek(fp1,0,SEEK_END);//文件指针从0挪到尾部
    long filesize;
    filesize=ftell(fp1);//ftell求文件指针相对于0的便宜字节数，就求出了文件字节数

    if(fp1==NULL||fp2==NULL)//判断两个文件是否打开
    {
        qDebug() << "file open filed!!" << endl;
        return -1;
    }

    rewind(fp1);//还原指针位置
    fseek(fp1,44,SEEK_SET);//wav文件的指针从头向后移动44字节
    buf=(char *)malloc(sizeof(char)*filesize);//开辟空间给缓存数组

    if(buf==NULL)
    {
        qDebug("memory  error");
        return -1;
    }

    result =fread(buf,1,(filesize-44),fp1);//每次读一个字节到buf，同时求读的次数
    if(result != (filesize-44))//判断读的次数和文件大小是否一致
    {
        qDebug() << "reing error!!" << endl;
        return -1;
    }
    fwrite(buf,1,(filesize-44),fp2);//写到pcm文件中
    fclose(fp1);//关闭文件指针
    fclose(fp2);
    free (buf);//释放buf
    return 0 ;
}

/**
 * @brief MainWindow::wav_to_pcm    将wav格式转为pcm格式
 * @param in_file
 * @param out_file
 * @return
 */
bool MainWindow::wav_to_pcm(QString in_file, QString out_file)
{
    QFileInfo fileInfo(in_file);
    if(!fileInfo.isFile())
    {
        QMessageBox::critical(NULL, "错误", "\"" + in_file + "\"" + "文件不存在！", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    char * in_ch;
    QByteArray in_ba = in_file.toLatin1();
    in_ch = in_ba.data();

    char * out_ch;
    QByteArray out_ba = out_file.toLatin1();
    out_ch = out_ba.data();

    int ret = wav2pcm(in_ch, out_ch);
    if( ret != 0 )
    {
        QMessageBox::critical(NULL, "错误", "转换pcm失败！", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }
    return true;
}

/**
 * @brief MainWindow::slot_cmd_execute_successfully 指令执行成功槽
 */
void MainWindow::slot_cmd_execute_successfully(struct com_struct com)
{
    if( com.ComStr == "factory 5001" )
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/pc_mic_record-48KHz-16bit-Stereo.wav";
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
        audioRecorder->record();    //开始录音
    }

    if( com.ComStr == "factory 5002" )
    {
        audioRecorder->stop();      //停止录音
        //转格式
        QString in_fileName = QCoreApplication::applicationDirPath() + "/pc_mic_record-48KHz-16bit-Stereo.wav";
        QString out_fileName = QCoreApplication::applicationDirPath() + "/pc_mic_record-48KHz-16bit-Stereo.pcm";
        int ret = wav_to_pcm(in_fileName, out_fileName);
    }

    if( com.ComStr == "factory 5003" )
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/pc_spk_record-48KHz-16bit-Stereo.wav";
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
        audioRecorder->record();    //开始录音
    }

    if( com.ComStr == "factory 5004" )
    {
        audioRecorder->stop();      //停止录音
        //转格式
        QString in_fileName = QCoreApplication::applicationDirPath() + "/pc_spk_record-48KHz-16bit-Stereo.wav";
        QString out_fileName = QCoreApplication::applicationDirPath() + "/pc_spk_record-48KHz-16bit-Stereo.pcm";
        int ret = wav_to_pcm(in_fileName, out_fileName);
    }
}
/**
 * @brief MainWindow::draw_file_waveform    绘制音频文件波形
 * @param filename
 * @param plot
 * @return
 */
bool MainWindow::draw_file_waveform(QString filename, struct wave_plot *plot,
                                    int sample_rate, int bit_rate, int channel)
{
    int data_interval = ((bit_rate/8)*channel);

    //***读文件 start***/
    QByteArray ba = filename.toLatin1();
    char *filename_ch = ba.data();
    FILE *fp=fopen(filename_ch, "rb");   //打开读权限
    fseek(fp,0,SEEK_END);      //文件指针从0挪到尾部
    long filesize=ftell(fp);   //ftell求文件指针相对于0的便宜字节数，就求出了文件字节数

    if(fp==NULL)//判断两个文件是否打开
    {
        QMessageBox::critical(NULL, "错误", "\"" + filename + "\"" + "文件打开失败！", QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    rewind(fp);//还原指针位置
    char *buf=(char *)malloc(filesize);//开辟空间给缓存数组
    if(buf==NULL)
    {
        QMessageBox::critical(NULL, "错误", "\"" + filename + "\"" + "内存分配失败！", QMessageBox::Yes, QMessageBox::Yes);
        return -1;
    }

    long result =fread(buf, 1, filesize, fp);//每次读一个字节到buf，同时求读的次数
    if(result != filesize)//判断读的次数和文件大小是否一致
    {
        QMessageBox::critical(NULL, "错误", "\"" + filename + "\"" + "文件读取失败！", QMessageBox::Yes, QMessageBox::Yes);
        return -1;
    }
    //***读文件 end***/

    //***显示时域波形 start***/
    for( int i=0; i<filesize/data_interval; i++ )
    {
        double x = ((double)1000/sample_rate)*i;
        short y = *((short *)(buf+(i*data_interval)));
        plot->time_waveform->graph(0)->addData( x, (((double)y)/32768) );
    }
    plot->time_waveform->replot();
    //***显示时域波形 end***/

    //***显示频域幅度波形 start***/
    int fft_N = filesize/data_interval;
    double * in = (double*)fftw_malloc(sizeof(double) * fft_N);
    for(int i=0; i<fft_N; i++)
    {
        short y = *((short *)(buf+(i*data_interval)));
        in[i] = y;  //将pcm文件中的数据复制到fft的输入
    }

    fftw_complex * out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fft_N);

    fftw_plan p = FFTW3_H::fftw_plan_dft_r2c_1d(fft_N, in, out, FFTW_ESTIMATE);
    fftw_execute(p);

    double dx3 = (double)sample_rate / fft_N;

    plot->amplitude_waveform->xAxis->setRange(0, sample_rate/2, Qt::AlignLeft);

    double val_max = 0;
    //根据FFT计算的复数计算振幅谱
    for( int i=0; i<fft_N/2; i++ )
    {
        double val = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        val = val / (fft_N / 2);
        plot->amplitude_waveform->graph(0)->addData( dx3 * i, val );

        if( val > val_max )
        {
            val_max = val;
        }

        double db = log(val);
        //qDebug("frequency = %f, amplitude = %f, db = %f", dx3 * i, val / (N / 2), db);
    }

    plot->amplitude_waveform->yAxis->setRange(val_max*0.6, val_max*1.2, Qt::AlignBottom);
    plot->amplitude_waveform->replot();
    //***显示频域幅度波形 end***/

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    fclose(fp);//关闭文件指针
    free (buf);//释放buf
}

/**
 * @brief MainWindow::fft_analyze   FFT分析
 * @param pc_audio_file
 * @param board_audio_file
 * @param pc_plot
 * @param board_plot
 * @return
 */
bool MainWindow::fft_analyze(QString pc_audio_file, QString board_audio_file,
                             struct wave_plot *pc_plot, struct wave_plot *board_plot)
{
    draw_file_waveform(pc_audio_file, pc_plot, PC_SAMPLE_RATE, PC_BIT_RATE, PC_CHANNEL);
    draw_file_waveform(board_audio_file, board_plot, BOARD_SAMPLE_RATE, BOARD_BIT_RATE, BOARD_CHANNEL);

}

/**
 * @brief MainWindow::slot_reset_config_file_triggered 恢复出厂设置菜单
 */
void MainWindow::slot_reset_config_file_triggered(bool)
{
    QString source_name = QCoreApplication::applicationDirPath() + "/config_reset.json";
    QString target_name = QCoreApplication::applicationDirPath() + "/config.json";

    QFile source_file(source_name);
    QFile target_file(target_name);

    //判断一下备份文件是否存在
    if( source_file.exists() != true )
    {
        QMessageBox::about(NULL, "错误", "默认配置文件不存在！");
        return;
    }

    //判断一下目标文件是否存在
    if( target_file.exists() == true )
    {
        //存在的话把文件删除
        if( target_file.remove() == true )
        {
            //如果成功删除了目标文件，把源文件拷贝成目标文件
            if( source_file.copy(target_name) == true )
            {
                QMessageBox::about(NULL, "提示", "恢复默认设置成功，软件将退出，请重新启动本软件！");
                qApp->quit();
            }
            else
            {
                QMessageBox::about(NULL, "错误", "恢复默认设置失败，请重试！");
            }
        }
        else
        {
            QMessageBox::about(NULL, "错误", "配置文件正在使用，恢复默认设置失败！");
        }
    }
    else
    {
        //不存在的话真接拷贝一份
        //如果成功删除了目标文件，把源文件拷贝成目标文件
        if( source_file.copy(target_name) == true )
        {
            QMessageBox::about(NULL, "提示", "恢复默认设置成功，软件将退出，请重新启动本软件！");
            qApp->quit();
        }
        else
        {
            QMessageBox::about(NULL, "错误", "恢复默认设置失败，请重试！");
        }
    }
}

/**
 * @brief MainWindow::signal_python_state_slots
 */
void MainWindow::signal_python_state_slots(e_python_state state)
{
    switch(state)
    {
        case INIT_ERROR:
            QMessageBox::about(NULL, "错误", "HTTP服务器初始化错误！");
            break;
        case IMPORT_ERROR:
            QMessageBox::about(NULL, "错误", "HTTP服务器模块导入错误！");
            break;
        case GETFUN_ERROR:
            QMessageBox::about(NULL, "错误", "HTTP服务器获取方法错误！");
            break;
        case CALL_ERROR:
            QMessageBox::about(NULL, "错误", "HTTP服务器调用错误！");
            break;
        case RUN_NORMAL:
            ;
            break;
        default:break;
    }

}
