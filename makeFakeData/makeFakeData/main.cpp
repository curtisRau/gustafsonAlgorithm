//
//  main.cpp
//  makeFakeData
//
//  Created by Curtis Rau on 6/27/16.
//  Copyright © 2016 Curtis Rau and Camilla Compton Inc. All rights reserved.
//

#include <iostream>
#include <fstream>                      // for working with files.
#include <math.h>                       // For M_PI, cos(), ...
#include <complex>                      // For working with complex numbers.

using namespace std;
typedef numeric_limits<double> dbl;

// Function Definitions::
//------------------------------------------------------------------------------------------

// Generates simulated signal.  NTS is the number of points returned.
double* generateData (double duration, unsigned int NTS, double amplitude, double f0, double f1, double Gamma, double phi0, double phi1) {
    double dt = duration / static_cast<double> (NTS - 1);
    double* data = new double [NTS];
    
    f0 *= 2.0 * M_PI;   // Convert to angular frequency.
    f1 *= 2.0 * M_PI;   // Convert to angular frequency.
    
    for (unsigned int i = 0; i < NTS; i++) {
        data[i] = amplitude * cos(f0 * dt * i + phi0 + Gamma * cos(f1 * dt * i + phi1));
    }
    
    return data;
}


// Adds a different random number between '-range' and 'range' to each element in 'data'.
void addNoise (double* data, unsigned int lengthData, double range) {
    for (unsigned int i = 0; i < lengthData; i++) {
        data[i] += range * static_cast<double>(rand() % 100001 - 50000) / 50000.0;
    }
}



// NTS         = The number of points in the time series.
// nyquistFreq = The Nyquist Frequency of the time series.
// df          = step size in frequency.
complex<double>* fourierTransform (double* timeSeries, unsigned int NTS, unsigned int NFS) {
    const complex<double> I (0.0, -2.0 * M_PI / NTS);
    complex<double>* freqSeries = new complex<double> [NFS];
    
    for (unsigned int i = 0; i < NFS; i++) {
        for (unsigned int j = 0; j < NTS; j++) {
            freqSeries[i] += timeSeries[j] * exp(I * static_cast<double>(i * j));
        }
        freqSeries[i] /= sqrt(NTS);
    }
    
    return freqSeries;
}



void saveArrayAsCSV (double* array, string filePath, unsigned int arrayLength) {
    ofstream file;
    file.open(filePath, ios::out | ios::trunc);          // Open a file for output and overwrite current content if it exists.
    if (file.is_open()) {                                                         // If the file is open...
        file << array[0];
        for (unsigned int i = 1; i < arrayLength; i++) {
            file << "," << array[i];
        }
    } else {
        cout << "File '" << filePath << "' did not open" << endl;
    }
    file.close();
}



void saveComplexArrayAsCSV (complex<double>* array, string filePath, unsigned int arrayLength) {
    ofstream file;
    file.open(filePath, ios::out | ios::trunc | ios::binary);          // Open a file for output and overwrite current content if it exists.
    if (file.is_open()) {                                                         // If the file is open...
        file << array[0];
        for (unsigned int i = 1; i < arrayLength; i++) {
            //            file << "," << array[i];
            file << array[i];
        }
    } else {
        cout << "File '" << filePath << "' did not open" << endl;
    }
    file.close();
}


bool saveComplxArray (const complex<double>* pdata, size_t length, const string& file_path )
{
    ofstream os(file_path.c_str(), ios::binary | ios::out);
    if ( !os.is_open() )
        return false;
    os.write(reinterpret_cast<const char*>(pdata), streamsize(length*sizeof(complex<double>)));
    os.close();
    return true;
}



int main(int argc, const char * argv[]) {
    
    cout.precision(dbl::max_digits10);          // Set the output to display the maximum precision
    clock_t begin_time;                         // Declare time variable (used for calculating computation times)
    
    
    // Declare Input Parameters::
    //------------------------------------------------------------------------------------------
    string directory;
    double sampleRate;
    double duration;
    double signalAmplitude;
    double carrierFrequency;
    double modulationFrequency;
    double modulationIndex;
    double carrierPhase;
    double modulationPhase;
    bool   noise;
    double noiseAmplitude           = 0.0;
    
    
    // Prompt User For Program Parameters::
    //------------------------------------------------------------------------------------------
    bool fileValidity = false;
    ifstream f;
    while (fileValidity == false) {
        cout << "Enter Path to Directory where Output will be Saved" << endl;
        cin >> directory;
        f.open(directory, ios::in);
        fileValidity = f.is_open();
    }
    f.close();
    
    cout << "Enter Sample Rate [Hz]" << endl;
    cin >> sampleRate;
    
    cout << "Enter Duration of Data [s]" << endl;
    cin >> duration;
    
    cout << "Enter Amplitude of Signal [strain??]" << endl;
    cin >> signalAmplitude;
    
    cout << "Enter Carrier Frequency [Hz]" << endl;
    cin >> carrierFrequency;
    
    cout << "Enter Modulation Frequency [Hz]" << endl;
    cin >> modulationFrequency;
    
    cout << "Enter Modulation Index [rad]" << endl;
    cin >> modulationIndex;
    
    cout << "Enter Phase of Carrier Wave [rad]" << endl;
    cin >> carrierPhase;
    
    cout << "Enter Phase of Modulation Wave [rad]" << endl;
    cin >> modulationPhase;
    
    cout << "Add Noise? (No = 0, Yes = 1)" << endl;
    cin >> noise;
    
    if (noise) {
        cout << "Enter Noise Amplitude [strain??]" << endl;
        cin >> noiseAmplitude;
    }
    
    
    // Parameters Derived from User Input::
    //------------------------------------------------------------------------------------------
    unsigned int NTS = floor(duration * sampleRate);
    unsigned int NFS = NTS / 2.0;                       // The number of points in the frequency series.
    
    
    // Print Useful Parameters::
    //------------------------------------------------------------------------------------------
    cout << " ----------- Useful Parameters ----------- "                              << endl;
    cout << "Number of Points in the Time Series      = " << NTS                       << endl;
    cout << "Number of Points in the Frequency Series = " << NFS                       << endl;
    cout << "Nyquist Frequency                        = " << sampleRate / 2.0 << " Hz" << endl;
    cout << "Fundamental Frequency                    = " << 1.0 / duration   << " Hz" << endl;
    cout << " ----------- Computation Times ----------- "                              << endl;
    
    
    // Allocate Memory::
    //------------------------------------------------------------------------------------------
    double*          data  = new double [NTS];
    complex<double>* trans = new complex<double> [NFS];
    
    // Generate Timeseries Data::
    //------------------------------------------------------------------------------------------
    begin_time = clock();
    data = generateData(duration, NTS, signalAmplitude, carrierFrequency, modulationFrequency, modulationIndex, carrierPhase, modulationPhase);
    cout << "Generating Data Took              = " << (clock () - begin_time) /  CLOCKS_PER_SEC << " s" << endl;
    
    
    // Save Timeseries Data::
    //------------------------------------------------------------------------------------------
    saveArrayAsCSV(data, directory + "/timeseries.csv", NTS);
    
    
    // Add Noise::
    //------------------------------------------------------------------------------------------
    if (noise) {
        begin_time = clock();
        addNoise(data, NTS, noiseAmplitude);
        cout << "Adding Noise to Time Series Took  = " << (clock () - begin_time) /  CLOCKS_PER_SEC << " s" << endl;
        
        // Save Noisy Timeseries Data::
        saveArrayAsCSV(data, directory + "/noisyTimeseries.csv", NTS);
    }
    
    
    // Perform Fourier Transform::
    //------------------------------------------------------------------------------------------
    begin_time = clock();
    trans = fourierTransform(data, NTS, NFS);
    cout << "Performing Fourier Transform Took = " << (clock () - begin_time) /  CLOCKS_PER_SEC << " s" << endl;
    
    
    // Save Fourier Transform::
    //------------------------------------------------------------------------------------------
    saveComplxArray(trans, NFS, directory + "/frequencySeries.csv");
    
    
    // Deallocate Memory::
    //------------------------------------------------------------------------------------------
    delete [] data;
    delete [] trans;
    
    
    // Write Text File Containing Data Parameters::
    //------------------------------------------------------------------------------------------
    ofstream file;
    file.open(directory + "/dataParameters.txt", ios::out | ios::trunc);          // Open a file for output and overwrite current content if it exists.
    if (file.is_open()) {                                                         // If the file is open...
        file << " ----------- User Defined Parameters ----------- "                            << endl;
        file << "Sample Rate                              = " << sampleRate          << " Hz"  << endl;
        file << "Duration                                 = " << duration            << " s"   << endl;
        file << "Amplitude of Signal                      = " << signalAmplitude     <<           endl;
        file << "Carrier Frequency                        = " << carrierFrequency    << " Hz"  << endl;
        file << "Modulation Frequency                     = " << modulationFrequency << " Hz"  << endl;
        file << "Modulation Index                         = " << modulationIndex     << " rad" << endl;
        file << "Phase of Carrier Wave                    = " << carrierPhase        << " rad" << endl;
        file << "Phase of Modulation Wave                 = " << modulationPhase     << " rad" << endl;
        file << "Noise Added                                " << noise               <<           endl;
        file << "Amplitude of Noise                       = " << noiseAmplitude      <<           endl;
        file << " -------------- Useful Parameters -------------- "                  <<           endl;
        file << "Number of Points in the Time Series      = " << NTS                 <<           endl;
        file << "Number of Points in the Frequency Series = " << NFS                 <<           endl;
        file << "Nyquist Frequency                        = " << sampleRate / 2.0    << " Hz"  << endl;
        file << "Fundamental Frequency                    = " << 1.0 / duration      << " Hz"  << endl;
    } else {
        cout << "Error Writing Data Parameters File" << endl;
    }
    file.close();
    
    return 0;
}
