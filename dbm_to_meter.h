#ifndef DBM_TO_METER_H_INCLUDED
#define DBM_TO_METER_H_INCLUDED

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED
#include <vector>
#endif

#ifndef MATH_H_INCLUDE
#define MATH_H_INCLUDE
#include <math.h>
#endif

#ifndef IOSTREAM_H_INCLUDED
#define IOSTREAM_H_INCLUDED

#include <iostream>

#endif // IOSTREAM_H_INCLUDED

#include <Define_class.h>
#include <global_variable.h>

using namespace std ;

double turn_dbm_to_meter( double db ) ;
void get_location( vector<RasPI_Area> vector_area ) ;

#endif // DBM_TO_METER_H_INCLUDED
