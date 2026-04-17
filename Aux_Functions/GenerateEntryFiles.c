#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TIME 10

// Função para trocar valores (usado no shuffle)
void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

// Fisher-Yates shuffle
void shuffle(int arr[], int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    swap(&arr[i], &arr[j]);
  }
}

void generateData(int numJobs, int numMachines) {

  srand(time(NULL));

  FILE *f = fopen("./test.jss", "w");

  if (f == NULL) {
    printf("Error opening file!\n");
    return;
  }

  // Primeira linha
  printf("%d %d\n", numJobs, numMachines);
  fprintf(f, "%d %d\n", numJobs, numMachines);

  for (int j = 0; j < numJobs; j++) {

    int machines[numMachines];

    // Inicializar máquinas
    for (int i = 0; i < numMachines; i++) {
      machines[i] = i;
    }

    // Baralhar máquinas
    shuffle(machines, numMachines);

    // Generate operations and save them on the new file
    for (int i = 0; i < numMachines; i++) {
      int m = machines[i];
      int t = (rand() % MAX_TIME) + 1;
      fprintf(f, "%d %d ", m, t);

      printf("%d %d ", m, t);
    }

    fprintf(f, "\n");
    printf("\n");
  }

  fclose(f);
}