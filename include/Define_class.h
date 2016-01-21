#ifndef DEFINE_CLASS_H_INCLUDED
#define DEFINE_CLASS_H_INCLUDED
#include <string>
#include <map>

using namespace std ;

class RasPI_Area {
public :
  string AP ;
  string Mon[4] ;

  RasPI_Area( string _AP, string _mon0, string _mon1, string _mon2, string _mon3 ) ;
};

class dbmSeq {
public :
    int signal[5] ; // each monitor just handle one of the signal[], ex: mon1 -> signal[1]
    int seq ;       // mon0 is for AP                                    mon2 -> signal[2]
    // a dbmSeq has five signal                          mon3 -> signal[3]
    bool b_HasThread ;  //                                               mon4 -> signal[4]
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
