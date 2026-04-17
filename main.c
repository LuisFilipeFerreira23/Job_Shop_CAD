#include "Aux_Functions/AuxFunctions.h"
#include <omp.h>
#include <stdio.h>
#include <time.h>   // for clock_t, clock()
#include <unistd.h> // for sleep()

#define BILLION 1000000000.0
#define MAX_BUFFER 10000
int main() {

  int numJobs, numMachines;

  printf("Select the number of jobs and machines for the entry file "
         "generation:\n");

  printf("Number of Jobs:\n ");
  scanf("%d", &numJobs);
  printf("Number of Machines:\n");
  scanf("%d", &numMachines);

  generateData(numJobs, numMachines);

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

          printf("Contador i %d na thread %d, num threads %d\n", i, threadid,
     numthreads );
      } */

  // Where the various different implementations of the algorithm will be called
  // sequencialImplementation(numJobs, numMachines);

  // Parallel Implementation:
  // particionedParallelImplementation(x, y, z);
  // sharedMemoryParallelImplementation(x, y, z);

  // Stop the timer
  clock_gettime(CLOCK_REALTIME, &end);

  // time_spent = end - start
  double time_spent =
      (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  // Print the time spent in seconds
  printf("Execution time: %f seconds \n", time_spent);

  int numCharacters = numJobs * numMachines * 4 + 10;

  // Initialize the string to empty
  char finalString[MAX_BUFFER] = "";

  // Call the function to read the file and store its content in finalString
  readFiles("./test.jss", finalString);

  // Initialize the array to store the file content (not implemented yet)
  int dataArray[numJobs][2 * numMachines];
  memset(dataArray, 0, sizeof(dataArray));

  // Call the function to convert the file content to an array (not implemented
  // yet)
  fileToArray(finalString, numJobs, numMachines, dataArray);

  return 0;
}
