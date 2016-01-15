#define LIBSSH_STATIC
#define RasPI_AP_Num 1

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include <string>
#include <libssh/libssh.h>
#include <thread>
#include <map>
#include <vector>
#include <mutex>
#include <sstream>
#include <chrono>

using namespace std;

int seqNum = 0 ;

class RasPI_Area {
public :
  string AP ;
  string Mon[4] ;

  RasPI_Area( string _AP, string _mon0, string _mon1, string _mon2, string _mon3 ) {
    AP = _AP ;
    Mon[0] = _mon0 ;
    Mon[1] = _mon1 ;
    Mon[2] = _mon2 ;
    Mon[3] = _mon3 ;
  } // RasPI_Area()
};

mutex mylock ;
// lock data

int verify_knownhost(ssh_session session)
{
    // show the status for ssh connection
  int state, hlen;
  unsigned char *hash = NULL;
  char *hexa;
  char buf[10];
  state = ssh_is_server_known(session);
  hlen = ssh_get_pubkey_hash(session, &hash);

  if (hlen < 0)
    return -1;
  switch (state)
  {
    case SSH_SERVER_KNOWN_OK:
      break; /* ok */
    case SSH_SERVER_KNOWN_CHANGED:
      fprintf(stderr, "Host key for server changed: it is now:\n");
      ssh_print_hexa("Public key hash", hash, hlen);
      fprintf(stderr, "For security reasons, connection will be stopped\n");
      free(hash);
      return -1;
    case SSH_SERVER_FOUND_OTHER:
      fprintf(stderr, "The host key for this server was not found but an other"
        "type of key exists.\n");
      fprintf(stderr, "An attacker might change the default server key to"
        "confuse your client into thinking the key does not exist\n");
      free(hash);
      return -1;
    case SSH_SERVER_FILE_NOT_FOUND:
      fprintf(stderr, "Could not find known host file.\n");
      fprintf(stderr, "If you accept the host key here, the file will be"
       "automatically created.\n");
      /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
      fprintf(stderr, "Public key hash: %s\n", hexa);
      free(hexa);
      if (fgets(buf, sizeof(buf), stdin) == NULL)
      {
        free(hash);
        return -1;
      }
      if (strcmp(buf, "yes\n") != 0)
      {
        free(hash);
        fprintf(stderr, "OK\n" );
        return -1;
      }
      if (ssh_write_knownhost(session) < 0)
      {
        fprintf(stderr, "Error %s\n", strerror(errno));
        free(hash);
        return -1;
      }
      break;
    case SSH_SERVER_ERROR:
      fprintf(stderr, "Error %s", ssh_get_error(session));
      free(hash);
      return -1;
  }

  free(hash);
  return 0;
} // verify_knownhost()

bool CreateSSH_Link( ssh_session & targetSession, string targetIP ) {

  int rc = 0 ;
  if ( targetSession == NULL )
    exit(-1) ;

  ssh_options_set( targetSession, SSH_OPTIONS_HOST, targetIP.c_str() ) ;
  rc = ssh_connect( targetSession ) ;
  // rc -> ssh
  if ( rc != SSH_OK ) {
    cerr << "Error connecting to " << targetIP << " "
         << ssh_get_error( targetSession ) << endl ;
    ssh_free( targetSession );
    return false ;
  } // if ( rc != SSH_OK )
  else if ( verify_knownhost( targetSession ) < 0 ) {
        //show the host status and check
    ssh_disconnect( targetSession ) ;
    ssh_free( targetSession );
    return false ;
  } // else if ( verify_knownhost( targetSession ) < 0 )
  else {

    // connected ssh !
    rc = ssh_userauth_password( targetSession, "root", "0000" );
    if ( rc != SSH_AUTH_SUCCESS ) {
      // wrong password
      cerr << "Error authenticating with password: "
           << ssh_get_error( targetSession ) << endl ;

      ssh_disconnect( targetSession ) ;
      ssh_free( targetSession ) ;
      return false ;
    } // if ( rc != SSH_AUTH_SUCCESS )

    return true ;
  } // else

  return false ;
} // CreateSSH_Link()

class dbmSeq {
public :
  int signal[3] ; ////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  int seq ;
  // a dbmSeq has three signal
};

class MonitorInfo {
public:
  string monIP ;
  ssh_session mySession ;
  ssh_channel myChannel ;
  // this class is for monitor raspberry pi 2
};

class AP_Info {
public:
  string ipAddr ;
  ssh_session mySession ;
  MonitorInfo myMonitor[4] ;

  map<string,dbmSeq> devicesList ;
  //  ip    ,dbm

  AP_Info( string i, string m0, string m1, string m2, string m3, ssh_session s ) {
    ipAddr = i ;
    myMonitor[0].monIP = m0 ;
    myMonitor[0].mySession = ssh_new() ;
    CreateSSH_Link( myMonitor[0].mySession, myMonitor[0].monIP ) ;
    myMonitor[1].monIP = m1 ;
    myMonitor[1].mySession = ssh_new() ;
    CreateSSH_Link( myMonitor[1].mySession, myMonitor[1].monIP ) ;
    myMonitor[2].monIP = m2 ;
    myMonitor[2].mySession = ssh_new() ;
    CreateSSH_Link( myMonitor[2].mySession, myMonitor[2].monIP ) ;
    myMonitor[3].monIP = m3 ;
    myMonitor[3].mySession = ssh_new() ;
    CreateSSH_Link( myMonitor[3].mySession, myMonitor[3].monIP ) ;
    mySession = s ;
  } // AP_Info() initialize
};

vector<AP_Info> allAP ;

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

int show_remote_processes( AP_Info * thisAP ) {
  int curDataCounter = -1 ;

  ssh_channel channel ;
  int rc = 0, nbytes = 0 ;

  while ( true ) {
    cout << "AP: (" << thisAP->ipAddr << ")create channel..." << endl ;
    channel = ssh_channel_new( thisAP->mySession );
    // create a ssh channel in thisAP->mySession

    if ( channel == NULL ) {
      cout << "AP: (" << thisAP->ipAddr << ")create failed (null)" << endl ;
      return SSH_ERROR ;
    }

    rc = ssh_channel_open_session( channel ) ;
    if ( rc != SSH_OK ) {
      cout << "AP: (" << thisAP->ipAddr << ")create failed (SSH NOT OK)" << endl ;
      ssh_channel_free( channel );
      return rc;
    } // if

    rc = ssh_channel_request_exec(channel, "iw dev wlan0 station dump");
    // command for openwrt AP

    if ( rc != SSH_OK ) {
      ssh_channel_close( channel );
      ssh_channel_free( channel );
      return rc ;
    } // if
    else {
      cout << "AP: (" << thisAP->ipAddr << ")create successful" << endl ;
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
          thisAP->devicesList[ targetMAC ].signal[0] = atoi( pch ) ;
          thisAP->devicesList[ targetMAC ].seq = seqNum ;
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
  } // while ( true )

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  cout << "AP: (" << thisAP->ipAddr << ") close" << endl ;
  return SSH_OK ;
} // show_remote_processes()

int Get_Monitor_db( MonitorInfo thisMon, string targetMAC ) {
  int curDataCounter = -1 ;
  stringstream ss ;
  ss << "tcpdump -i wlan0 \'ether src " << targetMAC
     << "\' -c 6" ;
  // make command for openwrt monitor

  ssh_channel channel ;
  int rc, nbytes, result = 0 ;

  if ( thisMon.myChannel == NULL ) {
    thisMon.myChannel = ssh_channel_new( thisMon.mySession );
    cout << "Monitor: (" << thisMon.monIP << ")create channel..." << endl ;

    if ( thisMon.myChannel == NULL ) {
      cout << "Monitor: (" << thisMon.monIP << ")create failed (null)..." << endl ;
      return SSH_ERROR ;
    }

  } // if

  rc = ssh_channel_open_session( thisMon.myChannel ) ;
  if ( rc != SSH_OK ) {
    cout << "Monitor: (" << thisMon.monIP << ")create failed (SSH NOT OK)..." << endl ;
    ssh_channel_free( thisMon.myChannel );
    return rc;
  } // if

  else {
    cout << "Monitor: (" << thisMon.monIP << ")create successful" << endl ;
    char allData[2048] = "" ;
    while ( result == 0 ) {
      rc = ssh_channel_request_exec( thisMon.myChannel, ss.str().c_str() );
      if ( rc != SSH_OK )
        return rc ;

      nbytes = ssh_channel_read( thisMon.myChannel, allData, 2048, 0 ) ;
      if ( nbytes < 0 )
        return SSH_ERROR;

      char * pch = strtok( allData, " \n\t" ) ;
      while ( pch != NULL ) {
        if ( pch[ strlen( pch ) - 1 ] == 'B' && pch[ strlen( pch ) - 2 ] == 'd' ) {
          //pch[ strlen( pch ) - 1 ] = '\0' ;
          //pch[ strlen( pch ) - 2 ] = '\0' ;
          // cout << pch << endl ;
          result = atoi( pch ) ;
        } // if

        pch = strtok( NULL, " \n\t" );
      } // ( pch != NULL )

      //cout << "ININDER" ;
    } // while

    //cout << "ININ\n" ;
  } // else


  return result ;
} // show_remote_processes()

void CreateAP_Link( RasPI_Area thisArea, vector<thread> & threads ) {
  for ( int i = 0 ; i < RasPI_AP_Num ; i ++ ) {
    allAP.push_back( *new AP_Info( thisArea.AP, thisArea.Mon[0],
                                   thisArea.Mon[1], thisArea.Mon[2],
                                   thisArea.Mon[3], ssh_new() ) ) ;

    if ( CreateSSH_Link( allAP.at(i).mySession, allAP.at(i).ipAddr ) ) {
      threads.push_back( thread( show_remote_processes, &allAP.at(i) ) ) ;
      cout << allAP.at(i).devicesList.size() << endl ;
    } // if
  } // for
} // CreateAP_Link()

vector<RasPI_Area> GetAreaFromConfig() {
    // get input area ip address in "config.conf" file
    vector<RasPI_Area> vector_input_area ;
    FILE * confFile = fopen( "config.conf", "r") ;
    string temp[5] ;
    char ch[100] ;
    if ( confFile != NULL ) {
        while ( fscanf( confFile, "%s", ch ) != EOF ) {
            temp[0] = ch ;
            // AP address
            fscanf( confFile, "%s", ch ) ;
            temp[1] = ch ;
            // MON1 address
            fscanf( confFile, "%s", ch ) ;
            temp[2] = ch ;
            // MON2 address
            fscanf( confFile, "%s", ch ) ;
            temp[3] = ch ;
            // MON3 address
            fscanf( confFile, "%s", ch ) ;
            temp[4] = ch ;
            // MON4 address

            RasPI_Area temp_area( temp[0], temp[1], temp[2], temp[3], temp[4] ) ;
            vector_input_area.push_back( temp_area ) ;
        }
    }
    else {
        cout << "No config.conf in the directory!" << endl ;
        return vector_input_area ;
    }

    return vector_input_area ;
} // GetAreaFromConfig()


int main() {
  vector<thread> threads ;

  vector<RasPI_Area> vector_area = GetAreaFromConfig() ;
  CreateAP_Link( vector_area.at(0), threads ) ;
/*
  while ( true ) {
    for ( int i = 0 ; i < allAP.size() ; i ++ ) {
      for ( map<string,dbmSeq>::iterator it = allAP.at(i).devicesList.begin() ; it != allAP.at(i).devicesList.end() ; ) {
        if ( it->second.seq != seqNum ) {
          map<string,dbmSeq>::iterator it2 = it ;
          it ++ ;
          allAP.at(i).devicesList.erase( it2 ) ;
        } // if
        else {
          stringstream ss ;
          ss << allAP.size() << " " << allAP.at(i).devicesList.size() << endl ;
          ss << "RasPI #" << i << " " << it->first << " -> "
             << it->second.signal[0] << " " << Get_Monitor_db( allAP.at(i).myMonitor[0], it->first ) << " "
             << Get_Monitor_db( allAP.at(i).myMonitor[1], it->first ) << " "
             << "seq: " << it->second.seq << endl ;

          cout << ss.str() ;
          it ++ ;
        } // else

      } // for
      cout << endl ;
    } // for

    seqNum = !seqNum ;
     this_thread::sleep_for( std::chrono::milliseconds(100) ) ;
  } // while

  for ( int i = 0 ; i < threads.size() ; i ++ )
    threads.at(i).join() ;
*/
  return 0 ;
} // main()
