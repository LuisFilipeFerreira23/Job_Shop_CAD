#include <stdio.h>
#include <string.h>

#define MAX_JOBS 100
#define MAX_OPS 100

int fileToArray(const char *finalString, int (*dataArray)[200]) {
  int num_jobs = 0, num_machines = 0;
  int num_count = 0;
  int i = 0;
  // Length of the input string
  int len = strlen(finalString);

  // Extract num_jobs and num_machines first
  while (i < len && num_count < 2) {

    // Check if the current character is a digit
    if (finalString[i] >= '0' && finalString[i] <= '9') {
      int num = 0;

      // While we have digits, build the number
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9') {
        // Convert character to integer and build the number
        num = num * 10 + (finalString[i] - '0');
        i++;
      }
      // Store the first number as num_jobs and the second as num_machines
      if (num_count == 0)
        num_jobs = num;
      // Store the third number as num_machines
      else
        num_machines = num;
      num_count++;
    } else {
      i++;
    }
  }

  printf("Jobs: %d, Machines: %d\n", num_jobs, num_machines);

  // Parse the remaining numbers into pairs
  int job_idx = 0;
  int pair_count = 0;

  // Loop through the string to extract pairs of numbers for each job
  while (i < len && job_idx < num_jobs) {
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

      // Once we have filled (2 * num_machines) for one job, move to next row
      if (pair_count == (2 * num_machines)) {
        job_idx++;
        pair_count = 0;
      }
    } else {
      i++;
    }
  }

  /*
  // Just to check the contents of the dataArray and if they are formating
  correctly printf("\nData Array:\n"); int counter = 0; for (int j = 0; j <
  num_jobs; j++) { for (int k = 0; k < 2 * num_machines; k++) { printf("%d",
  dataArray[j][k]); counter++; if (counter == 2)
          {
              printf("\n");
              counter = 0;
          }

      }
      printf("\n");
  }
  */

  // Set a 2D array to store the job operations and their corresponding machine
  // and time
  int jobs[MAX_JOBS][MAX_OPS][2];

  // Fill the jobs array with the machine and time data from dataArray
  for (int j = 0; j < num_jobs; j++) {
    // Each job has num_machines operations, and each operation has a machine
    // and time
    for (int op = 0; op < num_machines; op++) {
      jobs[j][op][0] = dataArray[j][2 * op];
      jobs[j][op][1] = dataArray[j][2 * op + 1];
    }
  }
  /*
    // Print the jobs array to verify the correct parsing
    for (int j = 0; j < num_jobs; j++) {
      printf("Job %d:\n", j);
      for (int op = 0; op < num_machines; op++) {
        printf("  Op %d -> Machine %d, Time %d\n", op, jobs[j][op][0],
               jobs[j][op][1]);
      }
    }
   */
  return 0;
}