// Example Program
// Optimizes code for maximum speed
#pragma optimize( "2", on )
#include <stdio.h>
#include <omp.h>
// #include <windows.h>
// #include <mmsystem.h>
#include <math.h>
// Adds an additional library so that timeGetTime() can be used
#pragma comment(lib, "winmm.lib")
using namespace std;
const long int VERYBIG = 100000;
// ***********************************************************************
int main(void)
{
	int i;
	long int j, k, sum;
	double  total;
	double starttime, elapsedtime;
	double final_result = 0;
	// -----------------------------------------------------------------------
	// Output a start message
	printf("None Parallel Timings for %d iterations\n\n", VERYBIG);
	// repeat experiment several times
	for (i = 0; i<6; i++)
	{
		// get starting time56 x CHAPTER 3 PARALLEL STUDIO XE FOR THE IMPATIENT
		starttime = omp_get_wtime();
		// reset check sum & running total
		sum = 0;
		total = 0.0;
		// Work Loop, do some work by looping VERYBIG times
		
		#pragma omp parallel for reduction(+:sum, total) schedule(static, 1000)
		 
			for (j=0; j<VERYBIG;j++)
			{
				// increment check sum
				// printf("Thread number %d is working\n",id);
				sum += 1;
				// Calculate first arithmetic series
				//private access to sumx
                double sumx = 0.0;
                #pragma omp parallel for reduction (+:sumx)
				for (k = 0; k<j; k++)
					sumx = sumx + (double)k;
				// Calculate second arithmetic series
                //private access to sumy
				double sumy = 0.0;
                #pragma omp parallel for reduction (+:sumy)
				for (k = j; k>0; k--)
					sumy = sumy + (double)k;

				if (sumx > 0.0)total+= 1.0 / sqrt(sumx);
				if (sumy > 0.0)total+= 1.0 / sqrt(sumy);	
		 }
		 	
		// get ending time and use it to determine elapsed time
		elapsedtime = omp_get_wtime() - starttime;
		// report elapsed time
		
		printf("Time Elapsed % 10d mSecs Total = %lf Check Sum = %ld\n",
			// (int)elapsedtime, total, sum);
			(int)elapsedtime, total, sum);
	}
	// return integer as required by function header
	return 0;

}
// **********************************************************************
