#ifndef EDITCMDWINDOW_H
#define EDITCMDWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QComboBox>
#include <QSignalMapper>
#include <QTreeWidget>
#include <QtWidgets>

class EditCmdWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditCmdWindow(QWidget *parent = 0);
    ~EditCmdWindow();

    void ui_init(void);
    void init_viewcmd_page();
    void init_editcmd_page();
    void init_editflow_page();
    void comboBox_add_CmdList(QComboBox * widget, QJsonObject root_obj);
    QJsonObject rootObj_backup;            //配置文件的主节点

signals:
    void signal_window_exit(QJsonObject *);

private slots:
    void slots_deletecmd_button_clicked();
    void slots_deleteall_button_clicked();
    void slots_addcmd_button_clicked();
    void slots_saveandexit_button_clicked();
    void slots_exit_button_clicked();
    void slots_viewcmd_comboBox_currentIndexChanged(int index);
    void slots_addcmd_comboBox_currentIndexChanged(int index);

    void slots_showrightMenu(QPoint point);
    void slots_add_parent_node_Item();
    void slots_add_child_node_Item();
    void slots_delete_node_Item();
    void slots_EditFlow_comb_changed(QWidget *);
    QJsonArray treeWidget_to_json(QTreeWidget * treewidget);

private:
    QMenu *popMenu;
    QAction *delete_node;
    QAction *add_parent_node;
    QAction *add_child_node;
    QSignalMapper *signalMapper_ComboBox;

    QComboBox *viewcmd_comboBox;
    QLabel *cmdinfo_label_view;
    QLabel *cmd_label_view;
    QLabel *param_label_view;
    QLabel *retry_label_view;
    QLabel *timeout_label_view;
    QLabel *interval_label_view;
    QLabel *issuccess_label_view;
    QLabel *isfailure_label_view;

    QComboBox *addcmd_comboBox;
    QLineEdit *cmdinfo_linedit;
    QLineEdit *cmd_linedit;
    QLineEdit *param_linedit;
    QLineEdit *retry_linedit;
    QLineEdit *timeout_linedit;
    QLineEdit *interval_linedit;
    QLineEdit *issuccess_linedit;
    QLineEdit *isfailure_linedit;

    QTreeWidget *edit_treewidget;
};

#endif // EDITCMDWINDOW_H
