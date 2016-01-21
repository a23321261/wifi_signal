#include <Define_class.h>
#include <vector>
#include <thread>

int verify_knownhost( ssh_session session ) ;
bool CreateSSH_Link( ssh_session & targetSession, string targetIP ) ;
void CreateAP_Link( RasPI_Area thisArea, vector<thread> & threads ) ;

RasPI_Area::RasPI_Area( string _AP, string _mon0, string _mon1, string _mon2, string _mon3 ) {
    AP = _AP ;
    Mon[0] = _mon0 ;
    Mon[1] = _mon1 ;
    Mon[2] = _mon2 ;
    Mon[3] = _mon3 ;
} // RasPI_Area()

AP_Info::AP_Info( string i, string m0, string m1, string m2, string m3, ssh_session s ) {
    ipAddr = i ;
    myMonitor[0].monIP = m0 ;
    myMonitor[1].monIP = m1 ;
    myMonitor[2].monIP = m2 ;
    myMonitor[3].monIP = m3 ;

    myMonitor[0].mySession = ssh_new() ;
    if ( !CreateSSH_Link( myMonitor[0].mySession, myMonitor[0].monIP ) )
        myMonitor[0].mySession = NULL ;

    myMonitor[1].mySession = ssh_new() ;
    if ( !CreateSSH_Link( myMonitor[1].mySession, myMonitor[1].monIP ) )
        myMonitor[1].mySession = NULL ;

    myMonitor[2].mySession = ssh_new() ;
    if ( !CreateSSH_Link( myMonitor[2].mySession, myMonitor[2].monIP ) )
        myMonitor[2].mySession = NULL ;

    myMonitor[3].mySession = ssh_new() ;
    if ( !CreateSSH_Link( myMonitor[3].mySession, myMonitor[3].monIP ) )
        myMonitor[3].mySession = NULL ;
    mySession = s ;

} // AP_Info() initialize
