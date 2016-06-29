//
//  fileIO.hpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/28/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#ifndef fileIO_hpp
#define fileIO_hpp

#include <stdio.h>
#include <iostream>     // For 'string'
#include <complex>      // For working with complex numbers.

using namespace std;

#endif

void loadComplxArray (complex<double>* data, size_t length, const string& file_path);

void saveArray4Mathematica (const char* filename, double* array, unsigned int arraySize);
//void saveMatrix4Mathematica (const char* filename, double** matrix, unsigned int matrixSizeM, unsigned int matrixSizeN);
void saveMatrix4Mathematica (string filename, double** matrix, unsigned int matrixSizeM, unsigned int matrixSizeN);

string makeFileName (string directory, double modIndx, double modPhase);