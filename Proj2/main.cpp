#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#ifndef NUMT
#define NUMT    10
#endif

#ifndef NUMNODES
#define NUMNODES    3
#endif

const float N = 2.5f;
const float R = 1.2f;

float Height( int, int );	// function prototype

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float rn = pow( fabs(R), (double)N );
	float r = rn - xn - yn;

	if( r <= 0. )
        return 0.;
	float height = pow( r, 1./(double)N );
	return height;
}

int main( int argc, char *argv[ ] )
{

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

    float halfTileArea = fullTileArea/2;
    float fourthTileArea = fullTileArea/4;

    //fprintf(stdout, "Full Tile Area: %f", fullTileArea);
    //fprintf(stdout, "Half Tile Area: %f", halfTileArea);
    //fprintf(stdout, "Fourth Tile Area: %f\n\n", fourthTileArea);

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for-loop and a reduction:

    float volume = 0.0f;
    double  maxPerformance = 0.;  

    double time0 = omp_get_wtime();

    #pragma omp parallel for default(none) shared(fourthTileArea, halfTileArea, fullTileArea) reduction(+ : volume)
    for( int i = 0; i < NUMNODES*NUMNODES; i++ )
    {
        int iu = i % NUMNODES;
        int iv = i / NUMNODES;
        float z = Height( iu, iv );
        
        //Check if node is a corner node
        if(iv == 0 && iu == 0 || iv == NUMNODES-1 && iu == NUMNODES-1 || iv == NUMNODES-1 && iu == 0 || iv == 0 && iu == NUMNODES-1){
            //printf("Corner Node, iu: %d, iv: %d, Height: %f\n", iu, iv, z);
            volume += (z * fourthTileArea);

        }else if(iv == 0 || iv == NUMNODES-1 || iu == 0 || iu == NUMNODES-1){ // Check if node is an edge node
            //printf("Edge Node, iu: %d, iv: %d, Height: %f\n", iu, iv, z);
            volume += (z * halfTileArea);

        }else{
            //printf("Full Node, iu: %d, iv: %d, Height: %f\n", iu, iv, z);
            volume += (z * fullTileArea);

        }
    }

    double time1 = omp_get_wtime();
    double megaHeightsPerSecond = (double)NUMNODES*NUMNODES / ( time1 - time0 ) / 1000000.;
    if( megaHeightsPerSecond > maxPerformance )
            maxPerformance = megaHeightsPerSecond;

    volume *= 2; // Doubling the volume of the positive 0-Height volume

    fprintf(stderr, "%2d, %d, %6.2f, %6.2lf\n",
                NUMT, NUMNODES*NUMNODES, volume, maxPerformance);
}