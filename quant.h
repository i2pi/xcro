#define MAX_SET_SIZE 65536

float	*mean;
int		*points_in_cluster;
int		*cluster;


void	km_quantize (float *v, int n, int vals);
void	init_km (int n, int vals);
