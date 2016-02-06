//
//  lineFxns.h
//  Sideload
//
//  Created by MICHAEL RICH on 11/9/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

// These only work on standard containers
#ifndef len_ptr
#define len_ptr(x) x->size()
#endif

#ifndef len
#define len(x) x.size()
#endif

#ifndef __Sideload__lineFxns__
#define __Sideload__lineFxns__

#include <stdio.h>
#include <vector>



using namespace std;

struct point_t {
    double x;
    double y;
};

// slope "m" and y-axis intercept "b"
struct mandb_t {
    double m;
    double b;
    
    // inf_m means slope is infinite (vertical line)
    bool inf_m = false;
};

typedef vector<point_t> points_t;
typedef vector<points_t> grid_t;
typedef vector<mandb_t> mandbVec_t;

class lineFxns {
  // Instance variables?

public:
    void calcGridFromEndpoints(points_t *topLinePts, points_t *bottomLinePts, points_t *leftLinePts, points_t *rightLinePts, grid_t* result);
    
    // Private?
    mandb_t calcSlopeAndInt(point_t pt1, point_t pt2);
    point_t calc2LineIntersect(mandb_t mb1, mandb_t mb2);
    point_t calc2LineIntersect(mandb_t mb1, point_t line2_pt1);
    
    
};

#endif /* defined(__Sideload__lineFxns__) */
