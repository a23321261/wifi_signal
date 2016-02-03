#include <Define_class.h>
#include <vector>
#include <thread>
#include <iostream>
#include <mainprocess.h>

using namespace std ;
int verify_knownhost( ssh_session session ) ;
bool CreateSSH_Link( ssh_session & targetSession, string targetIP ) ;
void CreateAP_Link( RasPI_Area thisArea, vector<thread> & threads ) ;

RasPI_Area::RasPI_Area( string info[5], string name[5], int _x[], int _y[] ) {
    AP = info[0] ;
    Mon[0] = info[1] ;
    Mon[1] = info[2] ;
    Mon[2] = info[3] ;
    Mon[3] = info[4] ;
    this->AP_name = name[0] ;
    this->Mon_name[0] = name[1] ;
    this->Mon_name[1] = name[2] ;
    this->Mon_name[2] = name[3] ;
    this->Mon_name[3] = name[4] ;
    for ( int a = 0 ; 5 > a ; ++a ) {
        this->x[a] = _x[a] ;
        this->y[a] = _y[a] ;
    }

} // RasPI_Area()

AP_Info::AP_Info( string i, string m0, string m1, string m2, string m3, ssh_session s ) {
    ipAddr = i ;
    myMonitor[0].monIP = m0 ;
    myMonitor[1].monIP = m1 ;
    myMonitor[2].monIP = m2 ;
    myMonitor[3].monIP = m3 ;

    myMonitor[0].mySession = ssh_new() ;
    cout << "A" << endl ;
    if ( !CreateSSH_Link( myMonitor[0].mySession, myMonitor[0].monIP ) )
        myMonitor[0].mySession = NULL ;

    myMonitor[1].mySession = ssh_new() ;
    cout << "B" << endl ;
    if ( !CreateSSH_Link( myMonitor[1].mySession, myMonitor[1].monIP ) )
        myMonitor[1].mySession = NULL ;

    myMonitor[2].mySession = ssh_new() ;
    cout << "C" << endl ;
    if ( !CreateSSH_Link( myMonitor[2].mySession, myMonitor[2].monIP ) )
        myMonitor[2].mySession = NULL ;

    myMonitor[3].mySession = ssh_new() ;
    cout << "D" << endl ;
    if ( !CreateSSH_Link( myMonitor[3].mySession, myMonitor[3].monIP ) )
        myMonitor[3].mySession = NULL ;
    mySession = s ;

} // AP_Info() initialize


