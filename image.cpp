//
//  image.cpp
//  Sideload
//
//  Created by MICHAEL RICH on 11/12/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include "image.h"

#include <stdexcept>

//#include "EasyBMP.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#define STBI_ONLY_PNG

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Image::Image() {
    // Empty image
    
    Width = 0;
    Height = 0;
    pxData.clear();
}

Image::Image(string filename) {
    // Opens an image file "filename"
    // Throws exceptions if doesn't exist/can't be processed
    
    Width = 0;
    Height = 0;
    pxData.clear();
    
    open(filename);
}

Image::Image(int width, int rows, pxData_t *imageData) {
    // This is for starting a new image
    // Check consistency
    // imageData length == width*rows (for monochrome images and no alpha)
    if ((width * rows) != len_ptr(imageData)) {
        throw runtime_error("imageData size does not match desired image size");
    }
    
    // Again this is optimized for grayscale work at the moment
    
    Width = width;
    Height = rows;
    pxData = *imageData;
    numLayers = 1;
}

void Image::open(string filename) {
    loadImageSTB(filename);
    
    if ((Width == 0) || (Height == 0)) {
        // then we didn't load anything
        throw runtime_error("Image file failed to load");
    }

}



void Image::save(string filename) {
    
    // Call the image writer here
    //bool success = saveImageEasyBMP(filename);
    bool success = saveImageSTB(filename);
    
   if (!(success)) {
        throw runtime_error("Image::save - image save failed");
    }
    
}


colorVal_t Image::getpixel(point_t point){
    // Check size
    if ((Width < point.x) || (Height < point.y)) {
        throw runtime_error("getpixel location exceeds image bounds");
    }
    
    int offset = calcOffset(point);
    return pxData[offset];
}


void Image::putpixel(point_t point, colorVal_t val){
    // Check size
    if ((Width < point.x) || (Height < point.y)) {
        throw runtime_error("putpixel location exceeds image bounds");
    }
    
    int offset = calcOffset(point);
    pxData[offset] = val;
}


int Image::width(){
    return Width;
}


int Image::height(){
    return Height;
}

int Image::layers() {
    return numLayers;
}

int Image::calcOffset(point_t point) {
    // Returns the array location for the pixel at point
    return ((point.y) * Width) + (point.x);
}


void Image::colorfill(pxData_t *picData, colorVal_t color, int count){
    // static function to quickly append colorVal count times to existing pic data
    // Actually, this looks useless now that I've done it.
    picData->insert(picData->end(), count, color);
}

void Image::loadImageSTB(string filename) {
    // specfically designed to just get the grayscale.
    pxData.clear();
    
    unsigned char *data = stbi_load(filename.c_str(), &Width, &Height, &numLayers, 1);
    
    for (int i = 0; i < (Width * Height); i++) {
        pxData.push_back(data[i]);
    }
    
    stbi_image_free(data);
}

bool Image::saveImageSTB(string filename) {
    unsigned char *data;
    data = new unsigned char[Width*Height*3];
     
    // Expanding to RGB
    int dataIndex = 0;
    for (int i = 0; i < len(pxData); i++) {
        colorVal_t color = pxData[i];
        data[dataIndex++] = color;
        data[dataIndex++] = color;
        data[dataIndex++] = color;
    }
    
    bool result = (bool)stbi_write_bmp(filename.c_str(), Width, Height, 3, data);
    delete[] data;
    return result;
}

/*bool Image::saveImageEasyBMP(string filename) {
    BMP pic;
    
    pic.SetSize(Width, Height);
    
    // setting bit depth to 3x8bits just to make it clear
    pic.SetBitDepth(24);
    
    int pxIndex = 0;
    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            colorVal_t color = pxData[pxIndex];
            pic(i,j)->Red = color;
            pic(i,j)->Green = color;
            pic(i,j)->Blue = color;
            pxIndex++;
        }
    }
    
    return pic.WriteToFile(filename.c_str());
}*/

