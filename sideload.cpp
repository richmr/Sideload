//
//  sideload.cpp
//  Sideload
//
//  Created by MICHAEL RICH on 12/13/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#include "sideload.h"

#include <fstream>
#include <sstream>

//DEBUG
//#include <iostream>

#include "md5.h"
#include "ReedSolomon.h"
#include "BigBarCode.h"

using namespace std;

Sideload_class::Sideload_class() {
    black_threshold = 150;
    masksize = 7;
    wigglesize = 3;
    RS_n = 255;
    RS_k = 170;
    
}

void Sideload_class::setBlackThreshold(int value) {
    black_threshold = value;
}

void Sideload_class::setMaskValues(int masksize_val, int wigglesize_val) {
    masksize = masksize_val;
    wigglesize = wigglesize_val;
}

void Sideload_class::setReedSolomon(int n_value, int k_value) {
    RS_n = n_value;
    RS_k = k_value;
}


long Sideload_class::encodeSLD(string filenamein, string filenameout, string *md5sum) {
    // Steps:
    // 1 - load file and get md5
    // 2 - RS encode, save length of new data (that's what is returned)
    // 3 - Convert/save as BBC
    
	//DEBUG
	string tracefilename = "C:/Users/owner/Downloads/trace.dat";
	ofstream tracefile(tracefilename);

    // Load file
    ifstream datafile(filenamein, ios::binary);
    stringstream data;
    data << datafile.rdbuf();

	//DEBUG
	tracefile << "sld::encodeSLD: File loaded\n";
	tracefile.flush();
    
    // get md5
    md5sum->assign(md5(data.str()));
	//DEBUG
	tracefile << "sld::encodeSLD: md5 calced\n";
	tracefile.flush();
    
    // 2 - RS encode, save length of new data
    ReedSolomon RS(RS_n,RS_k); // Tring 220 with imp BBC
    string originalData = data.str();
    string encodedData;
    RS.encode(&originalData,&encodedData);
    long datalength = encodedData.length();
	//DEBUG
	tracefile << "sld::encodeSLD: RS calced\n";
	tracefile.flush();
    
    //DEBUG
    // Save out the encoded data
    //ofstream file("/Users/mrrich/Documents/hacks/sideload/bwtests2/encodeddata.bin", ios::binary);
    //file << encodedData;
    //file.close();

    // 3- Convert/save as BBC
    BigBarCode BBC;
    BBC.setThreshold(black_threshold);
    BBC.setMasksize(masksize);
    BBC.setWigglesize(wigglesize);
    
    BBCFileArr_t filescreated;
    BBC.makeBigBarCode(&encodedData, filenameout, &filescreated);
	//DEBUG
	tracefile << "sld::encodeSLD: BBC done\n";
	tracefile.flush();
	tracefile.close();
    return datalength;
}

bool Sideload_class::decodeSLD(BBCFileArr_t *infiles, string filenameout, string md5sum, long encDatalength) {
    // steps:
    // 1 - decode BBC in given files (requires datalength)
    // 2 - RS decode
    // 3 - Compare new data md5sum to given md5sum
    // 4 - If good, save new file, return true, else return false
    
    // 1 - decode BBC in given files (requires datalength)
    BigBarCode BBC;
    BBC.setThreshold(black_threshold);
    BBC.setMasksize(masksize);
    BBC.setWigglesize(wigglesize);

    //BBC.setThreshold(190);
    BBCByteArr_t datavec;
    BBC.decodeBigBarCode(infiles, encDatalength, &datavec);
	cout << "BBC Complete\n";
    
    //DEBUG
    // Save out the encoded data
    //ofstream file1("/Users/mrrich/Documents/hacks/sideload/bwtests2/recoveredfromBBC.bin", ios::binary);
   // string encodedData;
   // encodedData.assign(datavec.begin(), datavec.end());
    //file1 << encodedData;
   // file1.close();

    
    // 2 - RS decode
    // convert vec to string
    ReedSolomon RS(RS_n, RS_k);
    string data;
    data.assign(datavec.begin(), datavec.end());
    string origdata;
	cout << "RS begin\n";
    RS.decode(&data, &origdata);
	cout << "RS End\n";
    
    ofstream file(filenameout, ios::binary);
    file << origdata;
    file.close();

    
    // 3 - Compare new data md5sum to given md5sum
    // 4 - If good, save new file, return true, else return false
    string newmd5sum = md5(origdata);
    if (md5sum == newmd5sum) {
        return true;
    } else {
        return false;
    }
    
}
