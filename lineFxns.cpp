//
//  lineFxns.cpp
//  Sideload
//
//  Created by MICHAEL RICH on 11/9/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include "lineFxns.h"

#include <stdexcept>
#include <math.h>

void lineFxns::calcGridFromEndpoints(points_t *topLinePts, points_t *bottomLinePts, points_t *leftLinePts, points_t *rightLinePts, grid_t* result) {
    // This one rounds the results, intended for accessing whole integer pixel locations.
   
    // Check for consistency
    if (len_ptr(topLinePts) != len_ptr(bottomLinePts)) {
        throw runtime_error("calcGrid: top and bottom pt lists are different lengths!");
    }
    
    if (len_ptr(rightLinePts) != len_ptr(leftLinePts)) {
        throw runtime_error("calcGrid: left and right pt lists are different lengths!");
    }
    
    // get to work
    result->clear();
    
    mandbVec_t mbX;
    for (int i = 0; i < len_ptr(topLinePts); i++) {
        mandb_t mb1;
        mb1 = calcSlopeAndInt((*topLinePts)[i], (*bottomLinePts)[i]);
        mbX.push_back(mb1);
    }
    
    mandbVec_t mbY;
    for (int i = 0; i < len_ptr(leftLinePts); i++) {
        mandb_t mb1;
        mb1 = calcSlopeAndInt((*leftLinePts)[i], (*rightLinePts)[i]);
        mbY.push_back(mb1);
    }
    
    // Calc the results
    for (int j = 0; j < len(mbY); j++) {
        points_t gridrow;
        for (int i = 0; i < len(mbX); i++) {
            point_t xy;
            if (mbX[i].inf_m) {
                xy = calc2LineIntersect(mbY[j], (*topLinePts)[i]);
            } else {
                xy = calc2LineIntersect(mbX[i], mbY[j]);
            }
            xy.x = round(xy.x);
            xy.y = round(xy.y);
            gridrow.push_back(xy);
        }
        result->push_back(gridrow);
    }
    
}

// Private?
mandb_t lineFxns::calcSlopeAndInt(point_t pt1, point_t pt2) {
    mandb_t result;
    
    double x1,y1,x2,y2;
    x1 = pt1.x;
    y1 = pt1.y;
    x2 = pt2.x;
    y2 = pt2.y;
    
    if (x1 == x2) {
        result.inf_m = true;
        return result;
    }
    
    result.m = (y2 - y1)/(x2 - x1);
    result.b = (y1 - result.m*x1);
    
    return result;
}

// Used when both lines have a finite slope
point_t lineFxns::calc2LineIntersect(mandb_t mb1, mandb_t mb2) {
    point_t result;
    double m1,m2,b1,b2;
    m1 = mb1.m;
    b1 = mb1.b;
    m2 = mb2.m;
    b2 = mb2.b;
    
    result.x = (b2-b1)/(m1-m2);
    result.y = (m1*result.x+b1);
    
    return result;
}

// used when one line is vertical
point_t lineFxns::calc2LineIntersect(mandb_t mb1, point_t line2_pt1) {
    point_t result;
    double m1,b1,x2;
    m1 = mb1.m;
    b1 = mb1.b;
    x2 = line2_pt1.x;
    
    result.x = x2;
    result.y = m1*x2+b1;
    
    return result;
}


