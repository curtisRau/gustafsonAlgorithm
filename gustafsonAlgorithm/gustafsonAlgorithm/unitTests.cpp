//
//  unitTests.cpp
//  gustafsonAlgorithm
//
//  Created by Curtis Rau on 6/28/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include "unitTests.hpp"
#include <iostream>

using namespace std;


// Progress Bar Constructor
progressBar::progressBar(unsigned int windowWidth, unsigned int numIttersMax) {
    itterate(windowWidth, numIttersMax);
}

// Progress Bar Member Function
void progressBar::itterate (unsigned int windowWidth, unsigned int numIttersMax) {
    
    cout << "-";
}