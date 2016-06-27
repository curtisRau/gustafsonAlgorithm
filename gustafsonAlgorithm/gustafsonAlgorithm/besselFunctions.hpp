//
//  besselFunctions.hpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#ifndef besselFunctions_hpp
#define besselFunctions_hpp

#include <stdio.h>

#endif

double   BesselJ                                (unsigned int n, double z);
double*  calcBesselArrayConstN                  (unsigned int n, double xMin, double xMax, unsigned int numOfPtsRetrn,
                                                 unsigned int accuracyFactor);
double*  calcBesselArrayConstZ                  (double z, unsigned int nMin, unsigned int nMax);
double*  calcBesselArrayConstZProvidedEndpoints (double z, unsigned int nMin, unsigned int nMax, double J_nMin_z, double J_nMax_z);
double** generateBesselJMatrix                  (double zMin, double zMax, unsigned int numOfZ, unsigned int nMin, unsigned int nMax);