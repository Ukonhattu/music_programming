#include "iir.h"

static double b[] = {1, -1.4, 1};
static double a[] = {1, -1.3, 0.5};
static double v1m1 = 0, v2m1 = 0, v1m, v2m;

/* Toisen asteen */

double iirfilter(double x1)
{
    double y1 = 0;
    y1 = (b[0] * x1 + v1m1) / a[0];
    v1m = (b[1] * x1 + v2m1) - a[1] * y1;
    v2m = b[2] * x1 - a[2] * y1;
    v1m1 = v1m;
    v2m1 = v2m;
    return y1;
}