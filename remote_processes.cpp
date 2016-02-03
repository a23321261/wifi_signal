#include <libssh/libssh.h>
#include <vector>
#include <Define_class.h>
#include <sstream>
#include <iostream>
#include <global_variable.h>
#include <SSH_IO.h>
#include <monitor_manage.h>
#include <dbm_to_meter.h>

using namespace std ;

char * GetAllStationData( ssh_channel channel ) {
    vector<char*> temp ;
    char * buffer = (char*)calloc( 64, sizeof( char ) ) ;
    int nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0) ;
    // get the result from buffer

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
} // GetAllStationData()

int Get_Monitor_db( MonitorInfo thisMon, string targetMAC, ssh_session session, int index_handle ) {
    int curDataCounter = -1 ;
    stringstream ss ;

    ss << "tcpdump -i wlan0 \'ether src " << targetMAC << "\'" ;
    // make command for openwrt monitor

    ssh_channel channel = NULL ;
    int rc, nbytes ;


    if ( channel == NULL ) {
        channel = ssh_channel_new( session );
        cout << "Monitor: (" << thisMon.monIP << ")create channel..." << endl ;

        if ( channel == NULL ) {
            cout << "Monitor: (" << thisMon.monIP << ")create failed (null)..." << endl ;
            return SSH_ERROR ;
        }

    } // if

    rc = ssh_channel_open_session( channel ) ;
    if ( rc != SSH_OK ) {
        cout << "Monitor: (" << thisMon.monIP << ")create failed (SSH NOT OK)..." << endl ;
        ssh_channel_free( channel );
        return rc;
    } // if

    else {
        cout << "Monitor: (" << thisMon.monIP << ")create successful" << endl ;
        char allData[256] = "" ;

        rc = ssh_channel_request_exec( channel, ss.str().c_str() );
        // execute the tcpdump command

        if ( rc != SSH_OK ) {
            //cout << "im here \n" ;
            return rc ;
        }

        while ( slotPtr->runFlag ) {
            nbytes = ssh_channel_read( channel, allData, 256, 0 ) ;
            if ( nbytes < 0 )
                return SSH_ERROR;

            char * pch = strtok( allData, " " ) ;
            while ( pch != NULL ) {

                if ( pch[ strlen( pch ) - 1 ] == 'B' && pch[ strlen( pch ) - 2 ] == 'd' ) {
                    //pch[ strlen( pch ) - 1 ] = '\0' ;
                    //pch[ strlen( pch ) - 2 ] = '\0' ;
                    //cout << atoi( pch ) << endl ;

                    // Does here need mylock.lock() ?
                    int temp_signal = atoi( pch ) ;
                    devicesList2[ targetMAC ].signal[index_handle] = temp_signal ; // handle the dbm for the devicesList
                    devicesList2[ targetMAC ].meter[index_handle] = turn_dbm_to_meter( temp_signal ) ;
                } // if

                pch = strtok( NULL, " " );
            } // ( pch != NULL )

            if ( devicesList2[ targetMAC ].b_HasThread == false ) {
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                ssh_disconnect( session ) ;
                ssh_free( session ) ;
                cout << "Monitor: (" << thisMon.monIP << ") close" << endl ;
                //cout << targetMAC << " => " << devicesList2[ targetMAC ].b_HasThread << endl ;
                return 0 ;
            } // if the MAC is disconnected, this thread will return ;

            this_thread::sleep_for( std::chrono::milliseconds(100) ) ;
        } // while ( true )
    } // else

    return 0 ;
} // Get_Monitor_db()

int createAP_Process( int AP_index ) {
    map<string, int[2]> AP_dulplicated ;
    int curDataCounter = -1 ;

    ssh_channel channel ;
    int rc = 0, nbytes = 0 ;
    bool isFirst = true ;

    while ( slotPtr->runFlag ) {
        // cout << "AP: (" << allAP.at(AP_index).ipAddr << ")create channel..." << endl ;
        channel = ssh_channel_new( allAP.at(AP_index).mySession );
        // create a ssh channel in thisAP->mySession

        if ( channel == NULL ) {
            cout << "AP: (" << allAP.at(AP_index).ipAddr << ")create failed (null)" << endl ;
            return SSH_ERROR ;
        }

        rc = ssh_channel_open_session( channel ) ;
        if ( rc != SSH_OK ) {
            cout << "AP: (" << allAP.at(AP_index).ipAddr << ")create failed (SSH NOT OK)" << endl ;
            ssh_channel_free( channel );
            return rc;
        } // if

        rc = ssh_channel_request_exec( channel, "iw dev wlan0 station dump");
        // command for openwrt AP

        if ( rc != SSH_OK ) {
            ssh_channel_close( channel );
            ssh_channel_free( channel );
            return rc ;
        } // if
        else {
            //cout << "AP: (" << allAP.at(AP_index).ipAddr << ")create successful" << endl ;
            string targetMAC = "" ;

            char * allData = GetAllStationData( channel ) ;

            char * pch = strtok( allData, " \n\t" ) ;
            bool isMAC = false, isSignal = false ;

            while ( pch != NULL ) {
                // cut token loop
                // cut the MAC address and DBM from AP
                if ( strcmp( "Station", pch ) == 0 ) {
                    isMAC = true ;
                } // if
                else if ( strcmp( "signal:", pch ) == 0 ) {
                    isSignal = true ;
                } // else if
                else if ( isMAC ) {
                    targetMAC = *new string( pch ) ;
                    //cout << "mac " << targetMAC << endl ;
                    isMAC = false ;
                } // else if
                else if ( isSignal ) {
                  int temp_signal = atoi( pch ) ;

                  if ( AP_dulplicated[ targetMAC ][1] == 0 ) {
                    AP_dulplicated[ targetMAC ][0] = temp_signal ;
                    ++AP_dulplicated[ targetMAC ][1] ;
                  } // first time get the AP signal
                  else if ( AP_dulplicated[ targetMAC ][1] > 0 ) {
                    if ( AP_dulplicated[ targetMAC ][0] == temp_signal ) {
                      if ( AP_dulplicated[ targetMAC ][1] < 70 )
                        ++AP_dulplicated[ targetMAC ][1] ;
                    } // duplicate signal

                    if ( AP_dulplicated[ targetMAC ][0] != temp_signal ) {
                      AP_dulplicated[ targetMAC ][0] = temp_signal ;
                      devicesList2[ targetMAC ].meter[0] = turn_dbm_to_meter( temp_signal ) ;
                      AP_dulplicated[ targetMAC ][1] = 1 ;
                    } // different signal

                    if ( AP_dulplicated[ targetMAC ][1] < 70 ) {
                      devicesList2[ targetMAC ].signal[0] = temp_signal ;
                      devicesList2[ targetMAC ].meter[0] = turn_dbm_to_meter( temp_signal ) ;
                      allAP.at(AP_index).devicesList[ targetMAC ].seq = seqNum ;
                      if ( devicesList2[ targetMAC ].b_HasThread == false ) {
                        monitor_manager( allAP.at(AP_index), targetMAC ) ;
                      } // if it didn't has monitor threads

                    } // acceptable range
                    else {
                      devicesList2[ targetMAC ].b_HasThread = false ;
                    } // unacceptable range, higher than threshold
                    // and do not update global signal list and kill the mon1 to mon4
                  } // else if

                    isSignal = false ;
                } // else if

                pch = strtok( NULL, " \n\t" );
            } // ( pch != NULL )

            ssh_channel_close(channel);
            ssh_channel_free(channel);
        } // else

        if ( nbytes < 0 ) {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        } // if

        // system( "cls" ) ;
        this_thread::sleep_for( std::chrono::milliseconds(100) ) ;
    } // while ( true )


    cout << "AP: (" << allAP.at(AP_index).ipAddr << ") close" << endl ;
    return SSH_OK ;
} // show_remote_processes()

int SetWifiChannel( ssh_session thisSession, int newChannel ) {
    ssh_channel channel ;
    channel = ssh_channel_new( thisSession );
    stringstream ss ;
    ss << "iw wlan0 set channel " << newChannel ;
    int rc = 0 ;
    // create a ssh channel in thisAP->mySession

    if ( channel == NULL ) {
        cout << "Set wifi channel fail when creating new ssh_channel!!" << endl ;
        return SSH_ERROR ;
    }
    else {
        rc = ssh_channel_open_session( channel ) ;
        if ( rc != SSH_OK ) {
            cout << "Set wifi channel fail when opening a session by ssh_channel!!" << endl ;
            ssh_channel_free( channel );
            return rc;
        } // if
        else {
            rc = ssh_channel_request_exec( channel, ss.str().c_str() );
            /*executing a remote command*/
            if ( rc != SSH_OK ) {
                cout << "Set wifi channel fail when execute channel-changing cmd!!" << endl ;
                ssh_channel_close( channel );
                ssh_channel_free( channel );
                return rc ;
            } // if
            else {
                char returnBuffer[64] = "" ;
                ssh_channel_read(channel, returnBuffer, 64, 0) ;
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);
                cout << "Set wifi channel successful :" << endl
                     << returnBuffer << endl ;
            } // else
        } // else
    } // else
} // SetChannel()
