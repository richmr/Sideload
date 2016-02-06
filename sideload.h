//
//  sideload.h
//  Sideload
//
//  Created by MICHAEL RICH on 12/13/15.
//  Copyright (c) 2015 Mike Rich. All rights reserved.
//

#ifndef __Sideload__sideload__
#define __Sideload__sideload__

#include <stdio.h>
#include <string>

#include "BigBarCode.h"

using namespace std;

class Sideload_class {
private:
    int black_threshold;
    int masksize;
    int wigglesize;
    int RS_n;
    int RS_k;

public:
    Sideload_class();
    
    long encodeSLD(string filenamein, string filenameout, string *md5sum);
    bool decodeSLD(BBCFileArr_t *infiles, string filenameout, string md5sum, long encDataLength);
    
    void setBlackThreshold(int value);
    void setMaskValues(int masksize_val, int wigglesize_val);
    void setReedSolomon(int n_value, int k_value);
    
};

#endif /* defined(__Sideload__sideload__) */
