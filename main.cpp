//
//  main.cpp
//  Sideload
//
//  Created by MICHAEL RICH on 11/9/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "image.h"

// looks like k = 170 might work

#include "BigBarCode.h"
#include "lineFxns.h"
#include "ReedSolomon.h"
#include "md5.h"
#include "sideload.h"

using namespace std;

void printPx(point_t point, Image *image) {
    
    cout << "Point " << point.x << "," << point.y << ": " << (int)image->getpixel(point) << "\n";
    
}

void testImage() {
    string filename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin-2of2.sld-bwencode2.bmp";
    Image image(filename);
    
    // Image stats
    cout << "H, W: " << image.height() << ", " << image.width() << "\n";
    
    // get some points
    printPx({42,28}, &image);
    printPx({42,29}, &image);
    printPx({42,30}, &image);
    printPx({42,31}, &image);
    
    // crash it?
    //image.getpixel({0, 710});
    
    // save it
    filename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/savetest.bmp";
    image.save(filename);

}

void testBBC() {
    string infilename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin";
    BigBarCode BBC;
    
    BBCFileArr_t createdFiles;
    BBC.makeBigBarCode(infilename, &createdFiles);
    
    cout << "BBC created " << len(createdFiles) << " files: \n";
    for (int i = 0; i < len(createdFiles); i++) {
        cout << createdFiles[i] << "\n";
    }
    
}

void testmultiBBCdecode() {
    BBCFileArr_t files;
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin-1of2.sld-scanned.gif");
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin-2of2.sld-scanned.gif");
    BigBarCode BBC;
    
    BBCByteArr_t data;
    BBC.decodeBigBarCode(&files, 89760, &data);
    string outfilename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/multitest-decode.bin";
    BBC.writeVectorToFile(&data, outfilename);
}

void testBBCDecode() {
    string infilename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/imp2o2.png";
    BigBarCode BBC;
    BBC.setThreshold(180);
    BBCByteArr_t data;
    BBC.decodeABBC(infilename, &data);
    
    // save it
    //string outfilename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/BBCdecodetest-1of2.bin";
    //BBC.writeVectorToFile(&data, outfilename);

}

void linefxntest() {
    points_t top;
    top.push_back({2,1});
    top.push_back({3,1});
    top.push_back({5,2});
    
    points_t right;
    right.push_back({7,1});
    right.push_back({7,3});
    right.push_back({8,5});
    
    points_t left;
    left.push_back({1,2});
    left.push_back({1,4});
    left.push_back({1,5});
    
    points_t bottom;
    bottom.push_back({3,6});
    bottom.push_back({4,6});
    bottom.push_back({6,7});
    
    lineFxns lftest;
    grid_t results;
    lftest.calcGridFromEndpoints(&top, &bottom, &left, &right, &results);
    cout << "mom\n";
}

void quicktest() {
    BigBarCode BBC;
    ReedSolomon RS(255,170);
    
    string infile = "/Users/mrrich/Documents/hacks/sideload/bwtests2/trial7650.bin";
    BBCByteArr_t indata;
    
    
    
}

void md5test() {
    string data = "grape";
    cout << "md5 of 'grape': " << md5(data) << endl;
}

void md5filetest() {
    // Load file
    string filenamein = "/Users/mrrich/Documents/hacks/sideload/bwtests2/trial7650.bin";
    ifstream datafile(filenamein, ios::binary);
    stringstream data;
    data << datafile.rdbuf();
    
    cout << "data length = " << data.str().length() << "\n";
    
    string md5sum = md5(data.str());
	string* md5sum_ptr = new string();
    md5sum_ptr->assign(md5(data.str()));
    cout << "md5 of 'file: " << *md5sum_ptr << endl;
	delete md5sum_ptr;
}

void sldtest() {
    Sideload_class sld;
    // This worked with k=170!!
    //string filenamein = "/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin";
    //string filenameout = "/Users/mrrich/Documents/hacks/sideload/bwtests2/test2.bin";
    // This being done with k = 140 to test.
    sld.setReedSolomon(255, 220);
    string filenamein = "/Users/mrrich/Documents/hacks/sideload/bwtests2/Archive 2.zip";
    string filenameout = "/Users/mrrich/Documents/hacks/sideload/bwtests2/payload-test-k220-imp.zip";
    
    string md5sum;
    long encodedlength = sld.encodeSLD(filenamein, filenameout, &md5sum);
    cout << "encoded length: " << encodedlength << "\nmd5 of file: " << md5sum << endl;
}

void decodeTestPayload() {
    Sideload_class sld;
    BBCFileArr_t files;
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/pay140-1o3.png");
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/pay140-2o3.png");
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/pay140-3o3.png");
    
    //encoded length: 157248 FOR k=220 imp
    //md5 of file: a5a24c8a80c2a12e13dffdd91cbdf8cb
    

    
    //encoded length: 247088 for K = 140
    //md5 of file: a5a24c8a80c2a12e13dffdd91cbdf8cb
    
    bool worked = sld.decodeSLD(&files, "/Users/mrrich/Documents/hacks/sideload/bwtests2/payload-recreate.zip", "a5a24c8a80c2a12e13dffdd91cbdf8cb", 247088);

    if (worked) {
        cout << "OMG!!\n";
    } else {
        cout << "Mom's spaghetti\n";
    }
}

void decodeTestTest() {
    Sideload_class sld;
    BBCFileArr_t files;
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2_1of2.png");
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/test2_2of2.png");
    
    bool worked = sld.decodeSLD(&files, "/Users/mrrich/Documents/hacks/sideload/bwtests2/test2-recreate.bin", "63a2e0f746e5673a6b8f60fec2e18c6f", 97555);
    
    if (worked) {
        cout << "OMG!!\n";
    } else {
        cout << "Mom's spaghetti\n";
    }
}

void testExtraTimingMark() {
    BigBarCode BBC;
    BBCByteArr_t data;
    string filename = "/Users/mrrich/Documents/hacks/sideload/bwtests2/imp1o2.png";
    BBC.decodeABBC(filename, &data);
}

void testImpBBC() {
    Sideload_class sld;
    BBCFileArr_t files;
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/imp1o2.png");
    files.push_back("/Users/mrrich/Documents/hacks/sideload/bwtests2/imp2o2.png");
    
    sld.setBlackThreshold(180);
    sld.setReedSolomon(255, 220);
    //encoded length: 157248 FOR k=220 imp
    //md5 of file: a5a24c8a80c2a12e13dffdd91cbdf8cb
    
    
    
    //encoded length: 247088 for K = 140
    //md5 of file: a5a24c8a80c2a12e13dffdd91cbdf8cb
    
    bool worked = sld.decodeSLD(&files, "/Users/mrrich/Documents/hacks/sideload/bwtests2/payload-recreate.zip", "a5a24c8a80c2a12e13dffdd91cbdf8cb", 157248);
    
    if (worked) {
        cout << "OMG!!\n";
    } else {
        cout << "Mom's spaghetti\n";
    }

}

int main(int argc, const char * argv[]) {
    cout << "start\n";
    //testImage();
    //testBBC();
    testBBCDecode();
    //linefxntest();
    //md5test();
    //md5filetest();
    //sldtest();
    //decodeTestPayload();
    //decodeTestTest();
    //testmultiBBCdecode();
    //testExtraTimingMark();
    //testImpBBC();
    cout << "done\n";
        return 0;
}
