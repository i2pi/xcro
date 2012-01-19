

/*
 *  Author: Arvin Schnell
 */


#include <math.h>

#include "fft.h"


inline double
sqr (double arg)
{
    return arg * arg;
}


inline void
swap (double* a, double* b)
{
    double t = *a;
    *a = *b;
    *b = t;
}


void
fft (double* data, int N, int isign)
{
    int n = N << 1;
    int i, j, m;
    double theta, wr, wpr, wpi, wi, wtemp;
    double tempr, tempi;
    const double tmp = 1.0 / sqrt (N);
    int mmax;
	int istep;

    /* bit reversal section */

    j = 0;
    for (i = 0; i < n; i += 2) {
	if (j > i) {
	    swap (&data[j], &data[i]);
	    swap (&data[j + 1], &data[i + 1]);
	}
	m = N;
	while (m >= 2 && j >= m) {
	    j -= m;
	    m >>= 1;
	}
	j += m;
    }

    /* Daniel-Lanczos section */

    for (mmax = 2; n > mmax;) {
	istep = mmax << 1;
	theta = isign * (2.0 * M_PI / mmax);
	wpr = -2.0 * sqr (sin (0.5 * theta));
	wpi = sin (theta);
	wr = 1.0;
	wi = 0.0;
	for (m = 0; m < mmax; m += 2) {
	    for (i = m; i < n; i += istep) {
		j = i + mmax;
		tempr = wr * data[j] - wi * data[j + 1];
		tempi = wr * data[j + 1] + wi * data[j];
		data[j] = data[i] - tempr;
		data[j + 1] = data[i + 1] - tempi;
		data[i] += tempr;
		data[i + 1] += tempi;
	    }
	    wr = (wtemp = wr) * wpr - wi * wpi + wr;
	    wi = wi * wpr + wtemp * wpi + wi;
	}
	mmax = istep;
    }

    /* normalisation section */

    for (i = 0; i < n; i++)
	data[i] *= tmp;
}


void
rfft (double* data, int N, int isign)
{
    /* main section */

    double c1 = 0.5, c2;
    double theta = M_PI / (double) (N >> 1);
    double wpr = -2.0 * sqr (sin (0.5 * theta));
    double wpi = sin (theta);
    double wr = 1.0 + wpr;
    double wi = wpi;
    double wtemp;
    const double tmp = isign == 1 ? M_SQRT1_2 : M_SQRT2;
    int i, i1, i2, i3, i4;
    double h1r, h1i, h2r, h2i;
	int	k;

    for (i = 1; i < N >> 2; i++) {

    if (isign == 1) {
	c2 = -0.5;
	fft (data, N >> 1, +1);
    } else {
	c2 = +0.5;
	theta = -theta;
    }

	i1 = i + i;
	i2 = i1 + 1;
	i3 = N - i1;
	i4 = i3 + 1;
	h1r = c1 * (data[i1] + data[i3]);
	h1i = c1 * (data[i2] - data[i4]);
	h2r = -c2 * (data[i2] + data[i4]);
	h2i = c2 * (data[i1] - data[i3]);
	data[i1] = h1r + wr * h2r - wi * h2i;
	data[i2] = h1i + wr * h2i + wi * h2r;
	data[i3] = h1r - wr * h2r + wi * h2i;
	data[i4] = -h1i + wr * h2i + wi * h2r;
	wr = (wtemp = wr) * wpr - wi * wpi + wr;
	wi = wi * wpr + wtemp * wpi + wi;
    }

    if (isign == 1) {
	data[0] = (h1r = data[0]) + data[1];
	data[1] = h1r - data[1];
    } else {
	data[0] = c1 * ((h1r = data[0]) + data[1]);
	data[1] = c1 * (h1r - data[1]);
	fft (data, N >> 1, -1);
    }

    /* normalisation section */

    for (k = 0; k < N; k++)
	data[k] *= tmp;
}

