#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#define _USE_MATH_DEFINES

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;
const float DEER_HUNTED_PER_HUNTER = 1.0;
const float HUNTER_PER_DEER = 0.25;
const int HUNT_MONTH_BEGIN = 7; 
const int HUNT_MONTH_END = 10;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

unsigned int seed = 0;

int	NowYear;		// 2022 - 2027
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumHunter; // number of hunters this month

void Deer();
void Grain();
void Watcher();
void MyAgent();

float
Ranf( unsigned int *seedp,  float low, float high );
int
Ranf( unsigned int *seedp, int ilow, int ihigh );

float
SQR( float x );

int main(){

    // starting date and time:
    NowMonth =    0;
    NowYear  = 2022;

    // starting state (feel free to change this if you want):
    NowNumDeer = 1;
    NowHeight =  1.;

    // calculate starting environmental parameters
    float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

    float temp = AVG_TEMP - AMP_TEMP * cos( ang );
    NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
    NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
    if( NowPrecip < 0. )
        NowPrecip = 0.;

    omp_set_num_threads( 4 );	// same as # of sections // Set to 4
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            Deer( );
        }

        #pragma omp section
        {
            Grain( );
        }

        #pragma omp section
        {
            Watcher( );
        }

        #pragma omp section
        {
            MyAgent( );	// your own
        }
    }       // implied barrier -- all functions must return in order
        // to allow any of them to get past here
}

void Deer(){

    while( NowYear < 2028 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        int nextNumDeer = NowNumDeer;
        int carryingCapacity = (int)( NowHeight );
        if( nextNumDeer < carryingCapacity )
            nextNumDeer++;
        else
            if( nextNumDeer > carryingCapacity )
                nextNumDeer--;

        // hunting season culling
        nextNumDeer -= NowNumHunter * DEER_HUNTED_PER_HUNTER;

        if( nextNumDeer < 0 )
            nextNumDeer = 0;

        // DoneComputing barrier:
        #pragma omp barrier
        NowNumDeer = nextNumDeer;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }


}

void Grain(){

    while( NowYear < 2028 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );

        float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

        if( nextHeight < 0. ) nextHeight = 0.; 

        // DoneComputing barrier:
        #pragma omp barrier
        NowHeight = nextHeight;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void MyAgent(){
    while( NowYear < 2028 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        int currentMonth = NowMonth % 12;    // 0-11
        int nextNumHunter = 0;

        if(currentMonth >= HUNT_MONTH_BEGIN && currentMonth < HUNT_MONTH_END)  // Hunters hunt deer during hunting season [HUNT_MONTH_BEGIN, HUNT_MONTH_END)
            nextNumHunter = (int) NowNumDeer * HUNTER_PER_DEER + 1;

        // DoneComputing barrier:
        #pragma omp barrier
        NowNumHunter = nextNumHunter;

        // DoneAssigning barrier:
        #pragma omp barrier

        // DonePrinting barrier:
        #pragma omp barrier
    }
}

void Watcher(){

    while( NowYear < 2028 )
    {
        // compute a temporary next-value for this quantity
        // based on the current state of the simulation:
        // DoneComputing barrier:
        #pragma omp barrier

        // DoneAssigning barrier:
        #pragma omp barrier
        // Print Results (temperature, precipitation, number of deer, height of the grain, own choice quantity)
        fprintf(stderr, "%d, %d, %6.2f, %6.2f, %d, %6.2f, %d\n",
                NowYear, NowMonth, (5./9.)*(NowTemp-32.), NowPrecip*2.54, NowNumDeer, NowHeight*2.54, NowNumHunter);

        // Update Environmental Parameters
        NowMonth++;
        if(NowMonth % 12 == 0){
            NowYear++;
        }

        float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
        if( NowPrecip < 0. )
            NowPrecip = 0.;

        // DonePrinting barrier:
        #pragma omp barrier
    }
}


float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r(seedp);            // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}


float
SQR( float x )
{
        return x*x;
}