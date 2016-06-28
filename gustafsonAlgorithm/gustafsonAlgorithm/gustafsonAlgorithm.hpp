//
//  gustafsonAlgorithm.hpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#ifndef gustafsonAlgorithm_hpp
#define gustafsonAlgorithm_hpp

#include <stdio.h>
#include <complex>                      // For working with complex numbers.
using namespace std;

#endif

unsigned int    calcNumberSidebands    (double Gamma);
unsigned int    calcNumberStepsInPhase (double Gamma, unsigned int stepsPerOscillation);
unsigned int    foldIntFrequency       (int f, unsigned int fnq);
complex<double> foldIntFrequencyPhase  (complex<double>* F, int f, unsigned int fnq);
double          gustafsonAlgorithm     (complex<double>* F, unsigned int nyquistF, double* besselArray,
                                        unsigned int nSidebandsSum, unsigned int f0, unsigned int f1, double phi1);