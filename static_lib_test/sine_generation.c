/*
 * sine_generation.cpp
 *
 *  Created on: Mar 27, 2015
 *      Author: appsupport
 */


#include <math.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

int sine_generate( const double f, double const fs, const int lastIdx, double * const pSamples, const int nSamples ){
	int i;
	for (i = 0; i < nSamples; i++) {
		pSamples[i] = sin(2*M_PI*f/fs*i);
	}
	return (lastIdx + nSamples);
}

const char * get_hello_str( void ) {
	return "Hi from our C library!";
}



