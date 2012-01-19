#include "quant.h"

void	init_km (int n, int vals)
{
	mean = (float *) malloc (sizeof (float) * vals);
	cluster = (int *) malloc (sizeof (int) * n);
	points_in_cluster = (int *) malloc (sizeof (int) * vals);
}

void	km_quantize (float *v, int n, int vals)
{
	/*
	** Performs K-Means clustering to quantize to minimise
	** error. Hopefully.	
	*/

	int		i, c, step;


	/*
	** Assign evey point to cluster 0
	*/

	for (i=0; i<n; i++)
	{
		cluster[i] = 0;
	}

	/*
	** Guess at the cluster means, by taking
	** equally spaced values from the set
	*/

	for (c=0; c<vals; c++)
	{
		mean[c] = v[(int)(c*n/(float)vals)];
		cluster[(int)(c*n/(float)vals)] = c;
	}

	for (step=0; step<10; step++)
	{
		/*
		** Assign each point to its euclidean nearest
		** cluster.
		*/

		for (i=0; i<n; i++)
		{
			for(c=0; c<vals; c++)
			{
				if (fabs(v[i] - mean[c]) <  fabs(v[i] - mean[cluster[i]]))
				{	
					/*
					** Assign to new cluster as its closer
					*/
					cluster[i] = c;
				}
			}		
		}

		/*
		** Find the new cluster means
		*/

		for (c=0; c<vals; c++)
		{	
			points_in_cluster[c] = 0;
		}

		for (i=0; i<n; i++)
		{
			points_in_cluster[cluster[i]]++;
			mean[cluster[i]] += v[i];	
		}

		for (c=0; c<vals; c++)
		{
			mean[c] /= (float) points_in_cluster[c];
		}	

	}

	/*
	** Assign each point the value of its cluster mean
	*/

	for (i=0; i<n; i++)
	{	
		v[i] = mean[cluster[i]];
	}
}
