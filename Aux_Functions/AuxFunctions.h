#ifndef READ_FILE_H
#define READ_FILE_H

int readFiles(const char *fileName, char *finalString);
int fileToArray(const char *finalString, int (*dataArray)[200]);
void generateData(int numJobs, int numMachines);

#endif
