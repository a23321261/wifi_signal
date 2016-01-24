#ifndef MONITOR_MANAGE_H_INCLUDED
#define MONITOR_MANAGE_H_INCLUDED

using namespace std ;

void monitor_manager( AP_Info* thisArea, string targetMAC ) ;
char * GetMonitorData( ssh_channel channel ) ;

#endif // MONITOR_MANAGE_H_INCLUDED
