#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QTimer>
#include "User.h"
#include "TaskManager.h"
#include "SpeechRecognizer.h"
#include "Reminder.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    User userMgr;
    TaskManager* taskMgr;
    SpeechRecognizer speechRecog;
    Reminder* reminderThread;
    QTimer* refreshTimer;
    bool isLogin;

    QLineEdit* editUser;
    QLineEdit* editPwd;
    QPushButton* btnLogin;
    QPushButton* btnRegister; 
    QTableWidget* tableTask;
    QPushButton* btnAdd;
    QPushButton* btnDel;
    QPushButton* btnVoice;

    QLineEdit* edName;
    QLineEdit* edStart;
    QLineEdit* edRemind;
    QComboBox* cbPri;
    QComboBox* cbCat;

    void initUI();
    void refreshTable();

private slots:
    void slotLogin();
    void slotRegister();
    void slotOpenAddDialog();
    void slotConfirmAdd();
    void slotDeleteTask();
    void slotVoiceInput();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif