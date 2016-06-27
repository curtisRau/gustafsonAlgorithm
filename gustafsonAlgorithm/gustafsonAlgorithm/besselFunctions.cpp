//
//  besselFunctions.cpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include "besselFunctions.hpp"
#include <math.h>                   // For using M_PI and ...


// THIS FUCTIONS APPEARS TO BE WORKING SHOCKINGLY WELL!
// COMPARABLE TO MATHEMATICA FUNCTION IN SPEED AND ACCURACY!!
// Calculate bessel function of first kind using integral
// (1\pi) int_{0}^{pi} Cos[ z Sin (theta) - n theta ] d theta
double BesselJ (unsigned int n, double z) {
    
    if (z == 0.0) {
        if (n == 0) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
    
    unsigned int Npts = 100 * ( 2 * floor(z / M_PI + 0.5) + floor(n + 0.5) + 1);
    double dx = M_PI / Npts;
    double sum = 0.0;
    
    sum += 0.5;
    for (unsigned int i = 1; i < Npts; i++) {
        sum += cos(z * sin(i * dx) - n * i * dx);
    }
    sum += 0.5 * cos(n * M_PI);
    
    return sum / Npts;
}


// THIS FUNCTION APPEARS TO BE WORKING VERY WELL; BEING BOTH FAST AND SEAMINGLY ACCURATE!
// HOWEVER, IT MAY NOT BE ACCURATE ENOUGH TO GENERATE BOUNDARY CONDITIONS!!
//  Calculates an array of bessel function values:
//  J_n (z) for constant n and z in [zMin, zMax].
//  Uses the Thomas Algorithm to solve Bessel's Differential Equation
//  numOfPts is the number of points returned, which includes endpoints.
// numOfPtsRetrn -- The number of points, including endpoints, returned.
// Accuracy Factor -- An integer; should always be greater than or equal to one.
// This is the ratio of the number of points used to solve the differential equation,
// to the number of points returned.  For example, we may only want 10 points returned,
// but only using 10 points to solve the differential equation via the Thomas Algorithm
// could give poor results.  Therefore we use more points to solve the equation, but
// only return a fraction of them.
double* calcBesselArrayConstN (unsigned int n, double xMin, double xMax, unsigned int numOfPtsRetrn, unsigned int accuracyFactor) {
    unsigned int numOfPtsSolve = (numOfPtsRetrn - 1) * accuracyFactor + 1;
    //    double       dx          = (xMax - xMin) / static_cast<double>(numOfPtsRetrn - 1);
    double       dx          = (xMax - xMin) / static_cast<double>(numOfPtsSolve - 1);
    double       dx2         = dx * dx;
    unsigned int n2          = n * n;
    
    
    // Generate the "a" array:
    double* a = new double[numOfPtsSolve - 2];
    for (unsigned int i = 0; i < (numOfPtsSolve - 2); i++) {
        a[i] = (i + 1.0) * (i + 0.5);
    }
    
    // Generate the "b" array:
    double* b = new double[numOfPtsSolve - 2];
    for (unsigned int i = 0; i < (numOfPtsSolve - 2); i++) {
        b[i] = (i + 1.0) * (i + 1.0) * (dx2 - 2.0) - n2;
    }
    
    // Generate the "c" array:
    double* c = new double[numOfPtsSolve - 2];
    for (unsigned int i = 0; i < (numOfPtsSolve - 2); i++) {
        c[i] = (i + 1.0) * (i + 1.5);
    }
    
    // Generate the solution array:
    double* J = new double[numOfPtsSolve];
    
    // Boundary conditions (This is the bottel neck for this function.  VERY SLOW)::
    J[0]            = BesselJ(n, xMin);
    J[numOfPtsSolve - 1] = BesselJ(n, xMax);
    
    // Generate the source array:
    double* S = new double[numOfPtsSolve - 2];
    S[0] = - a[0] * J[0];
    for (unsigned int i = 1; i < (numOfPtsSolve - 3); i++) {
        S[i] = 0.0;
    }
    S[numOfPtsSolve - 3] = - c[numOfPtsSolve - 3] * J[numOfPtsSolve - 1];
    
    
    // Forward substitution
    for (unsigned int i = 1; i <= (numOfPtsSolve - 3); i++) {
        b[i] = b[i] - (a[i]/b[i-1])*c[i-1];
        S[i] = S[i] - (a[i]/b[i-1])*S[i-1];
    }
    
    J[numOfPtsSolve - 2] = S[numOfPtsSolve - 3] / b[numOfPtsSolve - 3];
    
    // Backwards substitution
    for (int i = (numOfPtsSolve - 4); i >= 0; i--) {
        S[i]   = S[i] - (c[i]/b[i+1])*S[i+1];
        J[i+1] = S[i]/b[i];
    }
    
    
    // Allocate Memory for Output::
    double* out = new double [numOfPtsRetrn];
    
    // Extract 'out' from 'J'::
    for (unsigned int i = 0; i < numOfPtsRetrn; i++) {
        out[i] = J[accuracyFactor * i];
    }
    
    
    // Deallocate Memory::
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] S;
    delete [] J;
    
    return out;
}


// THIS FUNCTION APPEARS TO BE WORKING VERY WELL; BEING BOTH FAST AND SEAMINGLY ACCURATE!
// HOWEVER, IT MAY NOT BE ACCURATE ENOUGH TO GENERATE BOUNDARY CONDITIONS!!
//  Calculates an array of bessel function values:
//  J_n(z) for n in [nMin, nMax] and constant z.
//  Accuracy factor not required for this function (unlike calcBesselArrayConstN) because
//  this function solves a recursion relation (an exact formula), whereas calcBesselArrayConstN
//  is solving a system of equations which approximates a differential equation.
double* calcBesselArrayConstZ (double z, unsigned int nMin, unsigned int nMax) {
    unsigned int numOfPts = nMax - nMin + 1;
    double* J = new double [numOfPts];
    
    if (z == 0.0) {
        
        if (nMin == 0) {
            J[0] = 1.0;
        } else {
            J[0] = 0.0;
        }
        
        for (unsigned int n = 1; n < numOfPts; n++) {
            J[n] = 0.0;
        }
        
        return J;
        
    } else {                        // The following method doesn't work if z=0 which is why the if-statement above is required.
        
        // Boundary Conditions (This is the bottel neck for this function.  VERY SLOW)::
        J[0]            = BesselJ(nMin, z);
        J[numOfPts - 1] = BesselJ(nMax, z);
        
        // Generate the "b" array::
        double* b = new double[numOfPts - 2];
        for (unsigned int i = 0; i < numOfPts - 2; i++) {
            b[i] = - 2.0 * (i+1) / z;                           // Cannot divide by zero -> z != 0
        }
        
        // Generate the source array::
        double* S = new double[numOfPts - 2];
        S[0] = - J[0];
        for (unsigned int i = 1; i < numOfPts - 3; i++) {
            S[i] = 0.0;
        }
        S[numOfPts - 3] = - J[numOfPts - 1];
        
        
        // Forward substitution::
        for (unsigned int i = 1; i <= (numOfPts - 3); i++) {
            b[i] = b[i] -   1.0  / b[i-1];
            S[i] = S[i] - S[i-1] / b[i-1];
        }
        
        J[numOfPts - 2] = S[numOfPts - 3] / b[numOfPts - 3];
        
        // Backwards substitution
        for (int i = (numOfPts - 4); i >= 0; i--) {
            S[i]   = S[i] - S[i+1] / b[i+1];
            J[i+1] = S[i] / b[i];
        }
        
        // Deallocate Memory::
        delete [] b;
        delete [] S;
        
        return J;
        
    }
}


//  Calculates an array of bessel function values:
//  J_n(z) for n in [nMin, nMax] and constant z.
//  This function does not have the same bottleneck that 'calcBesselArrayConstZ' has because
//  it does not calculate its own endpoints, but rather expects them to be provided.
//  Accuracy factor not required for this function (unlike calcBesselArrayConstN) because
//  this function solves a recursion relation (an exact formula), whereas calcBesselArrayConstN
//  is solving a system of equations which approximates a differential equation.  Key word is
//  approximates.
double* calcBesselArrayConstZProvidedEndpoints (double z, unsigned int nMin, unsigned int nMax, double J_nMin_z, double J_nMax_z) {
    unsigned int numOfPts = nMax - nMin + 1;
    double* J = new double [numOfPts];
    
    if (z == 0.0) {
        
        if (nMin == 0) {
            J[0] = 1.0;
        } else {
            J[0] = 0.0;
        }
        
        for (unsigned int n = 1; n < numOfPts; n++) {
            J[n] = 0.0;
        }
        
        return J;
        
    } else {                        // The following method doesn't work if z=0 which is why the if-statement above is required.
        
        // Boundary Conditions::
        J[0]            = J_nMin_z;
        J[numOfPts - 1] = J_nMax_z;
        
        // Generate the "b" array::
        double* b = new double[numOfPts - 2];
        for (unsigned int i = 0; i < numOfPts - 2; i++) {
            b[i] = - 2.0 * (i+1) / z;
        }
        
        // Generate the source array::
        double* S = new double[numOfPts - 2];
        S[0] = - J_nMin_z;
        for (unsigned int i = 1; i < numOfPts - 3; i++) {
            S[i] = 0.0;
        }
        S[numOfPts - 3] = - J_nMax_z;
        
        
        // Forward substitution::
        for (unsigned int i = 1; i <= (numOfPts - 3); i++) {
            b[i] = b[i] -   1.0  / b[i-1];
            S[i] = S[i] - S[i-1] / b[i-1];
        }
        
        J[numOfPts - 2] = S[numOfPts - 3] / b[numOfPts - 3];
        
        // Backwards substitution
        for (int i = (numOfPts - 4); i >= 0; i--) {
            S[i]   = S[i] - S[i+1] / b[i+1];
            J[i+1] = S[i] / b[i];
        }
        
        // Deallocate Memory::
        delete [] b;
        delete [] S;
        
        return J;
    }
}



// THIS FUNCTION IS NOT NOT NOT NOT NOT GIVING ACURATE RESULTS AS IS!!!!!!!!!!!!!!!!!!!!
// CURTIS BELIEVES IT IS DUE TO BOUNDARY CONDITIONS OUT AT ZMAX BEING VERY CLOSE TO ZERO
double** generateBesselJMatrix (double zMin, double zMax, unsigned int numOfZ, unsigned int nMin, unsigned int nMax) {
    unsigned int numOfN = nMax - nMin + 1;
    double dz = (zMax - zMin) / static_cast<double>(numOfZ - 1);
    
    double* J_nMin = calcBesselArrayConstN(nMin, zMin, zMax, numOfZ, 100);  // The arbitrary accuracy factor of 100 greatly improves accuracty
    double* J_nMax = calcBesselArrayConstN(nMax, zMin, zMax, numOfZ, 100);  // The arbitrary accuracy factor of 100 greatly improves accuracty
    
    // Unit Test (Print the J_nMin and J_nMax arrays)::
    // --------------------------------------------------------
    //    for (unsigned int i = 0; i < numOfZ; i++) {
    //        cout << "J_nMin[" << i << "] = " << J_nMin[i] << endl;
    //    }
    //    for (unsigned int i = 0; i < numOfZ; i++) {
    //        cout << "J_nMax[" << i << "] = " << J_nMax[i] << endl;
    //    }
    // --------------------------------------------------------
    
    // Allocate Memory For Bessel Matrix::
    double** J = new double* [numOfZ];
    for (unsigned int i = 0; i < numOfZ; i++) {
        J[i] = new double [numOfN];
    }
    
    for (unsigned int z = 0; z < numOfZ; z++) {
        J[z] = calcBesselArrayConstZProvidedEndpoints(z * dz,
                                                      nMin,
                                                      nMax,
                                                      J_nMin[z],
                                                      J_nMax[z]);
    }
    
    // Deallocate Memory::
    delete [] J_nMin;
    delete [] J_nMax;
    
    return J;
}