#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <ui_mainwindow.h>
#include <string>

namespace Ui {
class MainWindow;
}

class Traceing : public QThread {
    Q_OBJECT

public:
    std::string path ;
    Ui_MainWindow * mainwindow ;
    bool runFlag = true ;
    Traceing() ;
    Traceing( std::string p ) ;
    void UpdateTextBrowser( std::string ss ) ;
signals:
    void sendText( std::string ss ) ;
private:
    void run() ;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setText( QString target ) ;
    QString configPath ;
    Traceing * tracker = NULL ;
    bool isTraceing = false ;
    ~MainWindow();

private slots:
    void on_action_triggered();
    void recvText( std::string ss ) ;
    void on_pushButton_clicked();

private:
    Ui_MainWindow *ui;
};

#endif // MAINWINDOW_H
