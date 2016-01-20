#ifndef SSH_IO_H_INCLUDED
#define SSH_IO_H_INCLUDED

#include <Define_class.cpp>
#include <thread>
#include <vector>
#include <iostream>
#include <string>



using namespace std ;

int verify_knownhost( ssh_session session ) ;
bool CreateSSH_Link( ssh_session & targetSession, string targetIP ) ;
void CreateAP_Link( RasPI_Area thisArea, vector<thread> & threads ) ;

#endif // SSH_IO_H_INCLUDED
