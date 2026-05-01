/* #include <stdio.h>
#include <string.h>

#define MAX_JOBS 100
#define MAX_OPS 100

void fileToArray(const char *finalString, int numJobs, int numMachines,
                 int dataArray[][2 * numMachines]) {

  int num_count = 0;
  int i = 0;
  // Length of the input string
  int len = strlen(finalString);

  // Extract num_jobs and num_machines first
  while (i < len && num_count < 2) {
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        i++;
      }
      num_count++;
    } else {
      i++;
    }
  }

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

  // Just to check the contents of the dataArray and if they are formating
  // correctly
  printf("\nData Array:\n");
  int counter = 0;
  printf("numJobs: %d, numMachines: %d\n", numJobs, numMachines);
  for (int j = 0; j < numJobs; j++) {
    printf("bolas");
    for (int k = 0; k < 2 * numMachines; k++) {
      printf("Teste: %d", dataArray[j][k]);
      counter++;
      if (counter == 2) {
        printf("\n");
        counter = 0;
      }
    }
    printf("\n");
  }
} */