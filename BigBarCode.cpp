//
//  BigBarCode.cpp
//  Sideload
//
//  Created by MICHAEL RICH on 12/1/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include "BigBarCode.h"

#include <fstream>
#include <algorithm>
#include <math.h>

// DEBUG
#include <iostream>

BigBarCode::BigBarCode(int maxBytesPerLine, int maxLines) {
    BBC_maxBytesPerLine = maxBytesPerLine;
    BBC_maxLinesPerPage = maxLines;
    BBC_extraTimingLineInterval = 341;
    
    off_pixel = 0;
    on_pixel = 255;
    
    setMasksize();
    setThreshold();
    setWigglesize();
    
    BBCPxData.clear();
}

void BigBarCode::readFileIntoVector(string filename, BBCByteArr_t* data) {
    // code from: http://stackoverflow.com/questions/15138353/reading-the-binary-file-into-the-vector-of-unsigned-chars
    
    // open the file:
    std::ifstream file(filename, std::ios::binary);
    
    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);
    
    // get its size:
    std::streampos fileSize;
    
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // reserve capacity
    data->reserve(fileSize);
    
    // read the data:
    data->insert(data->begin(), std::istream_iterator<BBCByte_t>(file), std::istream_iterator<BBCByte_t>());
    
}

void BigBarCode::writeVectorToFile(BBCByteArr_t* data, string filename) {
    // Open the file
    ofstream file(filename, std::ios::binary);
    
    //save it
    ostream_iterator<BBCByte_t> outit(file);
    copy(data->begin(), data->end(), outit);
}

void BigBarCode::generateImpBBC(BBCRowArray_t* dataRows, string filename) {
    // Calculate BBC px widths
    int fullPxWidth;
    // a row is: TimeMark Deadspace Datainbytes*8px Deadspace deadspace Timemark
    // = datainbytes*8 + 5
    fullPxWidth = BBC_maxBytesPerLine*8 + 5;
    int dataPxWidth = BBC_maxBytesPerLine*8;
    
    // Start making BBC
    BBCPxData.clear();
    colorVal_t timingMark = off_pixel;
    generateTimingLine(fullPxWidth);
    generateWhiteRow(fullPxWidth);
    
    
    int numRows = 2;
    int extraTimingLineInterval = BBC_extraTimingLineInterval; // Needs to be odd to start with a nice black timing mark
    int dataRowCount = 0;
    
    for (int rowindex = 0; rowindex < len_ptr(dataRows); rowindex++) {
        // Time for new timing line?
        if (dataRowCount == extraTimingLineInterval) {
            generateWhiteRow(fullPxWidth);
            generateTimingLine(fullPxWidth);
            generateWhiteRow(fullPxWidth);
            numRows += 3;
            dataRowCount = 0;
            timingMark = off_pixel;
        }
        
        startRow(timingMark);
        BBCByteArr_t *thisrow = &((*dataRows)[rowindex]);
        
        // Check to add padding
        int padding = BBC_maxBytesPerLine - len_ptr(thisrow);
        if (padding) {
            thisrow->insert(thisrow->end(), padding, 255);
        }
        
        for (int byteindex = 0; byteindex < len_ptr(thisrow); byteindex++) {
            encodeByte((*thisrow)[byteindex]);
        }
        
        endRow(timingMark);
        timingMark ^= 255;
        numRows++;
        dataRowCount++;
    }
    
    // If we end on an even row, we need to add an extra row
    if (!(numRows % 2)) {
        startRow(timingMark);
        generateWhiteRow(dataPxWidth);
        endRow(timingMark);
        numRows++;
    }
    
    // Add deadspace and timing line
    generateWhiteRow(fullPxWidth);
    generateTimingLine(fullPxWidth);
    numRows += 2;
    
    // Make the pic!
    Image pic(fullPxWidth, numRows, &BBCPxData);
    pic.save(filename);
}



void BigBarCode::generateBBC(BBCRowArray_t* dataRows, string filename) {
    // Calculate BBC px widths
    int fullPxWidth;
    // a row is: TimeMark Deadspace Datainbytes*8px Deadspace deadspace Timemark
    // = datainbytes*8 + 5
    fullPxWidth = BBC_maxBytesPerLine*8 + 5;
    int dataPxWidth = BBC_maxBytesPerLine*8;
    
    // Start making BBC
    BBCPxData.clear();
    colorVal_t timingMark = off_pixel;
    generateTimingLine(fullPxWidth);
    generateWhiteRow(fullPxWidth);
    
    
    int numRows = 2;
    
    for (int rowindex = 0; rowindex < len_ptr(dataRows); rowindex++) {
        startRow(timingMark);
        BBCByteArr_t *thisrow = &((*dataRows)[rowindex]);
        
        // Check to add padding
        int padding = BBC_maxBytesPerLine - len_ptr(thisrow);
        if (padding) {
            thisrow->insert(thisrow->end(), padding, 255);
        }
        
        for (int byteindex = 0; byteindex < len_ptr(thisrow); byteindex++) {
            encodeByte((*thisrow)[byteindex]);
        }
        
        endRow(timingMark);
        timingMark ^= 255;
        numRows++;
    }
    
    // If we end on an even row, we need to add an extra row
    if (!(numRows % 2)) {
        startRow(timingMark);
        generateWhiteRow(dataPxWidth);
        endRow(timingMark);
        numRows++;
    }
    
    // Add deadspace and timing line
    generateWhiteRow(fullPxWidth);
    generateTimingLine(fullPxWidth);
    numRows += 2;
    
    // Make the pic!
    Image pic(fullPxWidth, numRows, &BBCPxData);
    pic.save(filename);
}


void BigBarCode::generateTimingLine(int width) {
    // Timing line is "off (black) on off on..." to width pixels
    colorVal_t thispx = off_pixel;
    for (int i = 0; i < width; i++) {
        BBCPxData.push_back(thispx);
        thispx ^= 255;
    }
}

void BigBarCode::generateWhiteRow(int width) {
    BBCPxData.insert(BBCPxData.end(), width, on_pixel);
}

void BigBarCode::startRow(colorVal_t timingMark) {
    BBCPxData.push_back(timingMark);
    BBCPxData.push_back(on_pixel);
}
void BigBarCode::endRow(colorVal_t timingMark) {
    BBCPxData.push_back(on_pixel);
    BBCPxData.push_back(on_pixel);
    BBCPxData.push_back(timingMark);
}


void BigBarCode::encodeByte(BBCByte_t byte) {
    // Encodes into the px data
    BBCByte_t bitmask = 1;
    for (int i = 0; i < 8; i++) {
        if (byte & bitmask) {
            // Then this byte is on
            BBCPxData.push_back(on_pixel);
        } else {
            // It's off
            BBCPxData.push_back(off_pixel);
        }
        bitmask = bitmask << 1;
    }
}

int BigBarCode::makeBigBarCode(string rootFileName, BBCFileArr_t* createdFiles) {
    // load data
    BBCByteArr_t data;
    readFileIntoVector(rootFileName, &data);
    
    return makeBigBarCode(&data, rootFileName, createdFiles);
}

int BigBarCode::makeBigBarCode(string* data, string rootFilename, BBCFileArr_t* createdFiles) {
    BBCByteArr_t datain;
    datain.insert(datain.begin(), data->begin(), data->end());
    
    return makeBigBarCode(&datain, rootFilename, createdFiles);
}

int BigBarCode::makeImprovedBBC(BBCByteArr_t *data, string rootFileName, BBCFileArr_t* createdFiles) {
    // Adds a new timing line every 340 rows (with deadspace on two sides)
    BBCByteArr_t aRow;
    BBCRowArray_t allRows;
    
    int maxRowsData;
    // maxRowsdata is page size in px rows - (timing line, deadspace) x2 - (ds, tl, ds) x2
    maxRowsData = BBC_maxLinesPerPage - 4 - 6;
    
    int maxBytesData = maxRowsData*BBC_maxBytesPerLine;
    
    // How many BBC will I make?
    int totalBBC;
    if (len_ptr(data) % maxBytesData) {
        totalBBC = (len_ptr(data) / maxBytesData) + 1;
    } else {
        totalBBC = (len_ptr(data) / maxBytesData);
    }
    
    int BBCCount = 1;
    for (int i = 0; i < len_ptr(data); i++) {
        aRow.push_back((*data)[i]);
        if (len(aRow) == BBC_maxBytesPerLine) {
            allRows.push_back(aRow);
            aRow.clear();
        }
        if (len(allRows) == maxRowsData) {
            // Time to make a BBC
            string filename = rootFileName + "." + to_string(BBCCount) + "of" + to_string(totalBBC) + ".bmp";
            generateImpBBC(&allRows, filename);
            createdFiles->push_back(filename);
            BBCCount++;
            
            allRows.clear();
            aRow.clear();
        }
    }
    
    // Any residual data?
    if (len(aRow)) {
        allRows.push_back(aRow);
    }
    
    if (len(allRows)) {
        string filename = rootFileName + "." + to_string(BBCCount) + "of" + to_string(totalBBC) + ".bmp";
        generateImpBBC(&allRows, filename);
        createdFiles->push_back(filename);
    }
    
    return len_ptr(createdFiles);

}


int BigBarCode::makeBigBarCode(BBCByteArr_t *data, string rootFileName, BBCFileArr_t* createdFiles) {
    
    //return makeImprovedBBC(data, rootFileName, createdFiles);
    
    BBCByteArr_t aRow;
    BBCRowArray_t allRows;
    
    int maxRowsData;
    // maxRowsdata is page size in px rows - (timing line, deadspace) x2
    maxRowsData = BBC_maxLinesPerPage - 4;
    
    int maxBytesData = maxRowsData*BBC_maxBytesPerLine;
    
    // How many BBC will I make?
    int totalBBC;
    if (len_ptr(data) % maxBytesData) {
        totalBBC = (len_ptr(data) / maxBytesData) + 1;
    } else {
        totalBBC = (len_ptr(data) / maxBytesData);
    }
    
    int BBCCount = 1;
    for (int i = 0; i < len_ptr(data); i++) {
        aRow.push_back((*data)[i]);
        if (len(aRow) == BBC_maxBytesPerLine) {
            allRows.push_back(aRow);
            aRow.clear();
        }
        if (len(allRows) == maxRowsData) {
            // Time to make a BBC
            string filename = rootFileName + "." + to_string(BBCCount) + "of" + to_string(totalBBC) + ".bmp";
            generateBBC(&allRows, filename);
            createdFiles->push_back(filename);
            BBCCount++;
            
            allRows.clear();
            aRow.clear();
        }
    }
    
    // Any residual data?
    if (len(aRow)) {
        allRows.push_back(aRow);
    }
    
    if (len(allRows)) {
        string filename = rootFileName + "." + to_string(BBCCount) + "of" + to_string(totalBBC) + ".bmp";
        generateBBC(&allRows, filename);
        createdFiles->push_back(filename);
    }
    
    return len_ptr(createdFiles);
}
    
// decodeBBC
BBCByte_t BigBarCode::decodeByte(string bits) {
    // expects an 8 place string of "0101010"
    BBCByte_t byte = 0;
    BBCByte_t bitmask = 1;
    for (int i = 0; i < len(bits); i++) {
        if (bits[i] == '1') byte ^= bitmask;
        bitmask = bitmask << 1;
    }
    
    return byte;
}

void BigBarCode::markpts(points_t *points) {
    for (int i = 0; i < len_ptr(points); i++) {
        point_t pnt = (*points)[i];
        if (BBCimage.getpixel(pnt) < threshold) {
            BBCimage.putpixel(pnt, 255);
        } else {
            BBCimage.putpixel(pnt, 0);
        }
    }
}

void BigBarCode::boxDecode(grid_t* dataGrid) {
    // This analyzes the image stored in BBCImage, finds all of the timing marks,
    // and calculates the image locations for each data element in the BBC
    point_t xbyb = findRootIndex();
    
    points_t topCenters, bottomCenters, rightCenters, leftCenters;
    findHorizCenters(&topCenters, xbyb);
    interpCenters(&topCenters);
    
    
    findVertCenters(&leftCenters, xbyb);
    interpCenters(&leftCenters);
    
    //in theory, the right centers should be locatable by using
    //findVertCenters and using the last top timing mark as the xbyb
    //but, due to way I wrote this, need to subtract the offsets first
    int centerOffset = masksize/2;
    point_t xy = topCenters[len(topCenters) - 1];
    xy.x -= centerOffset;
    xy.y -= centerOffset;
    findVertCenters(&rightCenters, xy);
    interpCenters(&rightCenters);
    // bottom centers should be the same as topCenters with last leftCenter as xbyb
    xy = leftCenters[len(leftCenters) - 1];
    xy.x -= centerOffset;
    xy.y -= centerOffset;
    findHorizCenters(&bottomCenters, xy);
    interpCenters(&bottomCenters);
    
    
    // DEBUG
    //markpts(&bottomCenters);
   // BBCimage.save("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin-2of2.sld-scanned.gif-BBCmarked.bmp");
    
    
    // Need to remove un-needed timing mark locations.
    // These are the Tm & DS on left and top, the Tm & 2 DS at right, the Tm & DS on bottom
    points_t topCenters_corrected, bottomCenters_corrected, rightCenters_corrected, leftCenters_corrected;
    topCenters_corrected.assign((topCenters.begin()+2), (topCenters.end()-3));
    bottomCenters_corrected.assign((bottomCenters.begin()+2), (bottomCenters.end()-3));
    rightCenters_corrected.assign((rightCenters.begin()+2), (rightCenters.end()-2));
    leftCenters_corrected.assign((leftCenters.begin()+2), (leftCenters.end()-2));
    
    lineFxns linefxn;
    linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  dataGrid);
}

void BigBarCode::decodeData(BBCByteArr_t* dataout, grid_t* dataGrid) {
    // This goes through the BBCImage using the coords in dataGrid and build 8 char "1010101" strings which are converted
    // to bytes and saved.  I'm assuming there are whole multiples of 8 bits in each line of the data grid, which
    // makeBigBarCode should have done to begin with.
    
    dataout->clear();
    for (int row = 0; row < len_ptr(dataGrid); row++) {
        points_t *thisRow = &(*dataGrid)[row];
        string bits;
        for (int index = 0; index < len_ptr(thisRow); index++) {
            colorVal_t thisPixel = BBCimage.getpixel((*thisRow)[index]);
            if (thisPixel >= threshold) {
                bits += '1';
            } else {
                bits += '0';
            }
            if (len(bits) == 8) {
                dataout->push_back(decodeByte(bits));
                bits.clear();
            }
        }
        // DEBUG
        //markpts(thisRow);
        // DEBUG
        //BBCimage.save("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin-2of2.sld-scanned.gif-BBCmarked.bmp");
    }
    
    
}

point_t BigBarCode::findRootIndex() {
    BBCIntArr_t edgeCount, edgeY;
    findLeftEdgeStats(&edgeCount, &edgeY);
    point_t x0y0 = findX0Y0(&edgeCount, &edgeY);
    point_t xbyb = wiggleFit(x0y0);
    return xbyb;
}

void BigBarCode::findLeftEdgeStats(BBCIntArr_t* edgeCount, BBCIntArr_t* edgeY) {
    // Calculates the number of times the black edge is detected for a given x value
    // stores the location of the black edge for a given value of y
    edgeCount->clear();
    edgeY->clear();
    
    // initialize values
    int width = BBCimage.width();
    int height = BBCimage.height();
    
    edgeCount->insert(edgeCount->begin(), width, 0);
    edgeY->insert(edgeY->begin(), height, 0);
    
    //bool foundFirstBlack = false;
    bool foundTopRow = false;
    bool rowAllWhite = true;
    double firstBlack = width;
    
    for (double y = 0; y < height; y++) {
        rowAllWhite = true;
        int blackCount = 0;
        for (double x = 0; x < width; x++) {
            // Look for first pix value that's black
            if (BBCimage.getpixel({x,y}) <= threshold) {
                firstBlack = min(firstBlack, x);
                if (!blackCount) {
                    //then this is the first black this line
                    //if this position is way past the most left black
                    //then we actually found the dead space
                    //a scan with vertical drift will never have a full row empty
                    if (x < (firstBlack+(2*masksize))) {
                        (*edgeCount)[round(x)]++;
                        (*edgeY)[round(y)] = round(x);
                        rowAllWhite = false;
                    } else {
                        // picked up a drifted pixel, and this is actually the deadspace
                        break;
                    }
                }
                blackCount++;
                if (blackCount > 5) {
                    foundTopRow = true;
                    break;
                }
                
            }
        }
        if (foundTopRow && rowAllWhite) {
            // found the deadspace row
            break;
        }
    }
    // edgeCount and edgeY have all the needed data
}

point_t BigBarCode::findX0Y0(BBCIntArr_t* edgeCount, BBCIntArr_t* edgeY) {
    // Find the first location of the largest count of edges
    double x0 = max_element(edgeCount->begin(), edgeCount->end()) - edgeCount->begin();
    // find first occurence of x0 in edgeY
    double y0 = find(edgeY->begin(), edgeY->end(), x0) - edgeY->begin();
    
    //cout << "x0y0: " << x0 << ", " << y0 << "\n";
    
    return {x0, y0};
}

point_t BigBarCode::wiggleFit(point_t x0y0) {
    // wiggle fit is used to find the center of timing marks.
    // It starts at point x0y0, draws a box masksize, and moves the box around in all 4 directions
    //  up to wigglesize to find the location with the most black pixels
    // returns the (x,y) of best wiggle fit
    double x0 = x0y0.x;
    double y0 = x0y0.y;
    
    // Bounds check
    if ((x0 - wigglesize) < 0) {
        throw runtime_error("x0 too small, no room to wiggle.");
    }
    if ((y0 - wigglesize) < 0) {
        throw runtime_error("y0 too small, no room to wiggle.");
    }
    if ((x0 + wigglesize + masksize) > BBCimage.width()) {
        throw runtime_error("x0 too big, no room to wiggle.");
    }
    if ((y0 + wigglesize + masksize) > BBCimage.height()) {
        throw runtime_error("y0 too big, no room to wiggle.");
    }
    
    // Build wiggle delta matrix.  This is the change in x and y used to anchor the mask
    points_t wiggleDelta;
    for (double y = -1*wigglesize; y <= wigglesize; y++) {
        for (double x = -1*wigglesize; x <= wigglesize; x++) {
            wiggleDelta.push_back({x,y});
        }
    }
    
    // Calculate maskvalues for each wiggle
    BBCIntArr_t maskValues;
    for (int i = 0; i < len(wiggleDelta); i++) {
        double xw = x0 + wiggleDelta[i].x;
        double yw = y0 + wiggleDelta[i].y;
        int currentValue = 0;
        
        for (double x = xw; x < (xw+masksize); x++) {
            for (double y = yw; y < (yw+masksize); y++) {
                currentValue += (BBCimage.getpixel({x, y}) < threshold);
            }
        }
        maskValues.push_back(currentValue);
    }
    
    // find the index to the highest wiggle value
    int bestMaskWiggleIndex = max_element(maskValues.begin(), maskValues.end()) - maskValues.begin();
    
    // return the locaiton of the best mask
    double xb = round(x0 + wiggleDelta[bestMaskWiggleIndex].x);
    double yb = round(y0 + wiggleDelta[bestMaskWiggleIndex].y);

    return {xb,yb};
}

void BigBarCode::findHorizCenters(points_t* centers, point_t startPoint) {
    centers->clear();
    bool foundAll = false;
    point_t xylast = startPoint;
    
    int centerOffset = (masksize/2);
    centers->push_back({startPoint.x + centerOffset, startPoint.y + centerOffset});
    while (!foundAll) {
        double nextx = findNextEdgeX({xylast.x+centerOffset, xylast.y+centerOffset});
        if (nextx == BBCimage.width()) {
            foundAll = true;
        } else {
            xylast = wiggleFit({nextx, xylast.y});
            centers->push_back({xylast.x+centerOffset, xylast.y+centerOffset});
        }
    }
}

double BigBarCode::findNextEdgeX(point_t xcyc) {
    // go directly right from center of last square
    // wait till black done, then white.  First black is the start
    double xc = xcyc.x;
    double yc = xcyc.y;
    bool foundWhite = false;
    
    for (double x = xc; x < BBCimage.width(); x++) {
        if (BBCimage.getpixel({x,yc}) >= threshold) {
            foundWhite = true;
        } else {
            if (foundWhite) {
                return round(x);
            }
        }
    }
    return BBCimage.width();
}

double BigBarCode::findNextEdgeY(point_t xcyc) {
    // go directly down from center of last square
    // wait till black done, then white.  First black is the start
    double xc = xcyc.x;
    double yc = xcyc.y;
    bool foundWhite = false;
    
    for (double y = yc; y < BBCimage.height(); y++) {
        if (BBCimage.getpixel({xc,y}) >= threshold) {
            foundWhite = true;
        } else {
            if (foundWhite) {
                return round(y);
            }
        }
    }
    return BBCimage.height();
}

void BigBarCode::findVertCenters(points_t* centers, point_t startPoint) {
    centers->clear();
    bool foundAll = false;
    point_t xylast = startPoint;
    
    int centerOffset = (masksize/2);
    centers->push_back({startPoint.x + centerOffset, startPoint.y + centerOffset});
    while (!foundAll) {
        double nexty = findNextEdgeY({xylast.x+centerOffset, xylast.y+centerOffset});
        if (nexty == BBCimage.height()) {
            foundAll = true;
        } else {
            xylast = wiggleFit({xylast.x, nexty});
            centers->push_back({xylast.x+centerOffset, xylast.y+centerOffset});
        }
    }
}

//double BigBarCode::round(float x) {
//    return (double)(x + 0.5);
//}

void BigBarCode::interpCenters(points_t* centers) {
    points_t allcenters;
    point_t lastCenter = {0,0};
    
    for (int i = 0; i < len_ptr(centers); i++) {
        point_t center = (*centers)[i];
        if (i > 0) {
            double x1 = lastCenter.x;
            double y1 = lastCenter.y;
            double x2 = center.x;
            double y2 = center.y;
            double xnew = round((x1+x2)/2.0);
            double ynew = round((y1+y2)/2.0);
            allcenters.push_back({xnew, ynew});
            allcenters.push_back(center);
            lastCenter = center;
        } else {
            allcenters.push_back(center);
            lastCenter = center;
        }
    }
    
    centers->assign(allcenters.begin(), allcenters.end());
}

void BigBarCode::boxDecodeImp(grid_t* dataGrid, grid_t* middleSegmentGrid, grid_t* bottomSegmentGrid) {
    int extraTimingLineInterval = BBC_extraTimingLineInterval;
    dataGrid->clear();
    
    // Get overall endpoints
    point_t xbyb = findRootIndex();
    
    points_t topCenters, bottomCenters, rightCenters, leftCenters;
    findHorizCenters(&topCenters, xbyb);
    interpCenters(&topCenters);
    
    
    findVertCenters(&leftCenters, xbyb);
    interpCenters(&leftCenters);
    
    //in theory, the right centers should be locatable by using
    //findVertCenters and using the last top timing mark as the xbyb
    //but, due to way I wrote this, need to subtract the offsets first
    int centerOffset = masksize/2;
    point_t xy = topCenters[len(topCenters) - 1];
    xy.x -= centerOffset;
    xy.y -= centerOffset;
    findVertCenters(&rightCenters, xy);
    interpCenters(&rightCenters);
    // bottom centers should be the same as topCenters with last leftCenter as xbyb
    xy = leftCenters[len(leftCenters) - 1];
    xy.x -= centerOffset;
    xy.y -= centerOffset;
    findHorizCenters(&bottomCenters, xy);
    interpCenters(&bottomCenters);
    
    
    // Look for extra timing lines
    int firstTimingLineLocation = extraTimingLineInterval + 3; //top timing line + deadspace + deadspace before the new timing line
    int secondTimingLineLocation = firstTimingLineLocation*2;
    
    points_t topCenters_corrected, bottomCenters_corrected, rightCenters_corrected, leftCenters_corrected;
    
    if (len(leftCenters) > firstTimingLineLocation) {
        // Get grid between the top and first timing line
        points_t timingline1;
        point_t timing_xy = leftCenters[firstTimingLineLocation];
        timing_xy.x -= centerOffset;
        timing_xy.y -= centerOffset;
        findHorizCenters(&timingline1, timing_xy);
        interpCenters(&timingline1);
        topCenters_corrected.assign(topCenters.begin()+2, topCenters.end()-3);
        bottomCenters_corrected.assign(timingline1.begin()+2, timingline1.end()-3);
        rightCenters_corrected.assign(rightCenters.begin()+2, rightCenters.begin()+(firstTimingLineLocation-1));
        leftCenters_corrected.assign(leftCenters.begin()+2, leftCenters.begin()+(firstTimingLineLocation-1));
        grid_t grid;
        lineFxns linefxn;
        linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  &grid);
        
        //DEBUG
        // Did I get the right points?
        //BBCByteArr_t dummy;
        //decodeData(&dummy, &grid);
        // Yes
        
        // Append to dataGrid
        dataGrid->insert(dataGrid->end(), grid.begin(), grid.end());
        if (len(leftCenters) > secondTimingLineLocation) {
            // Calc more grid points b/n first and second timing line, and then finish by using the bottom timing line
            points_t timingline2;
            timing_xy = leftCenters[secondTimingLineLocation];
            timing_xy.x -= centerOffset;
            timing_xy.y -= centerOffset;
            findHorizCenters(&timingline2, timing_xy);
            interpCenters(&timingline2);
            
            // Tops were last time's bottoms
            topCenters_corrected.assign(bottomCenters_corrected.begin(), bottomCenters_corrected.end());
            bottomCenters_corrected.assign(timingline2.begin()+2, timingline2.end()-3);
            rightCenters_corrected.assign(rightCenters.begin()+(firstTimingLineLocation+2), rightCenters.begin()+(secondTimingLineLocation-1));
            leftCenters_corrected.assign(leftCenters.begin()+(firstTimingLineLocation+2), leftCenters.begin()+(secondTimingLineLocation-1));
            grid.clear();
            linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  &grid);
            //DEBUG
            // Did I get the right points?
            //BBCByteArr_t dummy;
            //decodeData(&dummy, &grid);
            // Yes
            // Append to dataGrid
            dataGrid->insert(dataGrid->end(), grid.begin(), grid.end());
            
            // Now do the bottom
            // Tops were last time's bottoms
            topCenters_corrected.assign(bottomCenters_corrected.begin(), bottomCenters_corrected.end());
            bottomCenters_corrected.assign(bottomCenters.begin()+2, bottomCenters.end()-3);
            rightCenters_corrected.assign(rightCenters.begin()+(secondTimingLineLocation+2), rightCenters.end() - 2);
            leftCenters_corrected.assign(leftCenters.begin()+(secondTimingLineLocation+2), leftCenters.end()-2);
            grid.clear();
            linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  &grid);
            //DEBUG
            // Did I get the right points?
            //BBCByteArr_t dummy;
            //decodeData(&dummy, &grid);
            // Yes
            // Append to dataGrid
            dataGrid->insert(dataGrid->end(), grid.begin(), grid.end());
            
        } else {
            // Calc more grid points b/n the first and bottom timing lines
            // Tops were last time's bottoms
            topCenters_corrected.assign(bottomCenters_corrected.begin(), bottomCenters_corrected.end());
            bottomCenters_corrected.assign(bottomCenters.begin()+2, bottomCenters.end()-3);
            rightCenters_corrected.assign(rightCenters.begin()+(firstTimingLineLocation+2), rightCenters.end() - 2);
            leftCenters_corrected.assign(leftCenters.begin()+(firstTimingLineLocation+2), leftCenters.end()-2);
            grid.clear();
            linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  &grid);
            dataGrid->insert(dataGrid->end(), grid.begin(), grid.end());
        }
    } else {
        // Calc grid points between the top and bottom timing lines
        points_t topCenters_corrected, bottomCenters_corrected, rightCenters_corrected, leftCenters_corrected;
        topCenters_corrected.assign((topCenters.begin()+2), (topCenters.end()-3));
        bottomCenters_corrected.assign((bottomCenters.begin()+2), (bottomCenters.end()-3));
        rightCenters_corrected.assign((rightCenters.begin()+2), (rightCenters.end()-2));
        leftCenters_corrected.assign((leftCenters.begin()+2), (leftCenters.end()-2));
        
        lineFxns linefxn;
        linefxn.calcGridFromEndpoints(&topCenters_corrected, &bottomCenters_corrected, &leftCenters_corrected, &rightCenters_corrected,  dataGrid);
    }

//    // DEBUG
//    // Lets see if the extra lines are in the right place
//    points_t toMark;
//    toMark.push_back(leftCenters[extraTimingLineInterval+3]);
//    toMark.push_back(leftCenters[extraTimingLineInterval*2+6]);
//    toMark.push_back(rightCenters[extraTimingLineInterval+3]);
//    toMark.push_back(rightCenters[extraTimingLineInterval*2+6]);
//    //markpts(&leftCenters);
//    markpts(&toMark);
    // Now lets see if it can find the rows
//    points_t timingline1, timingline2;
//    point_t timing_xy = leftCenters[extraTimingLineInterval+3];
//    timing_xy.x -= centerOffset;
//    timing_xy.y -= centerOffset;
//    findHorizCenters(&timingline1, timing_xy);
//    interpCenters(&timingline1);
//    timing_xy = leftCenters[extraTimingLineInterval*2+6];
//    timing_xy.x -= centerOffset;
//    timing_xy.y -= centerOffset;
//    findHorizCenters(&timingline2, timing_xy);
//    interpCenters(&timingline2);
//    markpts(&timingline1);
//    markpts(&timingline2);
    
    
    //BBCimage.save("/Users/mrrich/Documents/hacks/sideload/bwtests2/timingcheck.bmp");
}


int BigBarCode::decodeImpBBC(string filename, BBCByteArr_t* data) {
    grid_t topSegment, middleSegment, bottomSegment;
    boxDecodeImp(&topSegment, &middleSegment, &bottomSegment); // Only top segment needed, change
    decodeData(data, &topSegment);
    //BBCimage.save("/Users/mrrich/Documents/hacks/sideload/bwtests2/timingcheck.bmp");

    return 0;
}

int BigBarCode::decodeABBC(string filename, BBCByteArr_t* data) {
    //BBCimage.open(filename);
    //decodeImpBBC(filename, data);
    //return 0;
    
    // decodes a single BBC image, and returns all data contained in the BBC
    BBCimage.open(filename);
    grid_t dataGrid;
    boxDecode(&dataGrid);
    decodeData(data, &dataGrid);
    
    //DEBUG
    //BBCimage.save("/Users/mrrich/Documents/hacks/sideload/bwtests2/timingcheck2.bmp");
    return 0;
}


int BigBarCode::decodeBigBarCode(BBCFileArr_t* BBCSourceFiles, long dataLength, BBCByteArr_t* data) {
    // cycle through each BBC in BBCSourceFiles and concatenate data results
    // truncate final data at datalength and return in data
    BBCByteArr_t decodeddata;
    for (int i = 0; i < len_ptr(BBCSourceFiles); i++) {
        BBCByteArr_t thisdata;
        string filename = (*BBCSourceFiles)[i];
		// DEBUG
		cout << "Decoding file: " << filename << "\n";
        decodeABBC(filename, &thisdata);
        decodeddata.insert(decodeddata.end(), thisdata.begin(), thisdata.end());
    }
    
    if (len(decodeddata) >= dataLength) {
        data->assign(decodeddata.begin(), decodeddata.begin()+dataLength);
    } else {
        throw runtime_error("BigBarCode: Not enough data decoded for given data length!");
    }
    
    return 0;
}

void BigBarCode::setThreshold(colorVal_t thrshld) {
    threshold = thrshld;
}
void BigBarCode::setMasksize(int msksize) {
    masksize = msksize;
}
void BigBarCode::setWigglesize(int wgglsze) {
    wigglesize = wgglsze;
}
