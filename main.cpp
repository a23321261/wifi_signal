#define LIBSSH_STATIC

#include <mainwindow.h>
#include <QApplication>

using namespace std;

int main( int argc, char *argv[] ) {
    QApplication qa( argc, argv ) ;
    MainWindow testWifi_GUI ;
    testWifi_GUI.show() ;
    return qa.exec() ;
} // main()
