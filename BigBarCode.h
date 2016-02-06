//
//  BigBarCode.h
//  Sideload
//
//  Created by MICHAEL RICH on 12/1/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#ifndef __Sideload__BigBarCode__
#define __Sideload__BigBarCode__

#include <stdio.h>
#include <vector>
#include <string>

#include "image.h"
#include "lineFxns.h"

using namespace std;

typedef unsigned char BBCByte_t;
typedef vector<BBCByte_t> BBCByteArr_t;
typedef vector<BBCByteArr_t> BBCRowArray_t;

typedef vector<string> BBCFileArr_t;

typedef vector<int> BBCIntArr_t;

class BigBarCode {
private:
    // makeBBC variables & functions
    int BBC_maxBytesPerLine;
    int BBC_maxLinesPerPage;
    int BBC_extraTimingLineInterval; // Needs to be odd to start with a nice black timing mark
    
    
    colorVal_t on_pixel, off_pixel;
    
    pxData_t BBCPxData;     // All BBC work is appended to this as it goes
    Image BBCimage;         // All decode fxn use this image
    int rows;
    
    void generateBBC(BBCRowArray_t* dataRows, string filename);
    void generateTimingLine(int width);
    void generateWhiteRow(int width);
    void startRow(colorVal_t timingMark);
    void endRow(colorVal_t timingMark);
    void encodeByte(BBCByte_t byte);

    // decodeBBC
    colorVal_t threshold;   // Threshold of what is an on_pixel, what is an off_pixel
    int masksize;           // How big of a square do we use when looking for timing marks
    int wigglesize;         // How much do I move the mask around to find the best mask
    
    BBCByte_t decodeByte(string bits);
    // void decodeBytes(BBCByteArr_t* data);
    void boxDecode(grid_t* dataGrid);
    void decodeData(BBCByteArr_t* dataout, grid_t* dataGrid);
    point_t findRootIndex();
    void findLeftEdgeStats(BBCIntArr_t* edgeCount, BBCIntArr_t* edgeY);
    point_t findX0Y0(BBCIntArr_t* edgeCount, BBCIntArr_t* edgeY);
    int lowestIndexOfVal(BBCIntArr_t* list, int toFind);
    point_t wiggleFit(point_t x0y0);
    
    void findHorizCenters(points_t* centers, point_t startPoint);
    double findNextEdgeY(point_t xcyc);
    void findVertCenters(points_t* centers, point_t startPoint);
    double findNextEdgeX(point_t xcyc);
    void interpCenters(points_t* centers);
    //double round(float x);
    
    void markpts(points_t *points);
    
    // Improved BBC
    int makeImprovedBBC(BBCByteArr_t *data, string filename, BBCFileArr_t* createdFiles);
    void generateImpBBC(BBCRowArray_t* dataRows, string filename);
    
    int decodeImpBBC(string filename, BBCByteArr_t* data);
    void boxDecodeImp(grid_t* topSegmentGrid, grid_t* middleSegmentGrid, grid_t* bottomSegmentGrid);
    
public:
    BigBarCode(int maxBytesPerLine = 88, int maxLines = 951);
    
    int makeBigBarCode(string filename, BBCFileArr_t* createdFiles);
    int makeBigBarCode(string* data, string rootFilename, BBCFileArr_t* createdFiles);
    int makeBigBarCode(BBCByteArr_t *data, string filename, BBCFileArr_t* createdFiles);
    int decodeABBC(string filename, BBCByteArr_t* data);
    int decodeBigBarCode(BBCFileArr_t* BBCSourceFiles, long dataLength, BBCByteArr_t* data);
    
    void setThreshold(colorVal_t thrshld = 150);
    void setMasksize(int msksize = 7);
    void setWigglesize(int wgglsze = 3);
    void readFileIntoVector(string filename, BBCByteArr_t* data);
    void writeVectorToFile(BBCByteArr_t* data, string filename);
};

#endif /* defined(__Sideload__BigBarCode__) */
