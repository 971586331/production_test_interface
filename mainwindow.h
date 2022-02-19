#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QLabel>
#include <QQueue>
#include <QPushButton>
#include <QTextEdit>
#include <QtWidgets>
#include <QAudioRecorder>
#include <qcustomplot.h>

#include "editcmdwindow.h"
#include "mythread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    //串口信息
    struct Serial_Info
    {
        QSerialPort *my_serialport;  //串口对象
        QString serial_data;
    };

    //指令的结构
    struct com_struct
    {
        QString Info;       //指令信息
        QString ComStr;     //指令字符串
        QString Param;      //指令参数
        int Retry;          //重发次数
        int TimeOut;        //指令超时
        int Interval;       //发送间隔
        QString IsSuccess;  //成功判定
        QString IsFailure;  //失败判定
        QLabel *label;      //显示测试结果
    };

    //流程的结构
    struct flow_struct
    {
        QLabel *label;      //顶层的label，显示测试结果
        QString flow_name;  //流程的名称
        int flow_retry;     //流程的重试
        QQueue<struct com_struct> com_list;  //指令列表
    };

    //状态机
    enum fsm
    {
        INIT,               //初始状态
        READ_TEXT_FLOW,     //读测试流程
        READ_TEXT_CMD,      //读测试指令
        SEND_CMD,           //发送测试指令
        WAIT_ACK,           //等待响应
        WAIT_INTERVAL,      //等待间隔
        TIMEOUT,            //超时
        FAILURE,            //失败
        SUCCEED,            //成功
        FLOW_FAILURE,       //一个流程以失败结束
        FLOW_SUCCEED,       //一个流程以成功结束
        RESULT_ANALYSIS,    //对结果进行分析
        FLOW_END,           //流程全部结束
    };

    //波形控件
    struct wave_plot
    {
        QCustomPlot *time_waveform;
        QCustomPlot *amplitude_waveform;
        QCustomPlot *phase_waveform;
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void ui_init(void);
    void read_josn(void);
    void init_treewidget(QTreeWidget *treeWidget, QJsonObject * jsonObj);
    void clean_flow_label(struct flow_struct * node);
    bool wav_to_pcm(QString in_file, QString out_file);
    void check_microphone();
    bool draw_file_waveform(QString filename, struct wave_plot *plot,
                            int sample_rate, int bit_rate, int channel);
    bool fft_analyze(QString pc_audio_file, QString board_audio_file,
                    struct wave_plot *pc_plot, struct wave_plot *board_plot);

    QQueue<struct flow_struct> test_flow_list;  //保存所有的测试流程
    enum fsm flow_state = INIT;                 //用于执行指令的状态机
    struct Serial_Info gSerial_Info;            //串口的信息
    bool Serial_status = false;

    struct wave_plot mic_pc_wave_plot;
    struct wave_plot mic_board_wave_plot;

//    struct wave_plot mic_pc_wave_plot =
//    {
//        .time_waveform = mic_pc_pcm_polt,
//        .amplitude_waveform = mic_pc_amplitude_polt,
//        .phase_waveform = mic_pc_phase_polt,
//    };
//    struct wave_plot mic_board_wave_plot =
//    {
//        .time_waveform = mic_board_pcm_polt,
//        .amplitude_waveform = mic_board_amplitude_polt,
//        .phase_waveform = mic_board_phase_polt,
//    };

signals:
    void signal_cmd_execute_successfully(struct com_struct);

private slots:
    void slots_open_com_button_clicked();
    void receiveInfo();
    void slots_check_all_button_clicked();
    void slot_edit_config_file_triggered(bool);
    void signal_window_exit_slots(QJsonObject * jsonObj);
    void slots_start_test_button_clicked();
    void slot_fsm_timer_timeout();
    void slot_reset_config_file_triggered(bool);
    void slots_refreshcom_button_clicked();
    void signal_python_state_slots(e_python_state state);
    void slot_cmd_execute_successfully(struct com_struct com);

private:
    EditCmdWindow *ui_EditCmdWindow;
    QTimer * fsm_timer;
    MyThread *thread1;

    QLabel *led_label;
    QPushButton * open_com_button;
    QComboBox *com_num_combobox;
    QLineEdit *cmd_baud_lineedit;
    QPushButton * check_all_button;
    QTreeWidget *test_treewidget;
    QLineEdit *reset_num_lineedit;
    QLabel * statusbar_label;

    QAudioRecorder *audioRecorder;

    QCustomPlot *mic_pc_pcm_polt;
    QCustomPlot *mic_pc_amplitude_polt;
    QCustomPlot *mic_pc_phase_polt;
    QCustomPlot *mic_board_pcm_polt;
    QCustomPlot *mic_board_amplitude_polt;
    QCustomPlot *mic_board_phase_polt;
};

#endif // MAINWINDOW_H
