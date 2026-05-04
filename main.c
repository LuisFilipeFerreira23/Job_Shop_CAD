#include "Aux_Functions/AuxFunctions.h"
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>   // for clock_t, clock()
#include <unistd.h> // for sleep()

#define BILLION 1000000000.0
#define MAX_BUFFER 10000
#define MAX_JOBS 100
#define MAX_OPS 100

int numJobs = 0, numMachines = 0;

// Funcão auxiliar para obter o máximo entre dois valores, usada para calcular o tempo de início de cada operação
int getMax(int a, int b)
{
  return (a > b) ? a : b;
}

void fileToArray(const char *finalString)
{

  int num_count = 0;
  int i = 0;
  // Length of the input string
  int len = strlen(finalString);

  // Extract num_jobs and num_machines first
  while (i < len && num_count < 2)
  {
    if (finalString[i] >= '0' && finalString[i] <= '9')
    {
      int val = 0;
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9')
      {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }
      if (num_count == 0)
      {
        numJobs = val;
      }
      else if (num_count == 1)
      {
        numMachines = val;
      }
      num_count++;
    }
    else
    {
      i++;
    }
  }

  // Arrays to store machine and duration for each operation of each job
  int jobMachines[numJobs][numMachines];        // which machine for each operation
  int operationDurations[numJobs][numMachines]; // duration for each operation

  // Initialize the arrays to zero and ensure they are properly sized based on
  // numJobs and numMachines
  memset(jobMachines, 0, sizeof(jobMachines));
  memset(operationDurations, 0, sizeof(operationDurations));

  // Parse the pairs [machine, duration] for each job
  int job_idx = 0;
  int operation_idx = 0;

  while (i < len && job_idx < numJobs)
  {
    if (finalString[i] >= '0' && finalString[i] <= '9')
    {
      int val = 0;
      while (i < len && finalString[i] >= '0' && finalString[i] <= '9')
      {
        val = val * 10 + (finalString[i] - '0');
        i++;
      }

      // Alternate between machine and duration
      if (operation_idx % 2 == 0)
      {
        // Even: this is a machine ID
        jobMachines[job_idx][operation_idx / 2] = val;
      }
      else
      {
        // Odd: this is a duration
        operationDurations[job_idx][operation_idx / 2] = val;
        operation_idx++; // only increment after we get both machine and
                         // duration

        if (operation_idx / 2 == numMachines)
        {
          job_idx++;
          operation_idx = 0;
        }
        continue;
      }
      operation_idx++;
    }
    else
    {
      i++;
    }
  }

  /********* ALGORITMO SIMPLES ******/
  int machinesFreeTime[numMachines];
  int jobsFreeTime[numJobs];
  int scheduleArray[numJobs][2 * numMachines];

  memset(machinesFreeTime, 0, sizeof(machinesFreeTime));
  memset(jobsFreeTime, 0, sizeof(jobsFreeTime));
  memset(scheduleArray, 0, sizeof(scheduleArray));

  int totalMakespan = 0;

  // Agenda de forma cega: J0 primeiro, depois J1, depois J2
  for (int j = 0; j < numJobs; j++)
  {
    for (int op = 0; op < numMachines; op++)
    {
      int machine = jobMachines[j][op];
      int duration = operationDurations[j][op];

      // O tempo de início é quando a máquina fica livre
      // E quando o Job termina a operação anterior (o que for maior).
      int startTime = getMax(machinesFreeTime[machine], jobsFreeTime[j]);
      int endTime = startTime + duration;

      scheduleArray[j][2 * op] = startTime;
      scheduleArray[j][2 * op + 1] = endTime;

      machinesFreeTime[machine] = endTime;
      jobsFreeTime[j] = endTime;

      if (endTime > totalMakespan)
      {
        totalMakespan = endTime;
      }
    }
  }

  /***********ALGORITMO SHIFTING BOTTLENECK*****************************/

  // Auxiliary array to track when each machine becomes free
  int machinesFreeTime[numMachines];
  memset(machinesFreeTime, 0, sizeof(machinesFreeTime));

  // Schedule array to store actual entry and exit times for each job and
  // machine
  int scheduleArray[numJobs][2 * numMachines];
  memset(scheduleArray, 0, sizeof(scheduleArray));

  // Regista a que segundo cada job termina asua operação atual e pode ir para a próxima operação, ou seja, o tempo em que o job fica livre para a próxima operação
  int jobsFreeTime[numJobs];
  memset(jobsFreeTime, 0, sizeof(jobsFreeTime));

  // Regista a operação atual de cada job, ou seja, qual é a próxima operação a ser agendada para cada job
  int currentOperation[numJobs];
  memset(currentOperation, 0, sizeof(currentOperation));

  int jobsDone = 0;      // Contador de quantos jobs já completaram todas as suas operações
  int totalMakespan = 0; // Tempo total que a fábrica demora a fazer tudo

  // Enquanto ainda houver jobs para completar, tenta agendar as operações
  while (jobsDone < numJobs)
  {

    /** Descobrir o bottleneck **/

    // Array para somar quantas tempo de trabalho faltam em cada máquina
    int machineRemainingTime[numMachines];
    memset(machineRemainingTime, 0, sizeof(machineRemainingTime));

    // Para cada job
    for (int j = 0; j < numJobs; j++)
    {
      // vemos as operações que eles ainda não fizeram
      for (int op = currentOperation[j]; op < numMachines; op++)
      {
        int machine = jobMachines[j][op]; // qual máquina para esta operação
        int duration = operationDurations[j][op];
        machineRemainingTime[machine] += duration; // somamos o tempo que falta para esta máquina trabalhar
      }
    }

    /** Escolher o melhor job para entrar**/

    int bestJob = -1;
    int bestStartTime = 9999999; // Iniciar com um valor muito alto para encontrar o mínimo

    // vamos testar, para cada job, o que aconteceria se o agendássemos agora
    for (int j = 0; j < numJobs; j++)
    {
      // só testamos os jobs que ainda não terminaram todas as suas operações
      if (currentOperation[j] < numMachines)
      {
        int op = currentOperation[j];                              // próxima operação a ser agendada para este job
        int machine = jobMachines[j][currentOperation[j]];         // máquina da próxima operação
        int duration = operationDurations[j][currentOperation[j]]; // duração da próxima operação

        // O tempo de início seria o máximo entre quando a máquina fica livre e quando o job fica livre
        int startTime = getMax(machinesFreeTime[machine], jobsFreeTime[j]);

        // Se este job tem um tempo de início melhor, escolhemos ele
        if (startTime < bestStartTime)
        {
          bestStartTime = startTime;
          bestJob = j;
        }
      }
    }

    /** Agendar o melhor job encontrado **/

    int op = currentOperation[bestJob];             // próxima operação a ser agendada para o melhor job
    int machine = jobMachines[bestJob][op];         // máquina da próxima operação
    int duration = operationDurations[bestJob][op]; // duração da próxima operação

    // Calculamos as horas finais de entrada e saída
    int startTime = getMax(machinesFreeTime[machine], jobsFreeTime[bestJob]);
    int endTime = startTime + duration;

    // Guardamos o resultado no array de agendamento
    scheduleArray[bestJob][2 * op] = startTime;   // hora de entrada
    scheduleArray[bestJob][2 * op + 1] = endTime; // hora de saída

    // A máquina e o job ficam ocupados até ao "endTime", por isso não pode ser usadas antes disso
    machinesFreeTime[machine] = endTime;
    jobsFreeTime[bestJob] = endTime;

    // Se o job sair mais tarde do que o nosso tempo total atual, atualizamos o makespan
    if (endTime > totalMakespan)
    {
      totalMakespan = endTime;
    }

    // Avançamos este job para a sua próxima operação
    currentOperation[bestJob]++;

    // Se ele já fex a operação de todas as máquina (numMachine)
    if (currentOperation[bestJob] == numMachines)
    {
      jobsDone++; // este job já terminou todas as suas operações, incrementamos o contador de jobs feitos
    }

    // Print job machines
    printf("\nJob Machines:\n");
    for (int j = 0; j < numJobs; j++)
    {
      printf("Job %d: ", j);
      for (int op = 0; op < numMachines; op++)
      {
        printf("%d ", jobMachines[j][op]);
      }
      printf("\n");
    }

    // Print operation durations
    printf("\nOperation Durations:\n");
    for (int j = 0; j < numJobs; j++)
    {
      printf("Job %d: ", j);
      for (int op = 0; op < numMachines; op++)
      {
        printf("%d ", operationDurations[j][op]);
      }
      printf("\n");
    }

    // Print auxiliary array with machine free times
    printf("\nMachines Free Time:\n");
    for (int m = 0; m < numMachines; m++)
    {
      printf("Machine %d: %d\n", m, machinesFreeTime[m]);
    }

    // Print the schedule array with entry and exit times
    printf("\nSchedule Array (Entry and Exit Times):\n");
    for (int j = 0; j < numJobs; j++)
    {
      printf("Job %d: ", j);
      for (int op = 0; op < numMachines; op++)
      {
        printf("M%d:[%d-%d] ", jobMachines[j][op], scheduleArray[j][2 * op],
               scheduleArray[j][2 * op + 1]);
      }
      printf("\n");
    }
  }
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________

int main()
{

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
  double time_spent = (end_clock - start_clock) / 1000.0;

  // Print the time spent in seconds
  printf("Execution time: %f seconds \n", time_spent);

  return 0;
}

//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________
//_____________________________________________________________________________________________________________________________________________________