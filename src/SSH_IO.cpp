#define RasPI_AP_Num 1

#include <SSH_IO.hpp>
#include <remote_processes.cpp>

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
    case SSH_SERVER_NOT_KNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        free(hexa);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            free(hash);
            return -1;
        }
        if (strcmp(buf, "yes\n") != 0) {
            free(hash);
            fprintf(stderr, "OK\n" );
            return -1;
        }
        if (ssh_write_knownhost(session) < 0) {
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

void CreateAP_Link( RasPI_Area thisArea, vector<thread> & threads ) {
    for ( int i = 0 ; i < RasPI_AP_Num ; i ++ ) {
        allAP.push_back( *new AP_Info( thisArea.AP, thisArea.Mon[0],
                                       thisArea.Mon[1], thisArea.Mon[2],
                                       thisArea.Mon[3], ssh_new() ) ) ;
        // build session

        if ( CreateSSH_Link( allAP.at(i).mySession, allAP.at(i).ipAddr ) ) {
            threads.push_back( thread( createAP_Process, &allAP.at(i) ) ) ;
            cout << allAP.at(i).devicesList.size() << endl ;
        } // if
        // use the session to throw thread

    } // for
} // CreateAP_Link()
