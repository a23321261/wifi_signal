#ifndef GLOBAL_VARIABLE_H_INCLUDED
#define GLOBAL_VARIABLE_H_INCLUDED

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <vector>
#endif

#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED
#include <thread>
#endif

#include <mainwindow.h>

using namespace std ;

extern vector<AP_Info> allAP ;
extern vector<thread> v_shelter_monitor ;
extern map<string,dbmSeq> devicesList2 ;
extern Traceing * slotPtr ;
extern int seqNum ;
extern int RasPI_AP_Num ;

#endif // GLOBAL_VARIABLE_H_INCLUDED
