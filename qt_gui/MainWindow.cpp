#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      taskMgr(nullptr),
      reminderThread(nullptr),
      refreshTimer(nullptr),
      isLogin(false)
{
    QString exePath = QApplication::applicationFilePath();
    QFileInfo exeInfo(exePath);
    QDir binDir = exeInfo.dir();
    binDir.cdUp();
    QDir::setCurrent(binDir.absolutePath());

    setWindowTitle("MySchedule 日程管理系统");
    resize(1100, 700);

    initUI();

    setStyleSheet(

    /* 主窗口 */
    "QMainWindow{"
    "background:#202124;"
    "}"


    /* 分组框 */
    "QGroupBox{"
    "color:#dddddd;"
    "font-size:16px;"
    "border:1px solid #555;"
    "border-radius:8px;"
    "margin-top:10px;"
    "padding:10px;"
    "}"


    /* 普通文字 */
    "QLabel{"
    "color:#dddddd;"
    "font-size:14px;"
    "}"


    /* 输入框 */
    "QLineEdit{"
    "background:#303134;"
    "color:#000000;"
    "padding:6px;"
    "border-radius:6px;"
    "selection-background-color:#4285f4;"
    "}"


    /* 下拉框 */
    "QComboBox{"
    "background:#303134;"
    "color:#000000;"
    "padding:6px;"
    "border-radius:6px;"
    "}"


    /* 下拉菜单展开 */
    "QComboBox QAbstractItemView{"
    "background:#ffffff;"
    "color:#000000;"
    "selection-background-color:#4285f4;"
    "}"


    /* 按钮 */
    "QPushButton{"
    "background:#4285f4;"
    "color:white;"
    "border-radius:8px;"
    "padding:8px;"
    "}"


    "QPushButton:hover{"
    "background:#3367d6;"
    "}"


    /* 表格 */
    "QTableWidget{"
    "background:#303134;"
    "color:white;"
    "gridline-color:#666;"
    "}"


    /* 表头 */
    "QHeaderView::section{"
    "background:#4285f4;"
    "color:white;"
    "padding:5px;"
    "}"


    /* 弹窗 */
    "QDialog{"
    "background:#ffffff;"
    "}"


    "QDialog QLabel{"
    "color:#000000;"
    "}"


    "QDialog QLineEdit{"
    "background:#eeeeee;"
    "color:#000000;"
    "}"


    "QDialog QComboBox{"
    "background:#eeeeee;"
    "color:#000000;"
    "}"

);
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(1500);
    connect(refreshTimer,&QTimer::timeout,this,&MainWindow::refreshTable);
    refreshTimer->start();
}


MainWindow::~MainWindow()
{
    if(reminderThread)
    {
        reminderThread->stop();
        delete reminderThread;
    }

    delete taskMgr;
}


void MainWindow::initUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);


    // 左侧控制区域
    QVBoxLayout *left = new QVBoxLayout();


    QGroupBox *loginBox = new QGroupBox("用户登录");

    QVBoxLayout *loginLayout = new QVBoxLayout(loginBox);

    editUser = new QLineEdit();
    editUser->setPlaceholderText("用户名");

    editPwd = new QLineEdit();
    editPwd->setPlaceholderText("密码");
    editPwd->setEchoMode(QLineEdit::Password);

    btnLogin = new QPushButton("登录");
    btnRegister = new QPushButton("注册");

    loginLayout->addWidget(editUser);
    loginLayout->addWidget(editPwd);
    loginLayout->addWidget(btnLogin);
    loginLayout->addWidget(btnRegister);


    connect(btnLogin,&QPushButton::clicked,
            this,&MainWindow::slotLogin);
    connect(btnRegister,
        &QPushButton::clicked,
        this,
        &MainWindow::slotRegister);



    QGroupBox *operationBox = new QGroupBox("任务操作");

    QVBoxLayout *operationLayout = new QVBoxLayout(operationBox);


    btnAdd = new QPushButton("新增任务");
    btnDel = new QPushButton("删除任务");
    btnVoice = new QPushButton("语音录入");


    operationLayout->addWidget(btnAdd);
    operationLayout->addWidget(btnDel);
    operationLayout->addWidget(btnVoice);


    connect(btnAdd,&QPushButton::clicked,
            this,&MainWindow::slotOpenAddDialog);

    connect(btnDel,&QPushButton::clicked,
            this,&MainWindow::slotDeleteTask);

    connect(btnVoice,&QPushButton::clicked,
            this,&MainWindow::slotVoiceInput);



    left->addWidget(loginBox);
    left->addWidget(operationBox);
    left->addStretch();



    // 右侧任务表
    tableTask = new QTableWidget();

    QStringList header={
        "ID",
        "任务名",
        "开始时间",
        "提醒时间",
        "优先级",
        "分类",
        "已提醒"
    };

    tableTask->setColumnCount(header.size());
    tableTask->setHorizontalHeaderLabels(header);

    tableTask->horizontalHeader()->setStretchLastSection(true);
    tableTask->setEditTriggers(QAbstractItemView::NoEditTriggers);


    root->addLayout(left,1);
    root->addWidget(tableTask,3);
}


void MainWindow::refreshTable()
{
    if(!isLogin)
    {
        tableTask->setRowCount(0);
        return;
    }

    TaskManager freshTask("data/tasks.txt");
    freshTask.loadFromFile();

    std::vector<Task> list=freshTask.getTasks();

    tableTask->clearContents();
    tableTask->setRowCount(list.size());


    for(int i=0;i<list.size();i++)
    {
        const Task &t=list[i];

        tableTask->setItem(i,0,new QTableWidgetItem(QString::number(t.id)));
        tableTask->setItem(i,1,new QTableWidgetItem(QString::fromStdString(t.name)));
        tableTask->setItem(i,2,new QTableWidgetItem(QString::fromStdString(t.startTime)));
        tableTask->setItem(i,3,new QTableWidgetItem(QString::fromStdString(t.remindTime)));
        tableTask->setItem(i,4,new QTableWidgetItem(QString::fromStdString(t.priority)));
        tableTask->setItem(i,5,new QTableWidgetItem(QString::fromStdString(t.category)));
        tableTask->setItem(i,6,new QTableWidgetItem(t.reminded?"是":"否"));
    }
}
void MainWindow::slotLogin()
{
    QString user = editUser->text();
    QString pwd = editPwd->text();
    if (user.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(this, "提示", "用户名和密码不能为空");
        return;
    }
    bool loginOk = userMgr.authenticate(user.toStdString(), pwd.toStdString());
    if (!loginOk)
    {
        QMessageBox::critical(this, "登录失败", "用户名或密码错误");
        return;
    }
    if (taskMgr) delete taskMgr;
    taskMgr = new TaskManager("data/tasks.txt");
    taskMgr->loadFromFile();

    isLogin = true;
    refreshTable();
    QMessageBox::information(this, "登录成功", "欢迎使用日程管理");

    // 销毁旧提醒实例
    if (reminderThread != nullptr)
    {
        reminderThread->stop();
        delete reminderThread;
        reminderThread = nullptr;
    }

    // 启动提醒线程，直接使用当前任务管理器
	reminderThread = new Reminder(*taskMgr);
	reminderThread->start();

	qDebug() << "【系统】后台提醒线程启动完成";

}

void MainWindow::slotOpenAddDialog()
{
    if (!isLogin)
    {
        QMessageBox::warning(this, "提示", "请先登录！");
        return;
    }
    QDialog dlg(this);
dlg.setWindowTitle("新增任务");
dlg.resize(420,340);


// 新增：修复弹窗文字颜色
dlg.setStyleSheet(
    "QDialog {"
    "background-color:white;"
    "color:black;"
    "}"

    "QLabel {"
    "color:black;"
    "font-size:14px;"
    "}"

    "QLineEdit {"
    "background-color:white;"
    "color:black;"
    "border:1px solid #999;"
    "padding:6px;"
    "}"

    "QComboBox {"
    "background-color:white;"
    "color:black;"
    "border:1px solid #999;"
    "padding:6px;"
    "}"

    "QPushButton {"
    "background-color:#4285f4;"
    "color:white;"
    "border-radius:6px;"
    "padding:8px;"
    "}"
);


QVBoxLayout* dlgLayout = new QVBoxLayout(&dlg);

    dlgLayout->addWidget(new QLabel("任务名称"));
    edName = new QLineEdit();
    dlgLayout->addWidget(edName);

    dlgLayout->addWidget(new QLabel("开始时间(YYYY-MM-DD HH:MM)"));
    edStart = new QLineEdit("2026-07-26 19:00");
    dlgLayout->addWidget(edStart);

    dlgLayout->addWidget(new QLabel("提醒时间"));
    edRemind = new QLineEdit("2026-07-26 18:30");
    dlgLayout->addWidget(edRemind);

    dlgLayout->addWidget(new QLabel("优先级"));
    cbPri = new QComboBox();
    cbPri->addItems({"高", "中", "低"});
    cbPri->setCurrentText("中");
    dlgLayout->addWidget(cbPri);

    dlgLayout->addWidget(new QLabel("分类"));
    cbCat = new QComboBox();
    cbCat->addItems({"学习", "娱乐", "生活"});
    cbCat->setCurrentText("生活");
    dlgLayout->addWidget(cbCat);

    QPushButton* confirm = new QPushButton("确认添加");
    connect(confirm, &QPushButton::clicked, this, &MainWindow::slotConfirmAdd);
    dlgLayout->addWidget(confirm);
    dlg.exec();
}

void MainWindow::slotConfirmAdd()
{
    std::string name = edName->text().toStdString();
    std::string start = edStart->text().toStdString();
    std::string remind = edRemind->text().toStdString();
    std::string pri = cbPri->currentText().toStdString();
    std::string cat = cbCat->currentText().toStdString();

    if (name.empty() || start.empty())
    {
        QMessageBox::warning(this, "提示", "任务名称、开始时间不能为空");
        return;
    }
    bool ok = taskMgr->addTask(name, start, remind, pri, cat);
    if (ok)
    {
        QMessageBox::information(this, "成功", "任务添加完成");
        refreshTable();
    }
    else
    {
        QMessageBox::critical(this, "失败", "任务名称+开始时间重复");
    }
}

void MainWindow::slotDeleteTask()
{
    if (!isLogin)
    {
        QMessageBox::warning(this, "提示", "请先登录！");
        return;
    }
    auto items = tableTask->selectedItems();
    if (items.empty())
    {
        QMessageBox::warning(this, "提示", "请选中一行任务");
        return;
    }
    int delId = items[0]->text().toInt();
    bool ok = taskMgr->deleteTask(delId);
    if (ok)
    {
        QMessageBox::information(this, "成功", QString("任务%1已删除").arg(delId));
        refreshTable();
    }
    else
    {
        QMessageBox::critical(this, "失败", "删除任务失败");
    }
}

void MainWindow::slotVoiceInput()
{
    if (!isLogin)
    {
        QMessageBox::warning(this, "提示", "请先登录！");
        return;
    }


    QMessageBox::information(
        this,
        "语音录入",
        "请开始说话\n\n"
        "示例：\n"
        "明天下午3点提醒我学习C++\n\n"
        "录音时间约10秒"
    );


    // 开始录音+识别
    std::string text = speechRecog.recordAndRecognize();


    if(text.empty())
    {
        QMessageBox::warning(
            this,
            "识别失败",
            "没有检测到有效语音"
        );
        return;
    }


    QMessageBox::information(
        this,
        "识别结果",
        QString("识别内容：\n\n%1")
        .arg(QString::fromStdString(text))
    );


    bool ok = speechRecog.voiceToTask(
        *taskMgr,
        text
    );


    if(ok)
    {
        QMessageBox::information(
            this,
            "成功",
            "语音任务创建完成"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this,
            "失败",
            "语音解析失败或任务重复"
        );
    }
}
void MainWindow::slotRegister()
{
    QString user = editUser->text();
    QString pwd = editPwd->text();


    if(user.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(
            this,
            "提示",
            "用户名和密码不能为空"
        );
        return;
    }



    bool ok = userMgr.registerUser(
        user.toStdString(),
        pwd.toStdString()
    );



    if(ok)
    {
        QMessageBox::information(
            this,
            "注册成功",
            "用户创建成功，请登录"
        );
    }
    else
    {
        QMessageBox::warning(
            this,
            "注册失败",
            "用户名已经存在"
        );
    }
}