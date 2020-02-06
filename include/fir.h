#ifndef FIR_H
#define FIR_H

void firFloatInit(void);
void firFloatFree(void);
int firFloat(double *coeffs, double *input, double *output,
              int length, int filterLength);

#endif