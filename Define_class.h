#ifndef DEFINE_CLASS_H_INCLUDED
#define DEFINE_CLASS_H_INCLUDED
#include <string>
#include <map>
#include <libssh/libssh.h>

using namespace std ;

class RasPI_Area {
public :
  string AP_name ;
  string Mon_name[4] ;
  string AP ; // IP address
  string Mon[4] ;
  int x[5] ;
  int y[5] ;

  RasPI_Area(string info[], string name[], int _x[5], int _y[5] ) ;
};

class dbmSeq {
public :
    int signal[5] ; // each monitor just handle one of the signal[], ex: mon1 -> signal[1]
    int seq ;       // mon0 is for AP                                    mon2 -> signal[2]
                    // a dbmSeq has five signal                          mon3 -> signal[3]
                    //                                                   mon4 -> signal[4]
    float meter[5] ;  // same as signal
    int location[2] ; // x = 0 ; y = 1 ;
    bool b_HasThread ;
    // bool default false
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

    AP_Info( string i, string m0, string m1, string m2, string m3, ssh_session s ) ;
};

#endif // DEFINE_CLASS_H_INCLUDED
