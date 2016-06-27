//testing camilla
//  gustafsonAlgorithm.cpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include "gustafsonAlgorithm.hpp"
#include <iostream>
#include <complex>                      // For working with complex numbers.

using namespace std;


// Calculates the number of sidebands on each side of the carrier
// the algorithm should perform its search over.  This is based on
// Rau's Theorem.
unsigned int calcNumberSidebands (double Gamma) {
    //    return max(static_cast<unsigned int>(1),
    //               static_cast<unsigned int>(
    //                                         1.5 * (Gamma - log(Gamma)/log(M_PI))
    //                                         ));
    if (Gamma <= M_PI) {
        return floor(1.1 * Gamma + 1.9);
    } else {
        return floor(1.1 * (Gamma - log(Gamma)/log(M_PI)) + 3.0);
    }
}


unsigned int calcNumberStepsInPhase (double Gamma, unsigned int stepsPerOscillation) {
    return 2 * static_cast<unsigned int>(round(Gamma)) * stepsPerOscillation + 1;
}


// Fold a frequency.
// "f"   is the frequency to be folded
// "fnq" is the nyquist or folding frequency
// "f"   is folded between 0 and fnq
unsigned int foldIntFrequency (int f, unsigned int fnq) {
    f = abs(f);
    unsigned int ffolded = f;
    if (f > fnq) {
        unsigned int n = static_cast<unsigned int>(ceil(static_cast<double>(f)/static_cast<double>(fnq))) % 2;
        switch (n) {
            case 0:                         // Even folding zone
                ffolded = fnq - f % fnq;    // Mirror Frequency
                break;
            case 1:                         // Odd folding zone
                ffolded = f % fnq;          // Translate Frequency
                break;
        }
    }
    return ffolded;
}


// Fold a frequency.
// "f"   is the frequency to be folded
// "fnq" is the nyquist or folding frequency
// "f"   is folded between 0 and fnq
complex<double> foldIntFrequencyPhase (complex<double>* F, int f, unsigned int fnq) {
    f = abs(f);
    double phase = 1.0;
    if (f > fnq) {
        unsigned int n = static_cast<unsigned int>(ceil(static_cast<double>(f)/static_cast<double>(fnq))) % 2;
        switch (n) {
            case 0:                     // Even folding zone
                f = fnq - f % fnq;      // Mirror Frequency
                phase = -1.0;           // Include phase shift
                break;
            case 1:                     // Odd folding zone
                f = f % fnq;            // Translate Frequency
                break;
        }
    }
    return F[f] * phase;
}



// F is an array of complex points which represents a discrete fourier transform.
// fMax is the maximum frequency in F.
// df is the step size in frequency of F.
// besselArray is an array of bessel functions "J_n(x)" all evaluated at the same
//      value of "x", but different "n".
// omega is the value of "omega0" we are trying.
// f1 CANNOT BE 0 BECAUSE DIVISION BY 0!
// fMax CANNOT BE LESS THAN f!
double gustafsonAlgorithm (complex<double>* F, unsigned int nyquistF, double* besselArray, int besselArrayLength, unsigned int nSidebandsSum,
                           unsigned int f0, unsigned int f1, double phi1) {
    
    const complex<double> I   (0.0, -phi1 + M_PI_2);
    const complex<double> J   (0.0, -phi1 - M_PI_2);
    complex<double> sum (0.0, 0.0);
    
    if (nSidebandsSum > besselArrayLength) {
        nSidebandsSum = besselArrayLength;
    }
    
    for (int n = -static_cast<int>(nSidebandsSum); n < 0; n++) {
        sum += exp(I * static_cast<double>(n)) * besselArray[abs(n)] * foldIntFrequencyPhase(F, f0 + n*f1, nyquistF);
    }
    for (int n = 0; n <= static_cast<int>(nSidebandsSum); n++) {
        sum += exp(J * static_cast<double>(n)) * besselArray[n] * foldIntFrequencyPhase(F, f0 + n*f1, nyquistF);
    }
    
    return pow(2.0 * abs(sum), 2);
}