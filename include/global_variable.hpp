#ifndef GLOBAL_VARIABLE_H_INCLUDED
#define GLOBAL_VARIABLE_H_INCLUDED

#include <normalLib.h>
#include <Define_class.hpp>

using namespace std ;

extern vector<AP_Info> allAP ;
extern vector<thread> v_shelter_monitor ;
extern map<string,dbmSeq> devicesList2 ;
extern int seqNum ;

#endif // GLOBAL_VARIABLE_H_INCLUDED
