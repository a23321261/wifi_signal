#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QObject>
#include <mainprocess.h>

Q_DECLARE_METATYPE(std::string)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui_MainWindow)
{
    ui->setupUi(this);
    if ( configPath.isEmpty() ) {
       ui->configfileName->setText( "請選擇wifi設定檔......" );
    } // if
}

void MainWindow::setText(QString target) {
   // ui->textEdit->setText( target );
} // setText()

MainWindow::~MainWindow()
{
    delete ui;
}

Traceing::Traceing() {
  qRegisterMetaType<std::string>("std::string");
} // Traceing::Traceing()

Traceing::Traceing(std::string p) {
    path = p ;
    qRegisterMetaType<std::string>("std::string") ;
} // Traceing::Traceing( string p, MainWindow * mw )

void Traceing::run() {
    RunProcess( path, this ) ;
} // Traceing::run()

void Traceing::UpdateTextBrowser( std::string ss ) {
    emit sendText( ss );
} // Traceing::UpdateTextBrowser()

void MainWindow::on_action_triggered()
{
    configPath = QFileDialog::getOpenFileName(this, tr("開啟設定檔"), QString(),
            tr("設定檔 (*.conf)"));

    if (!configPath.isEmpty()) {
        QFile file(configPath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("錯誤"), tr("無法開啟檔案"));
            return;
        } // if
        else {
            ui->configfileName->setText( configPath );
            QTextStream in(&file);
            ui->textBrowser->setText(in.readAll());
            ui->pushButton->setEnabled(true);
            file.close();
        } // else
    } // if
} // on_action_triggered()

void MainWindow::recvText( std::string ss ) {
    ui->textBrowser->setText( ss.c_str() );
} // MainWindow::recvText( std::string ss )

void MainWindow::on_pushButton_clicked()
{
  if ( !isTraceing ) {
      tracker = new Traceing( configPath.toLocal8Bit().constData() ) ;
      if ( tracker != NULL ) {
          isTraceing = true ;
          ui->pushButton->setText( QString( "停止追蹤" ) );
          QObject::connect( tracker, SIGNAL( sendText(std::string) ), this, SLOT(recvText(std::string) ) );
          tracker->start();
      } // if
      else {
          ui->configfileName->setText( QString( "追蹤執行續創建失敗!!" ) );
      } // else
  } // if
  else {
    tracker->runFlag = false ;
    while( tracker->isRunning() )
        ui->textBrowser->setText( "等待追蹤執行緒結束......\n" );
    delete tracker ;
    isTraceing = false ;
    ui->pushButton->setText( QString( "開始追蹤" ) );
  } // else
} // MainWindow::on_pushButton_clicked()
