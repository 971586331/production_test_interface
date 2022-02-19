#include "editcmdwindow.h"
#include "mainwindow.h"
#include "main.h"
#include <QDebug>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>
#include <QPoint>
#include <QSignalMapper>
#include <QGridLayout>
#include <QtWidgets>

EditCmdWindow::EditCmdWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //QRegExp regExp("^[A-Za-z0-9]+$");
    //ui->lineEdit_CmdString->setValidator(new QRegExpValidator(regExp, this));

    //将配置文件对象备份
    rootObj_backup = json_obj->rootObj;

    ui_init();
}

EditCmdWindow::~EditCmdWindow()
{

}

void EditCmdWindow::ui_init(void)
{
    this->setMinimumSize(600, 400);

    //创建一个顶层的widget
    QWidget *widget = new QWidget();
    this->setCentralWidget(widget);

    //***查看指令页面 start***/
    viewcmd_comboBox = new QComboBox();
    connect(viewcmd_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slots_viewcmd_comboBox_currentIndexChanged(int)));
    QLabel *cmdinfo_label = new QLabel("指令名称：");
    cmdinfo_label_view = new QLabel("");
    QLabel *cmd_label = new QLabel("指令：");
    cmd_label_view = new QLabel("");
    QLabel *param_label = new QLabel("指令参数：");
    param_label_view = new QLabel("");
    QLabel *retry_label = new QLabel("重发次数：");
    retry_label_view = new QLabel("");
    QLabel *timeout_label = new QLabel("指令超时：");
    timeout_label_view = new QLabel("");
    QLabel *interval_label = new QLabel("发送间隔：");
    interval_label_view = new QLabel("");
    QLabel *issuccess_label = new QLabel("成功判定：");
    issuccess_label_view = new QLabel("");
    QLabel *isfailure_label = new QLabel("失败判定：");
    isfailure_label_view = new QLabel("");
    QPushButton *deletecmd_button = new QPushButton("删除当前指令");
    connect(deletecmd_button, SIGNAL(clicked()), this, SLOT(slots_deletecmd_button_clicked()));
    QPushButton *deleteall_button = new QPushButton("删除所有指令");
    connect(deleteall_button, SIGNAL(clicked()), this, SLOT(slots_deleteall_button_clicked()));
    QGridLayout *viewcmd_gridlayout = new QGridLayout();
    viewcmd_gridlayout->setColumnStretch(0, 1);
    viewcmd_gridlayout->setColumnStretch(1, 2);
    viewcmd_gridlayout->addWidget(viewcmd_comboBox,     0, 0);
    viewcmd_gridlayout->addWidget(cmdinfo_label,        1, 0);
    viewcmd_gridlayout->addWidget(cmdinfo_label_view,   1, 1);
    viewcmd_gridlayout->addWidget(cmd_label,            2, 0);
    viewcmd_gridlayout->addWidget(cmd_label_view,       2, 1);
    viewcmd_gridlayout->addWidget(param_label,          3, 0);
    viewcmd_gridlayout->addWidget(param_label_view,     3, 1);
    viewcmd_gridlayout->addWidget(retry_label,          4, 0);
    viewcmd_gridlayout->addWidget(retry_label_view,     4, 1);
    viewcmd_gridlayout->addWidget(timeout_label,        5, 0);
    viewcmd_gridlayout->addWidget(timeout_label_view,   5, 1);
    viewcmd_gridlayout->addWidget(interval_label,       6, 0);
    viewcmd_gridlayout->addWidget(interval_label_view,  6, 1);
    viewcmd_gridlayout->addWidget(issuccess_label,      7, 0);
    viewcmd_gridlayout->addWidget(issuccess_label_view, 7, 1);
    viewcmd_gridlayout->addWidget(isfailure_label,      8, 0);
    viewcmd_gridlayout->addWidget(isfailure_label_view, 8, 1);
    viewcmd_gridlayout->addWidget(deletecmd_button,     9, 0);
    viewcmd_gridlayout->addWidget(deleteall_button,     10, 0);
    viewcmd_gridlayout->setSpacing(10);
    viewcmd_gridlayout->setContentsMargins(10,10,10,10);
    //创建一个顶层的widget
    QWidget *viewcmd_top_widget = new QWidget();
    viewcmd_top_widget->setLayout(viewcmd_gridlayout);
    init_viewcmd_page();
    //***查看指令页面 end***/

    //***添加指令页面 end***/
    addcmd_comboBox = new QComboBox();
    connect(addcmd_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slots_addcmd_comboBox_currentIndexChanged(int)));
    QLabel *cmdinfo_label_2 = new QLabel("指令名称：");
    cmdinfo_linedit = new QLineEdit();
    QLabel *cmd_label_2 = new QLabel("指令：");
    cmd_linedit = new QLineEdit();
    QLabel *param_label_2 = new QLabel("指令参数：");
    param_linedit = new QLineEdit();
    QLabel *retry_label_2 = new QLabel("重发次数：");
    retry_linedit = new QLineEdit();
    QLabel *timeout_label_2 = new QLabel("指令超时：");
    timeout_linedit = new QLineEdit();
    QLabel *interval_label_2 = new QLabel("发送间隔：");
    interval_linedit = new QLineEdit();
    QLabel *issuccess_label_2 = new QLabel("成功判定：");
    issuccess_linedit = new QLineEdit();
    QLabel *isfailure_label_2 = new QLabel("失败判定：");
    isfailure_linedit = new QLineEdit();
    QPushButton *addcmd_button = new QPushButton("添加或修改指令");
    connect(addcmd_button, SIGNAL(clicked()), this, SLOT(slots_addcmd_button_clicked()));
    QGridLayout *addcmd_gridlayout = new QGridLayout();
    addcmd_gridlayout->setColumnStretch(0, 1);
    addcmd_gridlayout->setColumnStretch(1, 2);
    addcmd_gridlayout->addWidget(addcmd_comboBox,   0, 0);
    addcmd_gridlayout->addWidget(cmdinfo_label_2,   1, 0);
    addcmd_gridlayout->addWidget(cmdinfo_linedit,   1, 1);
    addcmd_gridlayout->addWidget(cmd_label_2,       2, 0);
    addcmd_gridlayout->addWidget(cmd_linedit,       2, 1);
    addcmd_gridlayout->addWidget(param_label_2,     3, 0);
    addcmd_gridlayout->addWidget(param_linedit,     3, 1);
    addcmd_gridlayout->addWidget(retry_label_2,     4, 0);
    addcmd_gridlayout->addWidget(retry_linedit,     4, 1);
    addcmd_gridlayout->addWidget(timeout_label_2,   5, 0);
    addcmd_gridlayout->addWidget(timeout_linedit,   5, 1);
    addcmd_gridlayout->addWidget(interval_label_2,  6, 0);
    addcmd_gridlayout->addWidget(interval_linedit,  6, 1);
    addcmd_gridlayout->addWidget(issuccess_label_2, 7, 0);
    addcmd_gridlayout->addWidget(issuccess_linedit, 7, 1);
    addcmd_gridlayout->addWidget(isfailure_label_2, 8, 0);
    addcmd_gridlayout->addWidget(isfailure_linedit, 8, 1);
    addcmd_gridlayout->addWidget(addcmd_button,     9, 0);
    addcmd_gridlayout->setSpacing(10);
    addcmd_gridlayout->setContentsMargins(10,10,10,10);
    //创建一个顶层的widget
    QWidget *addcmd_top_widget = new QWidget();
    addcmd_top_widget->setLayout(addcmd_gridlayout);
    init_editcmd_page();
    //***添加指令页面 end***/

    //***编辑测试流程 start***/
    edit_treewidget = new QTreeWidget();
    init_editflow_page();
    //***编辑测试流程 end***/

    //***构建tab start***/
    QTabWidget *editcmd_tabwidget = new QTabWidget(this);
    editcmd_tabwidget->addTab(viewcmd_top_widget, "查看指令");
    editcmd_tabwidget->addTab(addcmd_top_widget, "添加指令");
    editcmd_tabwidget->addTab(edit_treewidget, "编辑测试流程");
    //***构建tab end***/

    //***保存配置按键 start***/
    QPushButton *saveandexit_button = new QPushButton("保存后退出");
    connect(saveandexit_button, SIGNAL(clicked()), this, SLOT(slots_saveandexit_button_clicked()));
    saveandexit_button->setMinimumSize(100, 30);
    saveandexit_button->setMaximumSize(100, 30);
    QPushButton *exit_button = new QPushButton("不保存退出");
    connect(exit_button, SIGNAL(clicked()), this, SLOT(slots_exit_button_clicked()));
    exit_button->setMinimumSize(100, 30);
    exit_button->setMaximumSize(100, 30);
    //***保存配置按键 end***/

    //***顶层布局 start***/
    QVBoxLayout *hLayout_1 = new QVBoxLayout();
    hLayout_1->addWidget(editcmd_tabwidget);
    hLayout_1->addWidget(saveandexit_button, Qt::AlignHCenter);
    hLayout_1->addWidget(exit_button, Qt::AlignHCenter);
    hLayout_1->setSpacing(10);
    hLayout_1->setContentsMargins(10,10,10,10);
    //***顶层布局 end***/

    widget->setLayout(hLayout_1);
}

/**
 * @brief EditCmdWindow::comboBox_add_CmdList   根据mcd_list填充comboBox
 * @param widget
 * @param root_obj
 */
void EditCmdWindow::comboBox_add_CmdList(QComboBox * widget, QJsonObject root_obj)
{
    //获得所有的指令
    QJsonArray cmd_list_Array = root_obj.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    //填充comboBox_CmdList
    for(int i=0; i<cmd_list_Array.size(); i++)
    {
        QJsonObject temp_obj = cmd_list_Array[i].toObject();
        widget->addItem(temp_obj["指令"].toString());
    }
}

/**
 * @brief EditCmdWindow::Init_tab_ViewCmd   初始化查看指令视图
 */
void EditCmdWindow::init_viewcmd_page()
{
    //填充之前先清空
    viewcmd_comboBox->clear();

    //获得所有的指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    comboBox_add_CmdList(viewcmd_comboBox, rootObj_backup);

    //根据comboBox_CmdList的当前项目初始化文本显示
    //获取当前的comboBox
    QString current_cmd = viewcmd_comboBox->currentText();
    int index = json_obj->find_cmd_list(cmd_list_Array, current_cmd);
    if( index != -1 )
    {
        QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
        cmdinfo_label_view->setText(cmd_Obj.value("需求信息").toString());
        cmd_label_view->setText(cmd_Obj.value("指令").toString());
        param_label_view->setText(cmd_Obj.value("指令参数").toString());
        retry_label_view->setText(cmd_Obj.value("重发次数").toString());
        timeout_label_view->setText(cmd_Obj.value("指令超时").toString());
        interval_label_view->setText(cmd_Obj.value("发送间隔").toString());
        issuccess_label_view->setText(cmd_Obj.value("成功判定").toString());
        isfailure_label_view->setText(cmd_Obj.value("失败判定").toString());
    }
    else
    {
        cmdinfo_label_view->setText("");
        cmd_label_view->setText("");
        param_label_view->setText("");
        retry_label_view->setText("");
        timeout_label_view->setText("");
        interval_label_view->setText("");
        issuccess_label_view->setText("");
        isfailure_label_view->setText("");
    }
}

/**
 * @brief EditCmdWindow::Init_tab_EditCmd   初始化编辑指令页面
 */
void EditCmdWindow::init_editcmd_page()
{
    //填充之前先清空
    addcmd_comboBox->clear();

    //获得所有的指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    comboBox_add_CmdList(addcmd_comboBox, rootObj_backup);

    //根据comboBox_CmdList的当前项目初始化文本显示
    //获取当前的comboBox
    QString current_cmd = addcmd_comboBox->currentText();
    int index = json_obj->find_cmd_list(cmd_list_Array, current_cmd);
    if( index != -1 )
    {
        QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
        cmdinfo_linedit->setText(cmd_Obj.value("需求信息").toString());
        cmd_linedit->setText(cmd_Obj.value("指令").toString());
        param_linedit->setText(cmd_Obj.value("指令参数").toString());
        retry_linedit->setText(cmd_Obj.value("重发次数").toString());
        timeout_linedit->setText(cmd_Obj.value("指令超时").toString());
        interval_linedit->setText(cmd_Obj.value("发送间隔").toString());
        issuccess_linedit->setText(cmd_Obj.value("成功判定").toString());
        isfailure_linedit->setText(cmd_Obj.value("失败判定").toString());
    }
    else
    {
        cmdinfo_linedit->setText("");
        cmd_linedit->setText("");
        param_linedit->setText("");
        retry_linedit->setText("");
        timeout_linedit->setText("");
        interval_linedit->setText("");
        issuccess_linedit->setText("");
        isfailure_linedit->setText("");
    }
}

/**
 * @brief EditCmdWindow::Init_tab_EditFlow  初始化编辑测试流程页面
 */
void EditCmdWindow::init_editflow_page()
{
    edit_treewidget->clear();

    //设置tree的表格头
    QStringList headers;
    headers << "测试项目" << "指令";

    edit_treewidget->setHeaderLabels(headers);
    edit_treewidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //获得test_flow中的测试流程
    QJsonArray test_flow_Array = rootObj_backup.value("test_flow").toArray();
    //获得cmd_list中的所有指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();

    signalMapper_ComboBox = new QSignalMapper(this);
    //
    for(int i=0; i<test_flow_Array.size(); i++)
    {
        //遍历treeWidget的父节点
        QJsonObject test_Obj = test_flow_Array[i].toObject();
        QTreeWidgetItem *root = new QTreeWidgetItem( edit_treewidget, QStringList() << test_Obj["测试项目"].toString());
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

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
                //添加的每一个子项都是ComboBox
                QComboBox *comb = new QComboBox(this);      //新建一个ComboBox
                comboBox_add_CmdList(comb, rootObj_backup); //将所有的指令放到ComboBox中
                comb->setCurrentText(cmd_Obj["指令"].toString()); //根据指令显示当前的项
                edit_treewidget->setItemWidget(child,1,comb);   //将ComboBox放到tree中
                connect(comb, SIGNAL(currentIndexChanged(int)), signalMapper_ComboBox, SLOT(map()));
                signalMapper_ComboBox->setMapping(comb, comb);
                //connect(comb,SIGNAL(currentIndexChanged(int)),this,SLOT(slots_EditFlow_comb_changed(int)));
            }
        }
    }
    connect(signalMapper_ComboBox, SIGNAL(mapped(QWidget *)),this, SLOT(slots_EditFlow_comb_changed(QWidget *)));

    edit_treewidget->setContextMenuPolicy(Qt::CustomContextMenu);//右键 不可少否则右键无反应
    connect(edit_treewidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slots_showrightMenu(QPoint)));

    popMenu = new QMenu(this);
    add_parent_node = new QAction(QStringLiteral("添加测试项目"),this);
    add_parent_node->setStatusTip(QStringLiteral("添加测试项目"));
    add_child_node = new QAction(QStringLiteral("添加指令"),this);
    add_child_node->setStatusTip(QStringLiteral("添加指令"));
    delete_node = new QAction(QStringLiteral("删除项目"),this);
    delete_node->setStatusTip(QStringLiteral("删除项目"));

    connect(add_parent_node,SIGNAL(triggered()),this,SLOT(slots_add_parent_node_Item()));
    connect(add_child_node,SIGNAL(triggered()),this,SLOT(slots_add_child_node_Item()));
    connect(delete_node,SIGNAL(triggered()),this,SLOT(slots_delete_node_Item()));
}

/**
 * @brief EditCmdWindow::slots_showrightMenu    在列表上右键显示菜单
 * @param point
 */
void EditCmdWindow::slots_showrightMenu(QPoint point)
{
    popMenu->clear();//清除原有菜单
    popMenu->addAction(add_parent_node);
    popMenu->addAction(add_child_node);
    popMenu->addAction(delete_node);
    //QTreeWidgetItem *item = tree->itemAt(point); //可得到右键条目
    popMenu->exec(QCursor::pos());
}

/**
 * @brief EditCmdWindow::slots_add_node_Item    添加父节点按键
 */
void EditCmdWindow::slots_add_parent_node_Item()
{
    QTreeWidgetItem* curItem= edit_treewidget->currentItem();//获得当前节点
    if(NULL== curItem)//没有选择节点
    {
        QTreeWidgetItem *root = new QTreeWidgetItem(edit_treewidget);
        root->setText(0, "新增流程");
        root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    }
    else
    {
        QTreeWidgetItem* parent=curItem->parent();//获得当前节点的父节点
        //如果当前节点的父节点是空，那么当前节点就是父节点
        if(NULL==parent)
        {
            //QTreeWidgetItem *root = new QTreeWidgetItem( ui->treeWidget_EditFlow, QStringList() << "新增流程");
            QTreeWidgetItem *root = new QTreeWidgetItem(edit_treewidget, curItem);
            root->setText(0, "新增流程");
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        }
    }
}

/**
 * @brief EditCmdWindow::slots_add_child_node_Item  添加子节点
 */
void EditCmdWindow::slots_add_child_node_Item()
{
    QTreeWidgetItem* curItem= edit_treewidget->currentItem();//获得当前节点
    if(NULL== curItem)//没有选择节点
        return;
    qDebug() << "当前节点：" << curItem->text(0);

    QTreeWidgetItem* parent=curItem->parent();//获得当前节点的父节点
    //如果当前节点的父节点是空，那么当前节点就是父节点
    if(NULL==parent)
    {
        qDebug() << "parent";
        QTreeWidgetItem *child = new QTreeWidgetItem(curItem, QStringList() << "新增指令1");

        QComboBox *comb = new QComboBox(this);      //新建一个ComboBox
        edit_treewidget->setItemWidget(child, 1, comb);   //将ComboBox放到tree中
        signalMapper_ComboBox->setMapping(comb, comb);
        comboBox_add_CmdList(comb, rootObj_backup); //将所有的指令放到ComboBox中
        connect(comb, SIGNAL(currentIndexChanged(int)), signalMapper_ComboBox, SLOT(map()));


        //根据子节点，更新测试项目
        QString str = comb->currentText();
        //获得test_flow中的测试流程
        QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
        int index = json_obj->find_cmd_list(cmd_list_Array, str);
        if( index != -1 )
        {
            QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
            child->setText(0, cmd_Obj["需求信息"].toString());
        }
    }
    else    //如果光标在子节点上，在当前子节点后添加
    {
        qDebug() << "child";
        QTreeWidgetItem *child = new QTreeWidgetItem(parent, curItem);
        child->setText(0, "新增指令2");

        QComboBox *comb = new QComboBox(this);      //新建一个ComboBox
        edit_treewidget->setItemWidget(child, 1, comb);   //将ComboBox放到tree中
        signalMapper_ComboBox->setMapping(comb, comb);
        comboBox_add_CmdList(comb, rootObj_backup); //将所有的指令放到ComboBox中
        connect(comb, SIGNAL(currentIndexChanged(int)), signalMapper_ComboBox, SLOT(map()));


        //根据子节点，更新测试项目
        QString str = comb->currentText();
        //获得test_flow中的测试流程
        QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
        int index = json_obj->find_cmd_list(cmd_list_Array, str);
        if( index != -1 )
        {
            QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
            child->setText(0, cmd_Obj["需求信息"].toString());
        }

    }
}

/**
 * @brief EditCmdWindow::slots_delete_node_Item 删除节点
 */
void EditCmdWindow::slots_delete_node_Item()
{
    QTreeWidgetItem* curItem= edit_treewidget->currentItem();  //获取当前被点击的节点
        if(curItem==NULL)return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击

    qDebug() << "当前节点：" << curItem->text(0);

    QTreeWidgetItem* parent=curItem->parent();//获得当前节点的父节点
    //如果当前节点的父节点是空，那么当前节点就是父节点
    if(NULL == parent)
    {
        //先通过curItem找到curItem的index,再通过takeTopLevelItem删除这一项
        edit_treewidget->takeTopLevelItem(edit_treewidget->indexOfTopLevelItem(curItem));
    }
    else    //如果光标在子节点上
    {
        //如果是在子项上右键删除，则找到父项后用removeChild删除
        parent->removeChild(curItem);
    }
}

/**
 * @brief EditCmdWindow::slots_EditFlow_comb_changed    当编辑测试流程中的comboBox的索引改变时
 */
void EditCmdWindow::slots_EditFlow_comb_changed(QWidget *widget)
{
    //获得所有的指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    //将widget转为QComboBox
    QComboBox *box = qobject_cast<QComboBox*>(widget);
    qDebug() << "box = " << box->currentText();

    //获得QComboBox的坐标
    QPoint pt= box->pos();
    qDebug() << "x:" << pt.x() << "y:" << pt.y();
    //根据QComboBox的坐标判断控所在的行
    QTreeWidgetItem *item = edit_treewidget->itemAt(pt);
    if(item != NULL)
    {
       edit_treewidget->setCurrentItem(item);
        qDebug() << "slots_EditFlow_comb_changed 当前节点：" << item->text(0);

        int index = json_obj->find_cmd_list(cmd_list_Array, box->currentText());
        if( index != -1 )
        {
            QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
            item->setText(0, cmd_Obj.value("需求信息").toString());
        }
    }
}

/**
 * @brief EditCmdWindow::treeWidget_to_json 将树中的项目转化为json
 * @param treewidget    要转换的树控件
 * @return
 */
QJsonArray EditCmdWindow::treeWidget_to_json(QTreeWidget * treewidget)
{
    QJsonArray json_array;

    int top_num = treewidget->topLevelItemCount();
    qDebug() << "top = " << top_num;

    for(int i=0; i<top_num; i++)
    {
        QTreeWidgetItem *top_item = treewidget->topLevelItem(i);

        QJsonObject jsonObject;
        jsonObject.insert("测试项目", top_item->text(0));

        QJsonArray child_array;
        int child_num = top_item->childCount();
        qDebug() << "child_num = " << child_num;
        for(int j=0; j<child_num; j++)
        {
            QTreeWidgetItem *child_item = top_item->child(j);
            QWidget *widget = treewidget->itemWidget(child_item, 1);
            QComboBox *box = qobject_cast<QComboBox*>(widget);
            QString str = box->currentText();
            child_array.append(str);
        }
        jsonObject.insert("测试指令列表", child_array);

        json_array.append(jsonObject);
    }
    return json_array;
}

/**
 * @brief EditCmdWindow::slots_deletecmd_button_clicked   删除当前指令
 */
void EditCmdWindow::slots_deletecmd_button_clicked()
{

    QPushButton *okbtn = new QPushButton("确认删除");
    QPushButton *cancelbtn = new QPushButton("取消");
    QMessageBox *mymsgbox = new QMessageBox;

    mymsgbox->setIcon(QMessageBox::Warning);
    mymsgbox->setWindowTitle("提示");
    mymsgbox->setText("是否删除当前指令？");
    mymsgbox->addButton(okbtn, QMessageBox::AcceptRole);
    mymsgbox->addButton(cancelbtn, QMessageBox::RejectRole);
    mymsgbox->show();

    mymsgbox->exec();//阻塞等待用户输入
    if (mymsgbox->clickedButton()==okbtn)//点击了OK按钮
    {
        //从rootObj中删除一条指令
        QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
        qDebug() << "len1 = " << rootObj_backup.value("cmd_list").toArray().size();
        for(int i=0; i<cmd_list_Array.size(); i++)
        {
            QJsonObject temp_obj = cmd_list_Array[i].toObject();
            if( temp_obj["指令"] == viewcmd_comboBox->currentText() )
            {
                qDebug() << "删除完成！";
                cmd_list_Array.removeAt(i);
                rootObj_backup.insert("cmd_list", cmd_list_Array);
            }
        }
        qDebug() << "len2 = " << rootObj_backup.value("cmd_list").toArray().size();
    }
    //更新一下显示
    init_viewcmd_page();
    //
    init_editcmd_page();
    //
    init_editflow_page();
}

/**
 * @brief EditCmdWindow::slots_deleteall_button_clicked    删除所有指令槽
 */
void EditCmdWindow::slots_deleteall_button_clicked()
{
    QPushButton *okbtn = new QPushButton("确认删除");
    QPushButton *cancelbtn = new QPushButton("取消");
    QMessageBox *mymsgbox = new QMessageBox;

    mymsgbox->setIcon(QMessageBox::Warning);
    mymsgbox->setWindowTitle("提示");
    mymsgbox->setText("是否删除全部指令？");
    mymsgbox->addButton(okbtn, QMessageBox::AcceptRole);
    mymsgbox->addButton(cancelbtn, QMessageBox::RejectRole);
    mymsgbox->show();

    mymsgbox->exec();//阻塞等待用户输入
    if (mymsgbox->clickedButton()==okbtn)//点击了OK按钮
    {
        //从rootObj中删除一条指令
        QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
        qDebug() << "len1 = " << rootObj_backup.value("cmd_list").toArray().size();

        int com_list_len = cmd_list_Array.size();
        for(int i=0; i<com_list_len; i++)
        {
            cmd_list_Array.removeAt(0);
        }
        rootObj_backup.insert("cmd_list", cmd_list_Array);
        qDebug() << "len2 = " << rootObj_backup.value("cmd_list").toArray().size();
    }
    //更新一下显示
    init_viewcmd_page();
    //
    init_editcmd_page();
    //
    init_editflow_page();
}

/**
 * @brief EditCmdWindow::slots_viewcmd_comboBox_currentIndexChanged    查看指令中的comboBox改变时
 * @param index
 */
void EditCmdWindow::slots_viewcmd_comboBox_currentIndexChanged(int i)
{
    //获得所有的指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    //根据comboBox_CmdList的当前项目初始化文本显示
    //获取当前的comboBox
    QString current_cmd = viewcmd_comboBox->currentText();
    int index = json_obj->find_cmd_list(cmd_list_Array, current_cmd);
    if( index != -1 )
    {
        QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
        cmdinfo_label_view->setText(cmd_Obj.value("需求信息").toString());
        cmd_label_view->setText(cmd_Obj.value("指令").toString());
        param_label_view->setText(cmd_Obj.value("指令参数").toString());
        retry_label_view->setText(cmd_Obj.value("重发次数").toString());
        timeout_label_view->setText(cmd_Obj.value("指令超时").toString());
        interval_label_view->setText(cmd_Obj.value("发送间隔").toString());
        issuccess_label_view->setText(cmd_Obj.value("成功判定").toString());
        isfailure_label_view->setText(cmd_Obj.value("失败判定").toString());
    }
    else
    {
        cmdinfo_label_view->setText("");
        cmd_label_view->setText("");
        param_label_view->setText("");
        retry_label_view->setText("");
        timeout_label_view->setText("");
        interval_label_view->setText("");
        issuccess_label_view->setText("");
        isfailure_label_view->setText("");
    }
}

/**
 * @brief EditCmdWindow::slots_addcmd_button_clicked   添加指令槽
 */
void EditCmdWindow::slots_addcmd_button_clicked()
{
    //先将cmd_list取出
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();

    //先判断一下要添加的指令在不在指令列表中
    int index_cmd = json_obj->find_cmd_list(cmd_list_Array, cmd_linedit->text());
    if( index_cmd != -1 )
    {
        //如果指令已经存在，先删除
        cmd_list_Array.removeAt(index_cmd);
        qDebug() << "指令已经存在，删除指令！";
    }

    //先判断一下要添加的需求信息在不在指令列表中
    int index_info = json_obj->find_info_list(cmd_list_Array, cmdinfo_linedit->text());
    if( index_info != -1 )
    {
        //如果指令已经存在，先删除
        cmd_list_Array.removeAt(index_info);
        qDebug() << "需求信息已经存在，删除指令！";
    }

    // 判断输入是否合法
    // "需求信息"不能为空
    if( cmdinfo_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"需求信息\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    // "指令"不能为空，且必须为数字字母或空格
    if( cmd_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"指令\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QRegExp regExp("^[ A-Za-z0-9]+$");
        bool ret = regExp.exactMatch(cmd_linedit->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"指令\"必须是数字，字母或空格!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    // "指令参数"不能为空，且必须为ascii字符
    if( param_linedit->text() == "" )
    {

    }
    else
    {
        /*
        QRegExp regExp("[\x00-\x7f]");
        bool ret = regExp.exactMatch(ui->lineEdit_Param->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"指令参数\"中不能出现非ascii字符!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        */
    }
    // "重发次数"不能为空，且必须为数字
    if( retry_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"重发次数\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QRegExp regExp("^[0-9]+$");
        bool ret = regExp.exactMatch(retry_linedit->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"重发次数\"必须是数字!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    // "指令超时"不能为空，且必须为数字
    if( timeout_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"指令超时\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QRegExp regExp("^[0-9]+$");
        bool ret = regExp.exactMatch(timeout_linedit->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"重发次数\"必须是数字!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    // "发送间隔"不能为空，且必须为数字
    if( interval_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"重发间隔\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        QRegExp regExp("^[0-9]+$");
        bool ret = regExp.exactMatch(interval_linedit->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"重发间隔\"必须是数字!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
    }
    // "成功判定"不能为空，且必须为ascii字符
    if( issuccess_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"重发间隔\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        /*
        QRegExp regExp("/[\x00-\xff]+/g");
        bool ret = regExp.exactMatch(ui->lineEdit_IsSuccess->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"重发间隔\"不能是非ascii字符!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        */
    }
    // "失败判定"不能为空，且必须为ascii字符
    if( isfailure_linedit->text() == "" )
    {
        QMessageBox::information(NULL, "警告", "\"失败判定\"不能为空!", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    else
    {
        /*
        QRegExp regExp("/[\x00-\xff]+/g");
        bool ret = regExp.exactMatch(ui->lineEdit_IsFailure->text());
        if( ret == false )
        {
            QMessageBox::information(NULL, "警告", "\"失败判定\"不能是非ascii字符!", QMessageBox::Yes, QMessageBox::Yes);
            return;
        }
        */
    }

    qDebug() << "cmdinfo_linedit->text() = " << cmdinfo_linedit->text();

    QJsonObject new_cmd;
    new_cmd.insert("需求信息", cmdinfo_linedit->text());
    new_cmd.insert("指令",    cmd_linedit->text());
    new_cmd.insert("指令参数", param_linedit->text());
    new_cmd.insert("重发次数", retry_linedit->text());
    new_cmd.insert("指令超时", timeout_linedit->text());
    new_cmd.insert("发送间隔", interval_linedit->text());
    new_cmd.insert("成功判定", issuccess_linedit->text());
    new_cmd.insert("失败判定", isfailure_linedit->text());

    //将指令添加到json对象中
    cmd_list_Array.append(new_cmd);
    qDebug() << "cmd_list_Array len = " << cmd_list_Array.size();
    rootObj_backup.insert("cmd_list", cmd_list_Array);
    QMessageBox::information(NULL, "提示", "添加指令成功！", QMessageBox::Yes, QMessageBox::Yes);

    //将当前的指令名称保存一下
    QString current_cmd = cmd_linedit->text();

    //更新一下显示
    init_viewcmd_page();
    //
    init_editcmd_page();
    //
    init_editflow_page();

    //将combobox定位到刚才添加的指令上，也就是最后一个指令
    addcmd_comboBox->setCurrentIndex(addcmd_comboBox->findText(current_cmd));

}

/**
 * @brief EditCmdWindow::slots_saveandexit_button_clicked      保存所有槽
 */
void EditCmdWindow::slots_saveandexit_button_clicked()
{
    //根据”编辑测试流程“更新rootObj_backup
    QJsonArray test_flow_array = treeWidget_to_json(edit_treewidget);
    rootObj_backup.insert("test_flow", test_flow_array);

    //用备份的主节点更新主节点
    json_obj->rootObj = rootObj_backup;
    //将配置写入配置文件
    if(!json_obj->file->open(QIODevice::WriteOnly))
    {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }

    QJsonDocument jdc(QJsonDocument::fromJson(json_obj->file->readAll()));
    QJsonObject obj = jdc.object();
    obj = json_obj->rootObj;
    jdc.setObject(obj);
    //将文件指针定义到文件开头
    json_obj->file->seek(0);
    //写之前要清空文件
    json_obj->file->write(jdc.toJson());
    json_obj->file->flush();
    json_obj->file->close();

    this->close();

    //发信号给mainwindow，更新表格
    emit signal_window_exit(&json_obj->rootObj);
}

/**
 * @brief EditCmdWindow::slots_exit_button_clicked 不保存退出槽
 */
void EditCmdWindow::slots_exit_button_clicked()
{
    this->close();
}

/**
 * @brief EditCmdWindow::slots_addcmd_comboBox_currentIndexChanged  添加或修改指令耐压的combobox索引改变时
 * @param i
 */
void EditCmdWindow::slots_addcmd_comboBox_currentIndexChanged(int i)
{
    //获得所有的指令
    QJsonArray cmd_list_Array = rootObj_backup.value("cmd_list").toArray();
    if( cmd_list_Array.isEmpty() == true )
    {
        return;
    }

    //根据comboBox_CmdList的当前项目初始化文本显示
    //获取当前的comboBox
    QString current_cmd = addcmd_comboBox->currentText();
    int index = json_obj->find_cmd_list(cmd_list_Array, current_cmd);
    if( index != -1 )
    {
        QJsonObject cmd_Obj = cmd_list_Array[index].toObject();
        cmdinfo_linedit->setText(cmd_Obj.value("需求信息").toString());
        cmd_linedit->setText(cmd_Obj.value("指令").toString());
        param_linedit->setText(cmd_Obj.value("指令参数").toString());
        retry_linedit->setText(cmd_Obj.value("重发次数").toString());
        timeout_linedit->setText(cmd_Obj.value("指令超时").toString());
        interval_linedit->setText(cmd_Obj.value("发送间隔").toString());
        issuccess_linedit->setText(cmd_Obj.value("成功判定").toString());
        isfailure_linedit->setText(cmd_Obj.value("失败判定").toString());
    }
    else
    {
        cmdinfo_linedit->setText("");
        cmd_linedit->setText("");
        param_linedit->setText("");
        retry_linedit->setText("");
        timeout_linedit->setText("");
        interval_linedit->setText("");
        issuccess_linedit->setText("");
        isfailure_linedit->setText("");
    }
}
