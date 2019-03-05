// Example Program
// Optimizes code for maximum speed
#pragma optimize( "2", on )
#include <stdio.h>
#include <omp.h>
#include <math.h>
// Adds an additional library so that timeGetTime() can be used
#pragma comment(lib, "winmm.lib")
using namespace std;
const long int VERYBIG = 100000;
#define NUM_THREADS 4
// ***********************************************************************
int main(void)
{
	int i;
	long int j, sum;	
	double  total;
	double starttime, elapsedtime;
	double final_result = 0.0;
	// -----------------------------------------------------------------------
	// Output a start message
	printf("None Parallel Timings for %ld iterations\n\n", VERYBIG);
	// repeat experiment several times
	for (i = 0; i<6; i++)
	{
		// get starting time56 x CHAPTER 3 PARALLEL STUDIO XE FOR THE IMPATIENT
		starttime = omp_get_wtime();
		// reset check sum & running total
		sum = 0;
		total = 0.0;
		int number_of_threads;
		long double total_array[NUM_THREADS];
		long long sum_array[NUM_THREADS];
		#pragma omp parallel num_threads(NUM_THREADS) 
		 {
			int id_index =0;
			int id = omp_get_thread_num();
			int nthreads = omp_get_num_threads();
			#pragma omp master 
				{
					number_of_threads = nthreads;
					
				}
			#pragma omp barrier
			for (id_index = id, sum_array[id]= 0, total_array[id]=0; id_index<VERYBIG; id_index+=nthreads){
				// increment check sum
				sum_array[id] += 1;
				// Calculate first arithmetic series
				double sumx = 0.0;
				for (long int m = 0; m<id_index; m++)
					sumx = sumx + (double)m;
				// Calculate second arithmetic series
				double sumy = 0.0;
				for (long int k = id_index; k>0; k--)
					sumy = sumy + (double)k;

				if (sumx > 0.0)total_array[id] += 1.0 / sqrt(sumx);
				if (sumy > 0.0)total_array[id] += 1.0 / sqrt(sumy);
			}
		}
		// get ending time and use it to determine elapsed time
		elapsedtime = omp_get_wtime() - starttime;
		// report elapsed time
		
		for(int b=0;b<number_of_threads;b++){
			final_result += total_array[b];
			sum += sum_array[b];
		}
		
		printf("Time Elapsed % 10d mSecs Total = %lf Check Sum = %ld\n",
			// (int)elapsedtime, total, sum);
			(int)elapsedtime, final_result, sum);
		final_result = 0;
	}
	// return integer as required by function header
	return 0;

}
// **********************************************************************
