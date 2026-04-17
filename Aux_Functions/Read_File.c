#include <omp.h>
#include <stdio.h>
#include <string.h>


int readFiles(const char *fileName, char *finalString) {
  FILE *fptr;

  if (fileName == NULL) {
    printf("Error opening file!\n");
    return -1;
  }

  // Open a file in read mode
  fptr = fopen(fileName, "r");

  // Check if the file was opened successfully
  if (fptr == NULL) {
    printf("Error opening file!\n");
    return -1;
  }

  // Initialize the string to empty
  finalString[0] = '\0';

  // Store each line of the content of the file
  char myString[100];

  // Read the content and print it
  while (fgets(myString, 100, fptr)) {
    strcat(finalString, myString);
  }

  // Close the file
  fclose(fptr);

  return 0;
}