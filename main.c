#include <stdio.h>
#include <omp.h>
#include <time.h>      // for clock_t, clock()
#include <unistd.h>    // for sleep()
#include "Aux_Functions/AuxFunctions.h"

#define BILLION  1000000000.0

int main() {

    // Measure the execution time of the code block
    struct timespec start, end;
 
    // Start the timer
    clock_gettime(CLOCK_REALTIME, &start);
    
/*     int N = 10;
       
    #pragma omp parallel for
    for(int i=1; i<=N; i++) {
        
        #ifdef _OPENMP
            int threadid = omp_get_thread_num();
            int numthreads = omp_get_num_threads();
        #else
            int threadid = 0;
            int numthreads = 0;
        #endif

        printf("Contador i %d na thread %d, num threads %d\n", i, threadid, numthreads );
    } */
    
    clock_gettime(CLOCK_REALTIME, &end);
 
    // time_spent = end - start
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;
 
    // Print the time spent in seconds
    printf("Execution time: %f seconds \n", time_spent);

    // Initialize the string to empty
    char finalString[200] = "";
    
    // Call the function to read the file and store its content in finalString
    readFiles("./Entry_Files/Entry_File_2.txt", finalString);

    return 0;
}

