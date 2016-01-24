#ifndef MONITOR_MANAGE_H_INCLUDED
#define MONITOR_MANAGE_H_INCLUDED

#include <Define_class.hpp>
#include <string>
#include <iostream>
#include <libssh/libssh.h>
#include <global_variable.hpp>
#include <SSH_IO.hpp>

using namespace std ;

void monitor_manager( AP_Info* thisArea, string targetMAC ) ;
char * GetMonitorData( ssh_channel channel ) ;

#endif // MONITOR_MANAGE_H_INCLUDED
