#ifndef READ_FILE_H
#define READ_FILE_H

int readFiles(const char *fileName, char *finalString);
void fileToArray(const char *finalString, int numJobs, int numMachines,
                 int dataArray[][2 * numMachines]);
void generateData(int numJobs, int numMachines);

#endif
