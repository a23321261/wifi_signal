#include <libssh/libssh.h>
#include <iostream>
#include <Define_class.h>
#include <vector>
#include <thread>
#include <global_variable.h>
#include <remote_processes.h>
using namespace std ;

int verify_knownhost(ssh_session session) {
    // show the status for ssh connection
    int state, hlen;
    unsigned char *hash = NULL;
    char *hexa;
    char buf[10];
    state = ssh_is_server_known(session);
    hlen = ssh_get_pubkey_hash(session, &hash);

    if (hlen < 0)
        return -1;
    switch (state) {
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

void CreateAP_Link( vector<RasPI_Area> thisArea, vector<thread> & threads ) {
    cout << "GG" << endl ;
    cout << RasPI_AP_Num << endl ;
    for ( int i = 0 ; i < RasPI_AP_Num ; i ++ ) {
        cout << "super?" << endl ;
        cout << thisArea.at(i).AP << " "
             << thisArea.at(i).Mon[0] << " "
             << thisArea.at(i).Mon[1] << " "
             << thisArea.at(i).Mon[2] << " "
             << thisArea.at(i).Mon[3] << endl ;

        cout << "der1?" << endl ;

        allAP.push_back( *new AP_Info( thisArea.at(i).AP, thisArea.at(i).Mon[0],
                                       thisArea.at(i).Mon[1], thisArea.at(i).Mon[2],
                                       thisArea.at(i).Mon[3], ssh_new() ) ) ;
        // build session
        cout << "der2?" << endl ;
        if ( CreateSSH_Link( allAP.at(i).mySession, allAP.at(i).ipAddr ) ) {
            SetWifiChannel( allAP.at(i).mySession, 11 ) ;
            threads.push_back( thread( createAP_Process, i ) ) ;
            cout << allAP.at(i).devicesList.size() << endl ;
        } // if
        // use the session to throw thread

    } // for
   cout << "ININ" << endl ;
} // CreateAP_Link()

