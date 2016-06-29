//
//  fileIO.cpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/28/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include "fileIO.hpp"
#include <iostream>
#include <fstream>              // for working with files.
#include <complex>              // For working with complex numbers.

using namespace std;


void loadComplxArray (complex<double>* data, size_t length, const string& file_path) {
    ifstream file;
    file.open(file_path.c_str(), ios::binary | ios::in);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(data), streamsize(length * sizeof(complex<double>)));
    } else {
        cout << "File '" << file_path << "' did not load!" << endl;
    }
    file.close();
}


void saveArray4Mathematica (const char* filename, double* array, unsigned int arraySize) {
    ofstream outputFile;
    outputFile.open(filename, ios::out | ios::trunc);         // Open a file for output and overwrite current content if it exists.
    
    if (outputFile.is_open()) {                                         // If the file is open...
        outputFile << array[0];
        for (unsigned int i = 1; i < arraySize; i++) {
            outputFile << "\t" << array[i];
        }
    } else {
        cout << "File '" << filename << "' did not open!" << endl;
    }
    outputFile.close();
}

//void saveMatrix4Mathematica (const char* filename, double** matrix, unsigned int matrixSizeM, unsigned int matrixSizeN) {
void saveMatrix4Mathematica (string filename, double** matrix, unsigned int matrixSizeM, unsigned int matrixSizeN) {
    ofstream outputFile;
    outputFile.open(filename, ios::out | ios::trunc);         // Open a file for output and overwrite current content if it exists.
    
    if (outputFile.is_open()) {                                         // If the file is open...
        for (unsigned int i = 0; i < (matrixSizeM - 1); i++) {
            outputFile << matrix[i][0];
            for (unsigned int j = 1; j < matrixSizeN; j++) {
                outputFile << "\t" << matrix[i][j];
            }
            outputFile << "\r";
        }
        outputFile << matrix[matrixSizeM - 1][0];
        for (unsigned int j = 1; j < matrixSizeN; j++) {
            outputFile << "\t" << matrix[matrixSizeM - 1][j];
        }
    } else {
        cout << "File '" << filename << "' did not open!" << endl;
    }
    outputFile.close();
}


// This function generates a discriptive filename for output files based on
// the modulation index, modulation phase, carrier frequency, and modulation frequency.
// The function returns the full file path, ie. the directory concatinated with the
// descriptive filename.
string makeFileName (string directory, double modIndx, double modPhase) {
    return directory + "modIndx"  + to_string(modIndx)
                     + "modPhase" + to_string(modPhase)
                     + ".csv";
}



