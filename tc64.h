#ifndef __TC64_H__
#define __TC64_H__

#include "tinyptc.h"

#define uint64_t unsigned long long
#define uint32_t unsigned int 
#define uint8_t  unsigned char

#define WIDTH	3200
#define HEIGHT	1800
#define PCSZ	((WIDTH>>1)*HEIGHT*sizeof(uint64_t))

#define DOWNSAMPLE 4
#define DS_WIDTH (WIDTH/DOWNSAMPLE)
#define DS_HEIGHT (HEIGHT/DOWNSAMPLE)
#define DSSZ 	(PCSZ/(DOWNSAMPLE*DOWNSAMPLE))

typedef union pixel64_t
{
	uint64_t	tc64;
	struct {
		union {
			uint32_t	c;
			struct {
				uint8_t	b, g, r, a;
			} rgb;
		} l;
		union {
			uint32_t	c;
			struct {
				uint8_t	b, g, r, a;
			} rgb;
		} h;
	} tc32;
} pixel64_t;

extern pixel64_t	*pc;
extern pixel64_t	*ds_pc;

void	print_pixel (pixel64_t p);
void 	p64_mix50 (pixel64_t *a, pixel64_t *b);
void 	tc32_mix50 (uint32_t *a, uint32_t *b);
void	pxrgb (pixel64_t *pc, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void	line (pixel64_t *pc, int x, int y, int x1, int y1);
void	box (pixel64_t *pc, int x, int y, int x1, int y1);
void	linergb (pixel64_t *pc, int x, int y, int x1, int y1, char r, char g, char b);
void    linemovie (pixel64_t *pc, int x, int y, int x1, int y1, float Q, int movie_y);
void    tc_to_ppm (pixel64_t *tc);
void	boxrgb (pixel64_t *pc, int x, int y, int x1, int y1, char r, char g, char b);
void	downsample (pixel64_t *big, pixel64_t *small);
void    dot (int x, int y, int min_r, int max_r, float h);

#endif /* __TC64_H__ */
