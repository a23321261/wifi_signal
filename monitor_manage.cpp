#include <Define_class.h>
#include <global_variable.h>
#include <iostream>
#include <remote_processes.h>
#include <SSH_IO.h>
using namespace std ;

void monitor_manager( AP_Info thisArea, string targetMAC ) {
    devicesList2[ targetMAC ].b_HasThread = true ;
    ssh_session temp_session ;

    if ( thisArea.myMonitor[0].mySession == NULL ) ;
        //cout << "Monitor: (" << thisArea.myMonitor[0].monIP << ") session is null" << endl ;
    else {
        temp_session = ssh_new() ;
        CreateSSH_Link( temp_session, thisArea.myMonitor[0].monIP ) ;
        SetWifiChannel( temp_session, 11 ) ;
        v_shelter_monitor.push_back( thread( Get_Monitor_db, thisArea.myMonitor[0],
                                             targetMAC, temp_session, 1 ) ) ; // handle index 1 -> mon0
    }

    if ( thisArea.myMonitor[1].mySession == NULL ) ;
        //cout << "Monitor: (" << thisArea.myMonitor[1].monIP << ") session is null" << endl ;
    else {
        temp_session = ssh_new() ;
        CreateSSH_Link( temp_session, thisArea.myMonitor[1].monIP ) ;
        SetWifiChannel( temp_session, 11 ) ;
        v_shelter_monitor.push_back( thread( Get_Monitor_db, thisArea.myMonitor[1],
                                             targetMAC, temp_session, 2 ) ) ; // handle index 2 -> mon1
    }

    if ( thisArea.myMonitor[2].mySession == NULL ) ;
        //cout << "Monitor: (" << thisArea.myMonitor[2].monIP << ") session is null" << endl ;
    else {
        temp_session = ssh_new() ;
        CreateSSH_Link( temp_session, thisArea.myMonitor[2].monIP ) ;
        SetWifiChannel( temp_session, 11 ) ;
        v_shelter_monitor.push_back( thread( Get_Monitor_db, thisArea.myMonitor[2],
                                             targetMAC, temp_session, 3 ) ) ; // handle index 3 -> mon2
    }

    if ( thisArea.myMonitor[3].mySession == NULL ) ;
        //cout << "Monitor: (" << thisArea.myMonitor[3].monIP << ") session is null" << endl ;
    else {
        temp_session = ssh_new() ;
        CreateSSH_Link( temp_session, thisArea.myMonitor[3].monIP ) ;
        SetWifiChannel( temp_session, 11 ) ;
        v_shelter_monitor.push_back( thread( Get_Monitor_db, thisArea.myMonitor[3],
                                             targetMAC, temp_session, 4 ) ) ; // handle index 4 -> mon3
    }

    return ;
}

char * GetMonitorData( ssh_channel channel ) {
    vector<char*> temp ;
    char * buffer = (char*)calloc( 64, sizeof( char ) ) ;
    int nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0) ;
    //write( 1, buffer, nbytes ) ;
    while ( nbytes > 0 ) {
        temp.push_back( buffer ) ;
        buffer = (char*)calloc( 64, sizeof( char ) ) ;
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0 );
        //write( 1, buffer, nbytes ) ;
    } // while ( nbytes > 0 )

    free( buffer ) ;
    char * result = (char*)calloc( temp.size() * 64, sizeof(char) ) ;
    for ( int i = 0 ; i < temp.size() ; i ++ ) {
        strncat( result, temp.at(i), 64 ) ;
        free( temp.at(i) ) ;
    } // for

    return result ;
} // GetMonitorData()
