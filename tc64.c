#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "tinyptc.h"
#include "tc64.h"

pixel64_t    *pc;
pixel64_t    *ds_pc;

#undef DEBUG

void	print_pixel (pixel64_t p)
{
	printf ("p.tc64         = %016llx\n", p.tc64);
	printf ("p.tc32.h.c     = %08x\n", p.tc32.h.c);
	printf ("p.tc32.h.rgb.b = %02x                %d\n", p.tc32.h.rgb.b, p.tc32.h.rgb.b);
	printf ("p.tc32.h.rgb.g =   %02x              %d\n", p.tc32.h.rgb.g, p.tc32.h.rgb.g);
	printf ("p.tc32.h.rgb.r =     %02x            %d\n", p.tc32.h.rgb.r, p.tc32.h.rgb.r);
	printf ("p.tc32.h.rgb.a =       %02x          %d\n", p.tc32.h.rgb.a, p.tc32.h.rgb.a);
	printf ("p.tc32.l.c     =         %08x\n", p.tc32.l.c);
	printf ("p.tc32.l.rgb.b =         %02x        %d\n", p.tc32.l.rgb.b, p.tc32.l.rgb.b);
	printf ("p.tc32.l.rgb.g =           %02x      %d\n", p.tc32.l.rgb.g, p.tc32.l.rgb.g);
	printf ("p.tc32.l.rgb.r =             %02x    %d\n", p.tc32.l.rgb.r, p.tc32.l.rgb.r);
	printf ("p.tc32.l.rgb.a =               %02x  %d\n", p.tc32.l.rgb.a, p.tc32.l.rgb.a);
}

void p64_mix50 (pixel64_t *a, pixel64_t *b)
{
	#define RBMASK64  0x00ff00ff00ff00ffll
	#define GMASK64	  0x0000ff000000ff00ll

	a->tc64 = ((((a->tc64 & RBMASK64) + (b->tc64 & RBMASK64)) >> 1) & RBMASK64) +
			  ((((a->tc64 & GMASK64 ) + (b->tc64 & GMASK64 )) >> 1) & GMASK64);
}


inline
void tc32_mix50 (uint32_t *a, uint32_t *b)
{
	#define RBMASK32 0x00ff00ff
	#define GMASK32  0x0000ff00
	
	*a = ((((*a & RBMASK64) + (*b & RBMASK64)) >> 1) & RBMASK64) +
	     ((((*a & GMASK64 ) + (*b & GMASK64 )) >> 1) & GMASK64);
}

void	pxrgb (pixel64_t *pc, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	int _x, _y;
	int W = WIDTH>>1;
	pixel64_t	a;
/*
	if (x < 0) return;
	if (x >= WIDTH) return;
	if (y < 0) return;
	if (y >= HEIGHT) return;
	*/

	_y = y;
	_x = x >> 1;

	a.tc32.l.rgb.r = r;
	a.tc32.l.rgb.g = g;
	a.tc32.l.rgb.b = b;

	if (x%2)
	{
		tc32_mix50 (&pc[_x + _y*W].tc32.l.c, &a.tc32.l.c);
	} else
	{
		tc32_mix50 (&pc[_x + _y*W].tc32.h.c, &a.tc32.l.c);
	}
}

inline
void	getpxrgb (pixel64_t *pc, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b)
{
	int _x, _y;
	int W = WIDTH>>1;
/*
	if (x < 0) return;
	if (x >= WIDTH) return;
	if (y < 0) return;
	if (y >= HEIGHT) return;
	*/

	_y = y;
	_x = x >> 1;

	if (x%2)
	{
		*r = pc[_x + _y*W].tc32.l.rgb.r;
		*g = pc[_x + _y*W].tc32.l.rgb.g;
		*b = pc[_x + _y*W].tc32.l.rgb.b;
	} else
	{
		*r = pc[_x + _y*W].tc32.h.rgb.r;
		*g = pc[_x + _y*W].tc32.h.rgb.g;
		*b = pc[_x + _y*W].tc32.h.rgb.b;
	}
}


void	line (pixel64_t *pc, int x, int y, int x1, int y1)
{
	int		w, h;
	int		X,Y;
	double	d;

	w = x1 - x;
	h = y1 - y;	

	if (abs(w)>=abs(h))
	{
		if (!w)	
		{
		} else
		{
			if (x1 < x)
			{
				X = x1;
				x1 = x;
				x = X;
				Y = y1;
				y1 = y;
				y = Y;
			}

			d = (y1-y) / (double)(x1-x);
			for(X=x; X<x1; X++)
			{
				Y = y + (X-x) * d;
				pxrgb (pc, X,Y, 255, 255, 255);
			}
		}
	} else
	{
		if (y1 < y)
		{
			Y = y1;
			y1 = y;
			y = Y;
			X = x1;
			x1 = x;
			x = X;
		}

		d = (x1-x) / (double)(y1-y);
		for(Y=y; Y<y1; Y++)
		{
			X = x + (Y-y) * d;
			pxrgb (pc, X,Y, 255, 255, 255);
		}
	}
}

void	box (pixel64_t *pc, int x, int y, int x1, int y1)
{
	line (pc, x+1,y, x1, y);
	line (pc, x+1,y, x, y1);
	line (pc, x1+1,y1, x, y1);
	line (pc, x1+1,y1, x1, y);

	line (pc, x+1,y, x1,y1);
	line (pc, x1+1,y, x,y1);
}


void	linergb (pixel64_t *pc, int x, int y, int x1, int y1, char r, char g, char b)
{
	int		w, h;
	int		X,Y;
	double	d;

	w = x1 - x;
	h = y1 - y;	

	if (abs(w)>=abs(h))
	{
		if (!w)	
		{
		} else
		{
			if (x1 < x)
			{
				X = x1;
				x1 = x;
				x = X;
				Y = y1;
				y1 = y;
				y = Y;
			}

			d = (y1-y) / (double)(x1-x);
			for(X=x; X<x1; X++)
			{
				Y = y + (X-x) * d;
				pxrgb (pc, X,Y, r,g,b);
			}
		}
	} else
	{
		if (y1 < y)
		{
			Y = y1;
			y1 = y;
			y = Y;
			X = x1;
			x1 = x;
			x = X;
		}

		d = (x1-x) / (double)(y1-y);
		for(Y=y; Y<y1; Y++)
		{
			X = x + (Y-y) * d;
			pxrgb (pc, X,Y, r,g,b);
		}
	}
}

void	boxrgb (pixel64_t *pc, int x, int y, int x1, int y1, char r, char g, char b)
{
	linergb (pc, x+1,y, x1, y,r,g,b);
	linergb (pc, x+1,y, x, y1,r,g,b);
	linergb (pc, x1+1,y1, x, y1,r,g,b);
	linergb (pc, x1+1,y1, x1, y,r,g,b);
}



void	downsample (pixel64_t *big, pixel64_t *small)
{
	int	x,y;
	int	X,Y;
	int W = WIDTH>>1;
	pixel64_t	*sp, *bp;
	unsigned short r,g,b;
	unsigned char R,G,B;

	sp = small;
	bp = big;

#ifdef DEBUG
printf ("sizeof(uint64_t) = %ld, sizeof (pixel64_t) = %ld\n", sizeof(uint64_t), sizeof (pixel64_t));
#endif


	for (y=0; y<HEIGHT/DOWNSAMPLE; y++)
	for (x=0; x<WIDTH/DOWNSAMPLE; x++)
	{
#ifdef DEBUG
		printf ("(%d,%d)\n", x/2,y);
#endif
		r = 0;
		g = 0;
		b = 0;
		for (Y=y*DOWNSAMPLE; Y<(y+1)*DOWNSAMPLE; Y++) 
		for (X=x*DOWNSAMPLE; X<(x+1)*DOWNSAMPLE; X+=2) 
		{
//			getpxrgb (pc, X,Y, &R,&G,&B);
			R = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.r;
			G = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.g;
			B = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.b;
#ifdef DEBUG
			printf ("\t(%d,%d) : %d, %d, %d\n", X,Y, R,G,B);
#endif
			r += R;
			g += G;
			b += B;

//			if (X < WIDTH - 1)
			{
//				getpxrgb (pc, X+1,Y, &R,&G,&B);
				R = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.r;
				G = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.g;
				B = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.b;
#ifdef DEBUG
				printf ("\t(%d,%d) : %d, %d, %d\n", X+1,Y, R,G,B);
#endif
				r += R;
				g += G;
				b += B;
			}
		}
		r /= (float) DOWNSAMPLE*DOWNSAMPLE;
		g /= (float) DOWNSAMPLE*DOWNSAMPLE;
		b /= (float) DOWNSAMPLE*DOWNSAMPLE;

		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.l.rgb.r = r;
		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.l.rgb.g = g;
		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.l.rgb.b = b;

		x++;
	
		r = 0;
		g = 0;
		b = 0;
		for (Y=y*DOWNSAMPLE; Y<(y+1)*DOWNSAMPLE; Y++) 
		for (X=x*DOWNSAMPLE; X<(x+1)*DOWNSAMPLE; X+=2) 
		{
//			getpxrgb (pc, X,Y, &R,&G,&B);
			R = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.r;
			G = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.g;
			B = pc[(X>>1) + Y*(WIDTH>>1)].tc32.h.rgb.b;
#ifdef DEBUG
			printf ("\t(%d,%d) : %d, %d, %d *\n", X,Y, R,G,B);
#endif
			r += R;
			g += G;
			b += B;

//			if (X < WIDTH - 1)
			{
//				getpxrgb (pc, X+1,Y, &R,&G,&B);
				R = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.r;
				G = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.g;
				B = pc[(X>>1) + Y*(WIDTH>>1)].tc32.l.rgb.b;
#ifdef DEBUG
				printf ("\t(%d,%d) : %d, %d, %d *\n", X+1,Y, R,G,B);
#endif
				r += R;
				g += G;
				b += B;
			}
		}
		r /= (float) DOWNSAMPLE*DOWNSAMPLE;
		g /= (float) DOWNSAMPLE*DOWNSAMPLE;
		b /= (float) DOWNSAMPLE*DOWNSAMPLE;

		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.h.rgb.r = r;
		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.h.rgb.g = g;
		small[(x>>1)+y*((WIDTH/DOWNSAMPLE)>>1)].tc32.h.rgb.b = b;
	}
}


void	dot (int x, int y, int min_r, int max_r, float h)
{
	int	i, j;
	float R;
	int		c;

	h += 0.2;

	for (i=0; i<max_r; i++)
	for (j=0; j<max_r; j++)
	{
		R = sqrt (i*i + j*j);

		if (R < min_r)
		{
			pxrgb (pc, x+j, y+i, 255, 255, 255);
			pxrgb (pc, x-j, y+i, 255, 255, 255);
			pxrgb (pc, x+j, y-i, 255, 255, 255);
			pxrgb (pc, x-j, y-i, 255, 255, 255);
		} else
		if (R < max_r)
		{
			R -= min_r;
			R /= (max_r - min_r);
			R = 1.0 - exp(-h * (1.0 -R));
			c = R * 255;
			pxrgb (pc, x+j, y+i, c,c,c); 
			pxrgb (pc, x-j, y+i, c,c,c);
			pxrgb (pc, x+j, y-i, c,c,c);
			pxrgb (pc, x-j, y-i, c,c,c);
		}
	}
}
