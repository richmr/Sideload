//
//  image.h
//  Sideload
//
//  Created by MICHAEL RICH on 11/12/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#ifndef __Sideload__image__
#define __Sideload__image__

#include <stdio.h>
#include <vector>
#include <string>

#include "lineFxns.h"



using namespace std;

// Wrapper for handling images for sideload
// Only opens png (because that is what adobe saves)
// Only saves bmp (because it is a simple format)
// Currently written specifically for monochrome images with no alpha

typedef unsigned char colorVal_t;
typedef vector<colorVal_t> pxData_t;


class Image {
  private:
    void loadImageSTB(string filename);
    int calcOffset(point_t point);
    
//    bool saveImageEasyBMP(string filename);
    bool saveImageSTB(string filename);
    
    // instance variables
    int Width;
    int Height;
    int numLayers;
    pxData_t pxData;
    
   
public:
    Image();
    Image(string filename);
    Image(int width, int rows, pxData_t *imageData);
    
    void open(string filename);
    void save(string filename);
    colorVal_t getpixel(point_t point);
    void putpixel(point_t point, colorVal_t val);
    int width();
    int height();
    int layers();
    
    static void colorfill(pxData_t *picData, colorVal_t color, int count);
    
    
};

#endif /* defined(__Sideload__image__) */
