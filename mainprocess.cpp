#include <mainwindow.h>
#include <mainprocess.h>
#include <global_variable.h>
#include <dbm_to_meter.h>

using namespace std;

mutex mylock ;
// lock data

Traceing * slotPtr = NULL ;

vector<RasPI_Area> vector_area ;
vector<AP_Info> allAP ;
vector<thread> v_shelter_monitor ;
map<string,dbmSeq> devicesList2 ;
int seqNum = 0 ;
int RasPI_AP_Num = 0;

vector<RasPI_Area> GetAreaFromConfig( string cp ) {
    // get input area ip address in "config.conf" file
    vector<RasPI_Area> vector_input_area ;
    FILE * confFile = fopen( cp.c_str(), "r") ;
    string temp[5], name[5] ;
    int temp_x[5] ;
    int temp_y[5] ;

    // example:
    // Area1_AP = 192.168.11.20 , 0,0 {
    //   MON1 = 192.168.11.21 , 10,0
    //   MON2 = 192.168.11.19 , 0,10
    //   MON3 = 192.168.11.18 , 10,10
    //   MON4 = 192.168.11.17 , 20,20
    // }
    char ch[100] ;
    if ( confFile != NULL ) {

        while ( fscanf( confFile, "%s", ch ) != EOF ) {

            for ( int counter = 0 ; 5 > counter ; ++counter ) {

                name[counter] = ch ;
                // name
                fscanf( confFile, "%s", ch ) ; // =
                fscanf( confFile, "%s", ch ) ;
                temp[counter] = ch ;
                // address
                fscanf( confFile, "%s", ch ) ; // ,
                fscanf( confFile, "%s", ch ) ;
                // location

                for ( int a = 0 ; strlen( ch ) > a ; ++a ) {
                    if ( ch[a] == ',' ) {
                        char ch_x[10], ch_y[10] ;

                        for ( int b = 0 ; a > b ; ++b )
                            ch_x[b] = ch[b] ;
                        ch_x[a] = '\0' ;
                        for ( int c = a+1, z = 0 ; strlen( ch ) >= c ; ++c ) {

                            if ( strlen( ch ) == c )
                                ch_y[z] = '\0' ;
                            else
                                ch_y[z] = ch[c] ;
                            z++ ;
                        }

                        temp_x[counter] = atoi( ch_x ) ;
                        temp_y[counter] = atoi( ch_y ) ;

                        break ;
                    } // if
                } // for

                if ( counter == 0 )
                    fscanf( confFile, "%s", ch ) ; // ignore {

                if ( counter == 4 )
                    fscanf( confFile, "%s", ch ) ; // ignore }

                if ( counter != 4 )
                    fscanf( confFile, "%s", ch ) ; // read next
            } // for one area

            RasPI_Area temp_area( temp, name, temp_x, temp_y ) ;
            vector_input_area.push_back( temp_area ) ;

            ++RasPI_AP_Num ; // setting PI_number

        } // while
    } else {
        cout << "No config.conf in the directory!" << endl ;
        return vector_input_area ;
    }

    return vector_input_area ;
} // GetAreaFromConfig()

void cleanUp() {
    for ( int i = 0 ; i < allAP.size() ; i ++ ) {
        ssh_disconnect( allAP.at(i).mySession ) ;
        ssh_free( allAP.at(i).mySession );
    } // for

    vector_area.clear() ;
    allAP.clear();
    v_shelter_monitor.clear();
    devicesList2.clear();
    seqNum = 0 ;
    RasPI_AP_Num = 0 ;
} // cleanUp()

void RunProcess( string configPath, Traceing * sp ) {
    slotPtr = sp ;
    vector<thread> threads ;
    vector_area = GetAreaFromConfig( configPath ) ;

    if ( vector_area.empty() ) {
      exit(2) ;
    } // if

    cout << "CreateAP_Link!!" << endl ;
    CreateAP_Link( vector_area, threads ) ;
    cout << RasPI_AP_Num << endl ;
    bool traceEnd = false ;
    while ( !traceEnd ) {

        get_location( vector_area ) ;
        // refresh the location x, y

        for ( int i = 0 ; i < allAP.size() ; i ++ ) {
            for ( map<string,dbmSeq>::iterator it = allAP.at(i).devicesList.begin() ; it != allAP.at(i).devicesList.end() ; ) {
                if ( it->second.seq != seqNum ) {
                    map<string,dbmSeq>::iterator it2 = it ;
                    it ++ ;
                    allAP.at(i).devicesList.erase( it2 ) ;
                } // if
                else {
                    stringstream ss ;
                    //mylock.lock() ;

                    ss << allAP.size() << " " << allAP.at(i).devicesList.size() << endl ;
                    ss << "RasPI" << allAP.at(i).ipAddr << "#" << i << " " << it->first << " -> "
                                           << devicesList2[ it->first ].meter[0] << " "
                                           << devicesList2[ it->first ].meter[1] << " "
                                           << devicesList2[ it->first ].meter[2] << " "
                                           << devicesList2[ it->first ].meter[3] << " "
                                           << devicesList2[ it->first ].meter[4] << " "
                                           << "seq: " << it->second.seq << endl ;
                    //mylock.unlock() ;
                    cout << ss.str() ;
                    slotPtr->UpdateTextBrowser( ss.str() );
                    it ++ ;
                } // else

            } // for
            cout << endl ;
        } // for

        if ( !slotPtr->runFlag ) {
            for ( int i = 0 ; i < threads.size() ; i ++ ) {
                threads.at(i).join() ;
            } // for

            for ( int x = 0 ; x < v_shelter_monitor.size() ; x++ )
                v_shelter_monitor.at(x).join() ;

            cleanUp();
            traceEnd = true ;
        } // if


        seqNum = !seqNum ;
        this_thread::sleep_for( std::chrono::milliseconds(100) ) ;

    } // while (true)

    cout << "Traceing end!!" << endl ;
    this_thread::sleep_for( std::chrono::seconds(3) ) ;

    return ;
} // RunProcess()
