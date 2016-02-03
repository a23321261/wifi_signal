#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include<normalLib.h>
#include<ui_mainwindow.h>
#include<mainwindow.h>

namespace Ui {
class MainWindow ;
}

vector<RasPI_Area> GetAreaFromConfig( string cp ) ;
void RunProcess(string configPath, Traceing *sp) ;

#endif // MAINPROCESS_H
