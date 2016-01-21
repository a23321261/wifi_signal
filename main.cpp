#define LIBSSH_STATIC

#include <normalLib.h>
#include <global_variable.h>
using namespace std;

mutex mylock ;
// lock data

vector<AP_Info> allAP ;
vector<thread> v_shelter_monitor ;
map<string,dbmSeq> devicesList2 ;
int seqNum = 0 ;

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
    } else {
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
      cout << devicesList2[ "inin" ].b_HasThread << "   bool" << endl ;
      devicesList2[ "inin" ].seq = 15 ;
      devicesList2[ "inin" ].b_HasThread = true ;
      cout << devicesList2[ "inin" ].seq << "   one" << endl ;
      cout << devicesList2[ "inin" ].b_HasThread << "   bool" << endl ;
      map<string,dbmSeq>::iterator it = devicesList2.begin() ;
      devicesList2.erase( it ) ;
      cout << devicesList2[ "inin" ].seq << "   two" << endl ;
      cout << devicesList2[ "inin" ].b_HasThread << "   bool" << endl ;
    */
    while ( true ) {
        system( "CLS" ) ;
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
                    ss << "RasPI #" << i << " " << it->first << " -> "
                       << devicesList2[ it->first ].signal[0] << " " << devicesList2[ it->first ].signal[1] << " "
                       << devicesList2[ it->first ].signal[2] << " " << devicesList2[ it->first ].signal[3] << " "
                       << devicesList2[ it->first ].signal[4] << " " << "seq: " << it->second.seq << endl ;
                    //mylock.unlock() ;
                    cout << ss.str() ;
                    it ++ ;
                } // else

            } // for
            cout << endl ;
        } // for

        seqNum = !seqNum ;
        this_thread::sleep_for( std::chrono::milliseconds(100) ) ;

    } // while (true)

    for ( int i = 0 ; i < threads.size() ; i ++ )
        threads.at(i).join() ;

    for ( int x = 0 ; x < v_shelter_monitor.size() ; x++ )
        v_shelter_monitor.at(x).join() ;

    return 0 ;
} // main()
