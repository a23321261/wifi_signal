#ifndef REMOTE_PROCESSES_H_INCLUDED
#define REMOTE_PROCESSES_H_INCLUDED

using namespace std ;

int Get_Monitor_db( MonitorInfo thisMon, string targetMAC, ssh_session session, int index_handle ) ;
int createAP_Process( int AP_index ) ;

#endif // REMOTE_PROCESSES_H_INCLUDED
