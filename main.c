#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <math.h>
#include <errno.h>
#include <string.h>

int		fd;

#include "tc64.h"
#include "fft.h"
#include "quant.h"

#define BUF_SIZE 256
#define DECAY	0.7


#define SWAP(a,b) 	tempr=(a);(a)=(b);(b)=tempr
#define Tpi	 	6.28318530717959
#define PI		3.141592653589793

typedef struct
{
	double	val;
	int		pos;
} topT;

void	hammWin (double *signal, unsigned long n)
{
	float	a = 0.54;
	float	b;
	unsigned long	i;

	b = 1.0 - a;

	for (i = 0; i < n; i++)
	{
		signal[i] *= a + b*sin (Tpi*i/((n-1)<<2));
		signal[2*n-i] *=  a + b*sin (Tpi*i/((n-1)<<2));
	}
}

void init_oss (const char* name)
{
    int openmode = O_RDWR;
	int	channels = 2;
    const int want_format = AFMT_S16_NE;
	int	buffer_size;
	int	s  = 8;
    int arg = 0x00020000 + s;
    int tmp_format = AFMT_S16_NE;
    int tmp_stereo = 1;
    int tmp_rate = 5512;


    fd = open (name, openmode, 0);
    if (fd == -1) {
		fprintf (stderr, "Fatal: failed to open soundcard\n");
		exit (-1);
    }

    if (ioctl (fd, SNDCTL_DSP_SETFRAGMENT, &arg)) {
		fprintf (stderr, "Fatal: cant set framgent\n");
		exit (-1);
    }

    if (ioctl (fd, SNDCTL_DSP_SETFMT, &tmp_format) == -1) {
		fprintf (stderr, "Fatal: couldnt set format\n");
		exit (-1);
    }


    if (ioctl (fd, SNDCTL_DSP_STEREO, &tmp_stereo) == -1) {
		fprintf (stderr, "Fatal: couldnt get mono\n");
		exit (-1);
    }


    if (ioctl (fd, SNDCTL_DSP_SPEED, &tmp_rate) == -1) {
		fprintf (stderr, "Fatal: couldnt get rate\n");
		exit (-1);
    }

	printf ("Got audio rate %ld\n", tmp_rate);

    if (ioctl (fd, SNDCTL_DSP_GETBLKSIZE, &buffer_size) == -1) {
		fprintf (stderr, "Fatal: couldnt get buffer size\n");
		exit (-1);
    }

	printf ("Got buffer size %ld\n", buffer_size);
}

int	main (int argc, char **argv)
{
	int		x, y, i, j;
	double	signal[BUF_SIZE];
	float	v[16];
	signed short	buf[BUF_SIZE];


	init_oss ("/dev/dsp");

	pc = (pixel64_t *) malloc (PCSZ);
	ds_pc = (pixel64_t *) malloc (DSSZ);
	ptc_open ("i2pi-xcro", WIDTH/DOWNSAMPLE, HEIGHT/DOWNSAMPLE);

	

	x = 0;
	while (1)
	{
		read (fd, buf, sizeof (signed short) * BUF_SIZE);

		memset (pc, 0x50, PCSZ);

		for (i=0; i<BUF_SIZE; i++)
		{
			signal[i] = buf[i] / 32768.0;
			y = signal[i] * HEIGHT;
			if ((i>0) && (i<WIDTH) && (y>=0) && (y<HEIGHT))
			pxrgb (pc, i,y, 0x80, 0x80, 0x80);	
		}
		hammWin (signal, BUF_SIZE/2);
		rfft (signal, BUF_SIZE , 1);  

		for (i=0; i<16; i++)
		{
			v[i] = 0.0;
			for (j=i*4; j<(i+1)*4; j++)
			{
				v[i] += sqrt((signal[2*i]*signal[2*i]) +
						(signal[2*i+1]*signal[2*i+1]));
			}
		}
		
		for (i=0; i<16; i++)
		{
			for (j=0; j<16; j++)
			{
				/*
				if ((i>0) && (i<WIDTH) && (y>=0) && (y<HEIGHT))
				tcono[i*16 + j + WIDTH*1] = v[i]*15;
				tcono[i*16 + j + WIDTH*2] = v[i]*15;
				tcono[i*16 + j + WIDTH*3] = v[i]*15;
				tcono[i*16 + j + WIDTH*4] = v[i]*15;
				*/
			}
		}

		for (i=1; i <= BUF_SIZE/4; i++)
		{
			y =signal[2*i] * HEIGHT;
			if (y<0) y = 0; else
			if (y>HEIGHT-2) y=HEIGHT-1;
			for (j=0; j<y; j++)
			{
				/*
				tcono[4*i + (HEIGHT-j)*WIDTH] = 0xffff00;
				tcono[4*i + 1 + (HEIGHT-j)*WIDTH] = 0xf0f000;
				tcono[4*i + 2 + (HEIGHT-j)*WIDTH] = 0xffff00;
				*/
			}
		}
		downsample(pc, ds_pc);
		ptc_update (ds_pc);
	}

	return (0);
}
