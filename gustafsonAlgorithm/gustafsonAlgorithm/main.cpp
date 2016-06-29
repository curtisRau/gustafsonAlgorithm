//
//  main.cpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include <iostream>
#include <fstream>                      // for working with files.
#include <complex>                      // For working with complex numbers.
#include "gustafsonAlgorithm.hpp"
#include "besselFunctions.hpp"
#include "fileIO.hpp"
#include "unitTests.hpp"

using namespace std;
typedef numeric_limits<double> dbl;



int main(int argc, const char * argv[]) {
    cout.precision(dbl::max_digits10);          // Set the output to display the maximum precision
    clock_t begin_time;                         // Declare time variable (used for calculating computation times)


    // Declare Input Parameters::
    //------------------------------------------------------------------------------------------
    bool         fileValidity;
    ifstream     file;
    string       path2frequencySeries;
    string       pathToOutput;
    double       sampleRate;
    double       df;
    double       modIndxMin;
    double       modIndxMax;
    double       dModIndx;
    double       f0Start;
    double       f0End;
    double       f1Start;
    double       f1End;
    unsigned int nStepsModPhase;


    // Prompt User For Program Parameters::
    //------------------------------------------------------------------------------------------
    fileValidity = false;
    while (fileValidity == false) {
        cout << "Enter Path to Frequency Series Data (/Volumes/userFilesPartition/Users/curtisrau/Desktop/output/frequencySeries.csv)" << endl;
        cin >> path2frequencySeries;
        file.open(path2frequencySeries, ios::in);
        fileValidity = file.is_open();
    }
    file.close();
    
    fileValidity = false;
    while (fileValidity == false) {
        cout << "Enter Path to Directory where Output will be Written" << endl;
        cin >> pathToOutput;
        file.open(makeFileName(pathToOutput, 0.0, 0.0), ios::out | ios::trunc);
        fileValidity = file.is_open();
    }
    file.close();
    

    cout << "Enter Sample Rate [Hz]" << endl;
    cin >> sampleRate;

    cout << "Enter Step Size in Frequency of the Fourier Transform [Hz] (0.01)" << endl;
    cin >> df;

    cout << "Enter Minimum Value of Modulation Index to Search Over [rad] (0)" << endl;
    cin >> modIndxMin;

    cout << "Enter Maximum Value of Modulation Index to Search Over [rad] (10)" << endl;
    cin >> modIndxMax;

    cout << "Enter the Step Size in the Modulation Index" << endl;
    cin >> dModIndx;

    cout << "Enter Lowest Carrier Frequenty to Start Search Over [Hz]" << endl;
    cin >> f0Start;

    cout << "Enter Highest Carrier Frequency ot End Search Over [Hz]" << endl;
    cin >> f0End;

    cout << "Enter Lowest Modulation Frequency [Hz]" << endl;
    cin >> f1Start;

    cout << "Enter Highest Modulation Frequency [Hz]" << endl;
    cin >> f1End;

    cout << "Enter Number of Steps to Take in Modulation Phase per Oscillation" << endl;
    cin >> nStepsModPhase;


    // Parameters Derived from User Input::
    //------------------------------------------------------------------------------------------
    double       duration      = 1.0 / df;
    unsigned int NTS           = floor(sampleRate * duration);
    unsigned int NFS           = NTS / 2.0;                                         // The number of points in the frequency series.
    unsigned int NMI           = floor((modIndxMax - modIndxMin) / dModIndx) + 1;   // The number of steps in modulation index.
    double       nyquistF      = sampleRate / 2.0;                                  // The Nyquist Frequency.
    unsigned int natNyquistF   = NFS - 1;                                           // The Nyquist Frequency in Naturan Units.
    unsigned int nSidebandsMax = calcNumberSidebands(modIndxMax);                   // The maximum number of sidebands we want to sum over
    unsigned int nF0Min        = floor(f0Start / df);
    unsigned int nF0Max        = floor( f0End  / df);
    unsigned int nF0           = nF0Max - nF0Min + 1;
    unsigned int nF1Min        = floor(f1Start / df);
    unsigned int nF1Max        = floor( f1End  / df);
    unsigned int nF1           = nF1Max - nF1Min + 1;

    unsigned int nStepsModPhaseMax = calcNumberStepsInPhase(modIndxMax, nStepsModPhase);
    
    
    // Calculate the Size of the Output::
    //------------------------------------------------------------------------------------------
    unsigned int numFilesWritten = 0;
    double       sizeOfOutputFiles;
    double       outputSize;
    for (unsigned int g = 0; g < NMI; g++) {
        numFilesWritten += calcNumberStepsInPhase(modIndxMin + dModIndx * g, nStepsModPhase);
    }
    sizeOfOutputFiles = nF0 * nF1 * sizeof(double);
    sizeOfOutputFiles /= 1000.0;                        // Convert to KiloBytes
    outputSize = numFilesWritten * sizeOfOutputFiles;
    outputSize /= 1000.0;                            // Convert to MegaBytes
    

    // Print Useful Parameters::
    //------------------------------------------------------------------------------------------
    cout << " -------------- Output Files Info ------------- " <<                               endl;
    cout << " Number of Files to be Written                = " << numFilesWritten            << endl;
    cout << " Size of Each File to be Written              = " << sizeOfOutputFiles << " KB" << endl;
    cout << " Total Bytes to be Written                    = " << outputSize        << " MB" << endl;
    cout << " -------------- Search Parameters ------------- " <<                               endl;
    cout << "Number of Steps in Carrier Frequency          = " << nF0                        << endl;
    cout << "Number of Steps in Modulation Frequency       = " << nF1                        << endl;
    cout << "Number of Steps in Modulation Index           = " << NMI                        << endl;
    cout << "Maximum Number of Steps in Phase              = " << nStepsModPhaseMax          << endl;
    cout << "Maximum Number of Sidebands that will be used = " << nSidebandsMax              << endl;
    cout << " -------------- Useful Parameters ------------- " <<                               endl;
    cout << "Number of Points in the Time Series           = " << NTS                        << endl;
    cout << "Number of Points in the Frequency Series      = " << NFS                        << endl;
    cout << "Nyquist Frequency                             = " << nyquistF          << " Hz" << endl;
    cout << "Duration of Time Series                       = " << duration          << " s"  << endl;
    cout << " -------------- Computation Times ------------- " <<                               endl;


    // IMPLEMENTING THE GUSTAFSON ALGORITHM
    //------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------
    

    // Read in Fourier Series::
    //------------------------------------------------------------------------------------------
    complex<double>* freqSeries = new complex<double>[NFS];
    loadComplxArray(freqSeries, NFS, path2frequencySeries);
    
    
    // Declare Variables for Main Loops::
    //------------------------------------------------------------------------------------------
    double       modIndx;                       // The modulation Index.
    unsigned int NSB;                           // Number of SideBands in Sum (function of Modulation Index)
    unsigned int NMP;                           // Number of Steps in Modulation Phase (function of Modulation Index)
    double       dModPhase;                     // Step size in Modulation Phase
    double       modPhase;
    unsigned int carrFreq;                      // The value of the carrier frequency in natural units.
    unsigned int modlFreq;                      // The value of the modulation frequency in natural units.

    
    // Allocate Memory for Data Structures used in Main Loops::
    //------------------------------------------------------------------------------------------
    double*  J      = new double  [nSidebandsMax];      // This may be problematic because "nSidebandsMax" may not be equal to
                                                        // "calcNumberSidebands(modIndxMin + dModIndx * NMI)"
    double** output = new double* [nF0];
    for (unsigned int i = 0; i < nF0; i++) {
        output[i] = new double [nF1];
    }
    
    
    // The Main Loops -- The Code Which Performs the Actual Search::
    //------------------------------------------------------------------------------------------
    begin_time = clock();
    for (unsigned int g = 0; g < NMI; g++) {                            // -- Search over Modulation Index --
        // Calculate Parameters that Depend on the Modulation Index::
        modIndx   = modIndxMin + dModIndx * g;
        NSB       = calcNumberSidebands(modIndx);
        NMP       = calcNumberStepsInPhase(modIndx, nStepsModPhase);
        dModPhase = 2 * M_PI / NMP;
        J         = calcBesselArrayConstZ(modIndx, 0, NSB);             // Calculate Bessel Functions
        
        for (unsigned int p = 0; p < NMP; p++) {                        // -- Search over the Phase of the Modulation --
            modPhase = - M_PI + dModPhase * p;
            for (unsigned int fc = 0; fc < nF0; fc++) {                 // -- Search over the Carrier Frequency --
                carrFreq = nF0Min + fc;
                for (unsigned int fm = 0; fm < nF1; fm++) {             // -- Search over the Modulation Frequency --
                    modlFreq = nF1Min + fm;
                    output[fc][fm] = gustafsonAlgorithm(freqSeries,
                                                        natNyquistF,
                                                        J,
                                                        NSB,
                                                        carrFreq,
                                                        modlFreq,
                                                        modPhase);
                }
            }
            // Save the output -- This is more for debugging purposes
            // until a better method for processing the results
            // can be determined
            saveMatrix4Mathematica(makeFileName(pathToOutput, modIndx, modPhase),
                                   output,
                                   nF0,
                                   nF1);
        }
    }
    
    
    // Print the computation time to perform the search::
    cout << "Performint the Search Took = " << (clock () - begin_time) /  CLOCKS_PER_SEC << " s" << endl;
    
    
    // Deallocate Memory::
    //------------------------------------------------------------------------------------------
    delete [] freqSeries;
    delete [] J;
    for (unsigned int i = 0; i < nF0; i++) {
        delete [] output[i];
    }
    delete [] output;
    
    
    return 0;
}
