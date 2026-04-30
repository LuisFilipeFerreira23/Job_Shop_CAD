#include <omp.h>
#include <stdio.h>
#include <string.h>

int readFiles(const char *fileName, char *finalString) {
  FILE *fptr;

  printf("[LOG] readFiles() called with fileName: %s\n", fileName);

  if (fileName == NULL) {
    printf("[LOG] Error: fileName is NULL\n");
    printf("Error opening file!\n");
    return -1;
  }

  // Open a file in read mode
  printf("[LOG] Attempting to open file: %s\n", fileName);
  fptr = fopen(fileName, "r");

  // Check if the file was opened successfully
  if (fptr == NULL) {
    printf("[LOG] Error: Failed to open file: %s\n", fileName);
    printf("Error opening file!\n");
    return -1;
  }

  printf("[LOG] File successfully opened: %s\n", fileName);

  // Initialize the string to empty
  finalString[0] = '\0';

  // Store each line of the content of the file
  char myString[100];
  int lineCount = 0;

  // Read the content and print it
  while (fgets(myString, 100, fptr)) {
    lineCount++;
    printf("[LOG] Read line %d: %s", lineCount, myString);
    strcat(finalString, myString);
  }

  printf("[LOG] Total lines read: %d\n", lineCount);
  printf("[LOG] File content:\n%s\n", finalString);

  // Close the file
  fclose(fptr);
  printf("[LOG] File closed successfully\n");

  return 0;
}