#include <stdio.h>
#include <string.h>

#define MAX_JOBS 1000
#define MAX_OPS 1000

void convertJobs(int numJobs, int numMachines,
                 int dataArray[][2 * numMachines]) {

  // Set a 2D array to store the job operations and their corresponding machine
  // and time
  int jobs[MAX_JOBS][MAX_OPS][2];

  // Fill the jobs array with the machine and time data from dataArray
  for (int j = 0; j < numJobs; j++) {
    // Each job has num_machines operations, and each operation has a machine
    // and time
    for (int op = 0; op < numMachines; op++) {
      jobs[j][op][0] = dataArray[j][2 * op];
      jobs[j][op][1] = dataArray[j][2 * op + 1];
    }
  }

  // Print the jobs array to verify the correct parsing
  for (int j = 0; j < numJobs; j++) {
    printf("Job %d:\n", j);
    for (int op = 0; op < numMachines; op++) {
      printf("  Op %d -> Machine %d, Time %d\n", op, jobs[j][op][0],
             jobs[j][op][1]);
    }
  }
}