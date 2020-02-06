#include <stdio.h>
#include <stdint.h>
#include "fir.h"
 
/*maximum number of inputs that can be handled
 * in one function call */
#define MAX_INPUT_LEN   80
/* maximum length of filter than can be handled */
#define MAX_FLT_LEN     63
/* buffer to hold all of the input samples */
#define BUFFER_LEN      (MAX_FLT_LEN - 1 + MAX_INPUT_LEN)
 
/* array to hold input samples */
double *insamp;
 
/* FIR init */
void firFloatInit( void )
{
    insamp = calloc(0, BUFFER_LEN * sizeof(double));
}

void firFloatFree( void ) {
    free( insamp );
}
 
/* the FIR filter function */
void firFloat( double *coeffs, double *input, double *output,
       int length, int filterLength )
{
    double acc;     /*  accumulator */
    double *coeffp; /* pointer to coefficients */
    double *inputp; /* pointer to input samples */
    int i;
    int j;
 
    /* put the new samples at the high end of the buffer */
    memcpy( &insamp[filterLength - 1], input,
            length * sizeof(double) );
 
    /* apply the filter to each input sample */
    for ( i = 0; i < length; i++ ) {
        /* calculate output n */
        coeffp = coeffs;        
        inputp = &insamp[filterLength - 1 + i];
        acc = 0;
        for ( j = 0; j < filterLength; j++ ) {
            acc += (*coeffp++) * (*inputp--);
        }
        output[i] = acc;
    }
    
    // shift input samples back in time for next time
    memmove( &insamp[0], &insamp[length],
            (filterLength - 1) * sizeof(double) );
 
}