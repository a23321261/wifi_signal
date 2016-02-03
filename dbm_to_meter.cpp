#include <dbm_to_meter.h>


double constant = 100.04422483423212045872489174857 ;

double turn_dbm_to_meter( double db ) {
    return pow( 10, ( 0-db+60-constant ) / 20 ) ;
    // FL(dB)=20 lg R (km) +20 lg f (GHz) + 92.44
}

void get_location( vector<RasPI_Area> vector_area ) {
    int move_threshold = 2 ;

    for ( int i = 0 ; i < allAP.size() ; i ++ ) {
        for ( map<string,dbmSeq>::iterator it = allAP.at(i).devicesList.begin() ; it != allAP.at(i).devicesList.end() ; ++it ) {
            if ( it->second.seq == seqNum ) {

                bool b_has_a_tri = false ;
                int tri_one, tri_two, tri_three, tri_four ;
                tri_one = devicesList2[ it->first ].meter[1] + devicesList2[ it->first ].meter[2] ;
                tri_two = devicesList2[ it->first ].meter[2] + devicesList2[ it->first ].meter[3] ;
                tri_three = devicesList2[ it->first ].meter[3] + devicesList2[ it->first ].meter[4] ;
                tri_four = devicesList2[ it->first ].meter[4] + devicesList2[ it->first ].meter[1] ;
                // determine which triangle is the nearest

                int a_x, a_y, b_x, b_y, c_x, c_y, ra, rb, rc ;

                if ( tri_one > tri_two && tri_one > tri_three && tri_one > tri_four ) {
                    a_x = vector_area.at(i).x[0] ;
                    a_y = vector_area.at(i).x[0] ;
                    b_x = vector_area.at(i).x[1] ;
                    b_y = vector_area.at(i).y[1] ;
                    c_x = vector_area.at(i).x[2] ;
                    c_y = vector_area.at(i).y[2] ;
                    ra = devicesList2[ it->first ].meter[0] ;
                    rb = devicesList2[ it->first ].meter[1] ;
                    rc = devicesList2[ it->first ].meter[2] ;
                    b_has_a_tri = true ;
                }
                else if ( tri_two > tri_one && tri_two > tri_three && tri_two > tri_four ) {
                    a_x = vector_area.at(i).x[0] ;
                    a_y = vector_area.at(i).x[0] ;
                    b_x = vector_area.at(i).x[2] ;
                    b_y = vector_area.at(i).y[2] ;
                    c_x = vector_area.at(i).x[3] ;
                    c_y = vector_area.at(i).y[3] ;
                    ra = devicesList2[ it->first ].meter[0] ;
                    rb = devicesList2[ it->first ].meter[2] ;
                    rc = devicesList2[ it->first ].meter[3] ;
                    b_has_a_tri = true ;
                }
                else if ( tri_three > tri_one && tri_three > tri_two && tri_three > tri_four ) {
                    a_x = vector_area.at(i).x[0] ;
                    a_y = vector_area.at(i).x[0] ;
                    b_x = vector_area.at(i).x[3] ;
                    b_y = vector_area.at(i).y[3] ;
                    c_x = vector_area.at(i).x[4] ;
                    c_y = vector_area.at(i).y[4] ;
                    ra = devicesList2[ it->first ].meter[0] ;
                    rb = devicesList2[ it->first ].meter[3] ;
                    rc = devicesList2[ it->first ].meter[4] ;
                    b_has_a_tri = true ;
                }
                else if ( tri_four > tri_one && tri_four > tri_two && tri_four > tri_three ) {
                    a_x = vector_area.at(i).x[0] ;
                    a_y = vector_area.at(i).x[0] ;
                    b_x = vector_area.at(i).x[4] ;
                    b_y = vector_area.at(i).y[4] ;
                    c_x = vector_area.at(i).x[1] ;
                    c_y = vector_area.at(i).y[1] ;
                    ra = devicesList2[ it->first ].meter[0] ;
                    rb = devicesList2[ it->first ].meter[4] ;
                    rc = devicesList2[ it->first ].meter[1] ;
                    b_has_a_tri = true ;
                }

                if ( b_has_a_tri == true ) {
                    int x, y, x_max, y_max, x_min, y_min, ender ;

                    if ( a_x >= b_x ) {
                        if ( b_x >= c_x ) {
                            x_max = a_x ;
                            x_min = c_x ;
                        } // a > b > c
                        else if ( c_x >= a_x ){
                            x_max = c_x ;
                            x_min = b_x ;
                        } //  c > a > b
                        else {
                            x_max = a_x ;
                            x_min = b_x ;
                        } // a > c > b

                    } // if a > b
                    else {
                        if ( c_x >= b_x ) {
                            x_max = c_x ;
                            x_min = a_x ;
                        } // c > b > a
                        else if ( a_x >= c_x ) {
                            x_max = b_x ;
                            x_min = c_x ;
                        } // b > a > c
                        else {
                            x_max = b_x ;
                            x_min = a_x ;
                        } // b > c > a

                    } // b > a

                    if ( a_y >= b_y ) {
                        if ( b_y >= c_y ) {
                            y_max = a_y ;
                            y_min = c_y ;
                        } // a > b > c
                        else if ( c_y >= a_y ){
                            y_max = c_y ;
                            y_min = b_y ;
                        } //  c > a > b
                        else {
                            y_max = a_y ;
                            y_min = b_y ;
                        } // a > c > b

                    } // if a > b
                    else {
                        if ( c_y >= b_y ) {
                            y_max = c_y ;
                            y_min = a_y ;
                        } // c > b > a
                        else if ( a_y >= c_y ) {
                            y_max = b_y ;
                            y_min = c_y ;
                        } // b > a > c
                        else {
                            y_max = b_y ;
                            y_min = a_y ;
                        } // b > c > a

                    } // b > a

                    x = x_max ;
                    y = y_max ; // initial
                    ender = 10000 ;
                    // ender is used to store the smallest answer
                    int x_end, y_end ;
    cout << "y_min = " << y_min << "  x_min = " << x_min << "  y_max = " << y_max << "  x_max = " << x_max << endl ;
                    while ( y >= y_min ) {
                        int tempa, tempb, tempc, ans ;
                        tempa = ra-sqrt( (x-a_x)*(x-a_x)+(y-a_y)*(y-a_y) ) ;
                        tempb = rb-sqrt( (x-b_x)*(x-b_x)+(y-b_y)*(y-b_y) ) ;
                        tempc = rc-sqrt( (x-c_x)*(x-c_x)+(y-c_y)*(y-c_y) ) ;
                        ans = tempa + tempb + tempc ;
                        if ( tempa >= 0 && tempb >= 0 && tempc >= 0 && ans < ender ) {
                            ender = ans ;
                            x_end = x ;
                            y_end = y ;
                        }

                        --x ;
                        if ( x < 0 ) {
                            --y ;
                            x = x_max ;
                        }

                    } // calculate all of the answer and choose the smallest answer

                    if ( ( devicesList2[ it->first ].location[0] == 0 && devicesList2[ it->first ].location[1] == 0 )
                         || ( move_threshold > devicesList2[ it->first ].location[0] - x_end &&
                              move_threshold > devicesList2[ it->first ].location[1] - y_end ) ) {
                        devicesList2[ it->first ].location[0] = x_end ; // the last location
                        devicesList2[ it->first ].location[1] = y_end ;
                    } // if under the threshold or x y are zero

                } // if b_has_a_tri == true

            } // if seq is right

        } // for iterator

    } // for allAP

}
