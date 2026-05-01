#include "Aux_Functions/AuxFunctions.h"
#include <omp.h>
#include <stdio.h>
#include <time.h>   // for clock_t, clock()
#include <unistd.h> // for sleep()

#define BILLION 1000000000.0
#define MAX_BUFFER 10000
#define MAX_JOBS 100
#define MAX_OPS 100

int numJobs = 0, numMachines = 0;

void fileToArray(const char *finalString) {

  int num_count = 0;
  int i = 0;
  // Length of the input string
  int len = strlen(finalString);

  // Extract num_jobs and num_machines first
  while (i < len && num_count < 2) {
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      int val = 0;
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }
      if (num_count == 0) {
        numJobs = val;
      } else if (num_count == 1) {
        numMachines = val;
      }
      num_count++;
    } else {
      i++;
    }
  }

  // Initialize the array to store the file content (not implemented yet)
  int dataArray[numJobs][2 * numMachines];
  memset(dataArray, 0, sizeof(dataArray));

  // Parse the remaining numbers into pairs
  int job_idx = 0;
  int pair_count = 0;

  // Loop through the string to extract pairs of numbers for each job
  while (i < len && job_idx < numJobs) {
    // Check if the current character is a digit
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      int val = 0;

      // Build the number until we hit a non-digit character
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }

      // Store in the array
      // pair_count tracks the position within the 200-int row
      dataArray[job_idx][pair_count] = val;
      pair_count++;

      // Once we have filled (2 * numMachines) for one job, move to next row
      if (pair_count == (2 * numMachines)) {
        job_idx++;
        pair_count = 0;
      }
    } else {
      i++;
    }
  }

  /*
   // Just to check the contents of the dataArray and if they are formating
   // correctly
   printf("\nData Array:\n");
   int counter = 0;
   for (int j = 0; j < numJobs; j++) {
     for (int k = 0; k < 2 * numMachines; k++) {
       printf("%d", dataArray[j][k]);
       counter++;
       if (counter == 2) {
         printf("\n");
         counter = 0;
       }
     }
     printf("\n");
   }
    */
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________

int main() {

  /* MANTÉM-SE PARA UMA FUTURA IMPLEMENTAÇÃO DE GERAÇÃO DE FICHEIROS DE ENTRADA
    printf("Select the number of jobs and machines for the entry file "
           "generation:\n");

    printf("Number of Jobs:\n ");
    scanf("%d", &numJobs);
    printf("Number of Machines:\n");
    scanf("%d", &numMachines);

    generateData(numJobs, numMachines);
    */

  // Start the timer
  clock_t start_clock = clock();

  // Initialize the string to empty
  char finalString[MAX_BUFFER] = "";

  // Call the function to read the file and store its content in finalString
  readFiles("./Entry_Files/Entry_Files_1.txt", finalString);

  printf("File content:\n%s\n", finalString);

  // Call the function to convert the file content to an array (not implemented
  // yet)
  fileToArray(finalString);

  // Stop the timer
  clock_t end_clock = clock();

  // time_spent = end_clock - start_clock divided by CLOCKS_PER_SEC to get the
  // time in seconds
  double time_spent = (end_clock - start_clock) / CLOCKS_PER_SEC;

  // Print the time spent in seconds
  printf("Execution time: %f seconds \n", time_spent);

  return 0;
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________